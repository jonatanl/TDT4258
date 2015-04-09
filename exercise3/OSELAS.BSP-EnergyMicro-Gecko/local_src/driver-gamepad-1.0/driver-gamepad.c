#define DEBUG // enable debugging: pr_debug(), dev_dbg()

#include <linux/kernel.h>         // kernel stuff
#include <linux/module.h>         // module stuff
#include <linux/sched.h>          // scheduling and sleeping functions
#include <linux/init.h>           // module_init(), module_exit()
#include <linux/fs.h>             // alloc_crdev_region(), unregister_chrdev_region()
#include <linux/cdev.h>           // cdev_alloc(), cdev_init(), cdev_add()
#include <linux/err.h>            // IS_ERR(), ERR_PTR(), PTR_ERR()
#include <linux/device.h>         // class_create(), device_create()
#include <linux/ioport.h>         // request_mem_region(), ioremap_nocache()
#include <linux/io.h>             // iowrite*(), ioread*()
#include <linux/interrupt.h>      // request_irq(), free_irq()
#include <linux/slab.h>           // kmalloc(), kfree()
#include <linux/spinlock.h>       // init_spin_lock(), spin_lock()
#include <linux/wait.h>           // wait_queue_head_t
#include <linux/poll.h>           // poll_table
#include "efm32gg.h"              // efm32gg hardware definitions

// platform-specific IRQ numbers
#define GPIO_EVEN_IRQ 17
#define GPIO_ODD_IRQ  18

// function prototypes
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static unsigned int gamepad_poll(struct file *filp, poll_table *wait);
static int gamepad_fasync(int fd, struct file *filp, int mode);
static int setup_gpio_input(void);
static int setup_gpio_interrupts(void);
static void cleanup_gpio_input(void);
static void cleanup_gpio_interrupts(void);
static void work_tasklet(unsigned long unused);



////----------------------------------------------------------
//// Global declarations
////----------------------------------------------------------
//
// Device structure containing data specific to the gamepad device.
static struct gamepad_device
{
  dev_t dev;             // first device number
  struct cdev cdev;      // char structure
  struct semaphore sem;
  int open_count;        // open instances of this device
  struct fasync_struct *async_queue; // keeps track of processes listening for SIGIO
} my_gamepad;
#define DEVICE_NAME "gamepad"

// A file operations structure with pointers to the driver implementations of
// the supported file operations.
static struct file_operations my_fops = {
  .owner   = THIS_MODULE,
  .open    = gamepad_open,
  .release = gamepad_release,
  .read    = gamepad_read,
  .poll    = gamepad_poll,    // polling for available data
  .fasync  = gamepad_fasync,  // asyncronous notification
  .llseek  = no_llseek,  // seeking is not supported
};

// A cyclic input buffer used to store subsequent states of GPIO_PC_DIN.
//
// data - memory used to store states
// size - number of states stored in 'data'
// next_write - a counter increased on each write. The next index to write a
//              value is calculated as 'next_write % size'.
// sem - provides mutual exclusive access to 'data' and 'next_write'.
// read_queue - blocked readers wait here
static struct input_buffer
{
  uint8_t *data;
  uint32_t size;
  uint32_t next_write;
  struct semaphore sem;
  wait_queue_head_t read_queue;
} my_buffer;
#define BT_BUFFER_SIZE 64

// Work structure and tasklet input variable, used by the interrupt handler and
// the scheduled tasklet.
struct work_struct my_work;
uint8_t my_work_input;

// Class and device structure used when creating the device file.
static struct class *my_class;
static struct device *my_device;
#define CLASS_NAME  "gamepad-class"

// GPIO memory regions, used when requesting access to GPIO memory.
struct resource *gpio_pc_region;
struct resource *gpio_irq_region;

// GPIO memory pointers: For portability, this driver maps GPIO memory into
// virtual process memory. The virtual memory must be accessed with special
// pointers.
void *gpio_pc_ptr;  
void *gpio_irq_ptr;

// Error variable used to store last error.
static int err = 0;


////----------------------------------------------------------
//// Implementation of file operations
////----------------------------------------------------------
//
// Initialize device and file resources on open()
static int gamepad_open(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Opening device:\n");

  // initialize device on first open
  down(&my_gamepad.sem);
  if(my_gamepad.open_count == 0){
    dev_dbg(my_device, "No open instances. Initialize device data:\n");

    // initialize input buffer
    my_buffer.data = (uint8_t*) kmalloc(BT_BUFFER_SIZE, GFP_KERNEL);
    if(IS_ERR(my_buffer.data)){ return -1; } // TODO: Handle error
    my_buffer.size = BT_BUFFER_SIZE;
    my_buffer.next_write = 0;
    sema_init(&my_buffer.sem, 1);
    init_waitqueue_head(&my_buffer.read_queue);
    dev_dbg(my_device, "OK: Initialized input buffer\n");

    // initialize buffer work
    INIT_WORK(&my_work, work_tasklet);

    // setup GPIO to receive interrupts
    err = setup_gpio_input();
    if(err){ return -1; } // TODO: Handle error
    err = setup_gpio_interrupts();
    if(err){ return -1; } // TODO: Handle error
  }
  my_gamepad.open_count++;
  up(&my_gamepad.sem);

  // initialize private device data
  filp->private_data = kmalloc(sizeof(uint32_t), GFP_KERNEL);
  if(IS_ERR(filp->private_data)){ return -1; } // TODO: Handle error
  *((uint32_t*)filp->private_data) = my_buffer.next_write;
  dev_dbg(my_device, "OK: Initialized private device data\n");

  dev_dbg(my_device, "DONE: No errors opening device\n");
  return 0;
} 

// Release device and file resources.
//
// Called when a user process closes the last open instance of a device file.
static int gamepad_release(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Releasing device:\n");

  // remove this file from the list of active asynchronous readers
  gamepad_fasync(-1, filp, 0);
  dev_dbg(my_device, "OK: Removed file from list of asynchronous readers\n");

  // destroy private device data
  kfree(filp->private_data);
  dev_dbg(my_device, "OK: Destroyed private device data\n");

  // aquire device mutex
  down(&my_gamepad.sem);
  my_gamepad.open_count--;

  // shut down device on last close
  if(my_gamepad.open_count == 0){
    dev_dbg(my_device, "No other open instances. Shutting down device:\n");

    // release GPIO interrupt resources
    cleanup_gpio_interrupts();

    // release GPIO input resources
    cleanup_gpio_input();

    // destroy input buffer
    kfree(my_buffer.data);
    my_buffer.size = 0;
    my_buffer.next_write = 0;
    dev_dbg(my_device, "OK: Destroyed input buffer\n");

    dev_dbg(my_device, "DONE: No errors shutting down device\n");
  }
  up(&my_gamepad.sem);

  dev_dbg(my_device, "DONE: No errors releasing device\n");
  return 0;
}

// Copy data from input buffer to user space.
//
// TODO: Support blocking IO.
// TODO: Protect 'next_read' with a mutex to avoid race conditions where
// several forked processes uses the same file structure.
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{ 
  uint32_t next_read;  // next value to read from the input buffer
  int interrupted;

  // aquire buffer mutex
  interrupted = down_interruptible(&my_buffer.sem);
  if(interrupted){
    return -ERESTARTSYS; // if interrupted, system call must be restarted
  }

  // wait for data to arrive
  next_read = *(uint32_t*)filp->private_data;
  while(next_read == my_buffer.next_write){

    // release buffer mutex
    up(&my_buffer.sem);

    // if IO is nonblocking, return "try again" error
    if(filp->f_flags & O_NONBLOCK){
      return -EAGAIN;
    }
    
    // wait for data
    interrupted = wait_event_interruptible(my_buffer.read_queue, (next_read != my_buffer.next_write));
    if(interrupted){
      return -ERESTARTSYS; // if interrupted, system call must be restarted
    }

    // aquire buffer mutex
    interrupted = down_interruptible(&my_buffer.sem);
    if(interrupted){
      return -ERESTARTSYS; // if interrupted, system call must be restarted
    }
  }

  // read one byte into 'buff' and increase read pointer
  err = copy_to_user(buff, &my_buffer.data[next_read % my_buffer.size], 1); 
  if(err){ // some memory was not copied: an error occurred
    up(&my_buffer.sem);
    return -EFAULT;
  }else{ // everything is okay
    *(uint32_t*)filp->private_data += 1;
  }

  // release buffer mutex
  up(&my_buffer.sem);
  return 1;
} 

// Lets user processes wait for some driver operations to become available.
//
// A user process calls poll(), select(), or epoll(), specifying a set of
// operations to wait for on the device file. The user process is then blocked
// until those operations are available.
//
// This driver only supports normal read operations, indicated by the flags
// POLLIN and POLLRDNORM.
static unsigned int gamepad_poll(struct file *filp, poll_table *wait)
{
  unsigned int mask = 0;
  uint32_t next_read;

  // aquire buffer mutex
  down(&my_buffer.sem);

  // specify the wait queue of the operation
  poll_wait(filp, &my_buffer.read_queue, wait);

  // if data is available, set mask to indicate that a read operation can
  // succeed without blocking
  next_read = *(uint32_t*) filp->private_data;
  if(next_read != my_buffer.next_write){
    mask |= POLLIN | POLLRDNORM;
  }

  // release buffer mutex
  up(&my_buffer.sem);

  return mask;
}

// Called whenever a user process sets the F_ASYNC bit on the device file.
static int gamepad_fasync(int fd, struct file *filp, int mode)
{
  // add or remove a file from the list of asynchronous readers
  return fasync_helper(fd, filp, mode, &my_gamepad.async_queue);
}


////----------------------------------------------------------
//// Interrupt handler and tasklet
////----------------------------------------------------------
//
// Interrupt handler for GPIO interrupts.
//
// This handler reads GPIO_PC_DIN into 'my_work_input', then schedules a
// tasklet that copies the result into the input buffer.
static irqreturn_t gpio_handler(int irq, void *dev_id)
{
  dev_dbg(my_device, "(interrupt on irq = %d)\n", irq);

  // while tasklet is busy, no new interrupts are handled
  if(work_busy(&my_work)){
    dev_dbg(my_device, "(interrupt missed!)\n");
    iowrite32(0xff, gpio_irq_ptr + GPIO_IFC); // clear interrupts
    return IRQ_NONE;
  }

  // read GPIO_PC_DIN into 'button_state'
  my_work_input = ioread8(gpio_pc_ptr + GPIO_PC_DIN) ^ 0xff;

  // schedule tasklet to copy register for us
  schedule_work(&my_work);

  // clear all interrupts
  iowrite32(0xff, gpio_irq_ptr + GPIO_IFC);
  return IRQ_HANDLED;
}

// Tasklet that copies a value into the input buffer.
static void work_tasklet(unsigned long unused)
{
  dev_dbg(my_device, "(tasklet started)\n");

  // write value to buffer and increase write count
  down(&my_buffer.sem);
  my_buffer.data[my_buffer.next_write % my_buffer.size] = my_work_input;
  my_buffer.next_write++;
  up(&my_buffer.sem);

  // signal asynchronous readers
  kill_fasync(&my_gamepad.async_queue, SIGIO, POLL_IN);

  // wake up blocked readers
  wake_up_interruptible(&my_buffer.read_queue);
}


////----------------------------------------------------------
//// Initialization and exit functions
////----------------------------------------------------------
//
// Initialize the gamepad module and insert it into kernel space.
static int __init gamepad_init(void)
{
  pr_debug("Initializing the module:\n");

  // allocate device numbers
  err = alloc_chrdev_region(&my_gamepad.dev, 0, 1, DEVICE_NAME);
  if(err){ return -1; } // TODO: Handle error
  pr_debug("OK: Allocated device numbers\n");

  // initialize cdev structure
  cdev_init(&my_gamepad.cdev, &my_fops);

  // initialize mutex and open count
  sema_init(&my_gamepad.sem, 1);
  my_gamepad.open_count = 0;

  // set NULL, or fasync_helper() will think its nonempty
  my_gamepad.async_queue = NULL;

  // NOTE: the device input buffer is initialized on first open()

  // add cdev structure to kernel
  err = cdev_add(&my_gamepad.cdev, my_gamepad.dev, 1);
  if(err){ return -1; } // TODO: Handle error
  pr_debug("OK: Added cdev structure to kernel\n");

  // create a device file
  my_class = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(my_class)){ return -1; } // TODO: Handle error
  my_device = device_create(my_class, NULL, my_gamepad.dev, NULL, DEVICE_NAME);
  if(IS_ERR(my_device)){ return -1; } // TODO: Handle error
  pr_debug("OK: Device file created\n");

  pr_debug("DONE: No errors initializing the module\n");
	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space.
static void __exit gamepad_exit(void)
{
  pr_debug("Cleaning up the module:\n");

  // destroy structures used to create the device file
  device_destroy(my_class, my_gamepad.dev);
  class_destroy(my_class);
	pr_debug("OK: Destroyed structures used to create the device file\n");

  // remove cdev structure from the kernel
  cdev_del(&my_gamepad.cdev);
	pr_debug("OK: Removed cdev structure from the kernel\n");

  // deallocate device numbers
  unregister_chrdev_region(my_gamepad.dev, 1);
	pr_debug("OK: Deallocated device numbers\n");

  pr_debug("DONE: No errors cleaning up the module\n");
  return;
}



////----------------------------------------------------------
//// GPIO setup and cleanup functions
////----------------------------------------------------------
static int setup_gpio_input(void)
{
  dev_dbg(my_device, "Setting up GPIO input:\n");

  // allocate memory region for port C registers
  gpio_pc_region = request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_pc_region)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Allocated memory for port C registers\n");

  // map region into virtual memory
  gpio_pc_ptr = ioremap_nocache(GPIO_PC_BASE, GPIO_PC_LENGTH);
  if(IS_ERR(gpio_pc_ptr)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Mapped port C memory region to virtual memory\n");

  // configure input registers
  iowrite32(0x33333333, gpio_pc_ptr + GPIO_PC_MODEL); // set pins 0-7 to input
  iowrite32(0x000000ff, gpio_pc_ptr + GPIO_PC_DOUT);  // enable pull-up
  dev_dbg(my_device, "OK: Configured input registers\n");

  dev_dbg(my_device, "DONE: No errors setting up GPIO input\n");
  return 0;
}

static void cleanup_gpio_input(void)
{
  dev_dbg(my_device, "Cleaning up GPIO input:\n");

  // reset input registers
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_MODEL);
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_DOUT);
  dev_dbg(my_device, "OK: Reset input registers\n");

  // unmap memory region for input registers
  iounmap(gpio_pc_ptr);
  dev_dbg(my_device, "OK: Unmapped memory region for input registers\n");

  // deallocate memory region for input registers
  release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);
  dev_dbg(my_device, "OK: Deallocated memory region for input registers\n");

  dev_dbg(my_device, "DONE: No errors cleaning up GPIO input\n");
}

static int setup_gpio_interrupts(void)
{
  dev_dbg(my_device, "Setting up GPIO interrupts:\n");

  // allocate memory region for interrupt registers
  gpio_irq_region = request_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_irq_region)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Allocated memory for interrupt registers\n");

  // map region into virtual memory
  gpio_irq_ptr = ioremap_nocache(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);
  if(IS_ERR(gpio_irq_ptr)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Mapped interrupt memory region to virtual memory\n");

  // configure interrupt registers
  iowrite32(0x22222222, gpio_irq_ptr + GPIO_EXTIPSELL); // Set port C as interrupt source
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIFALL);  // Set interrupt on 1->0
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIRISE);  // Set interrupt on 0->1
  dev_dbg(my_device, "OK: Configured interrupt registers\n");

  // register interrupt handler
  int flags = 0;
  err = request_irq(GPIO_EVEN_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){ return -1; } // TODO: Handle error
  err = request_irq(GPIO_ODD_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Registered interrupt handler\n");

  // enable interrupts
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_IEN);
  dev_dbg(my_device, "OK: Enabled interrupts\n");

  dev_dbg(my_device, "DONE: No errors setting up GPIO interrupts\n");
  return 0;
}

static void cleanup_gpio_interrupts(void)
{
  dev_dbg(my_device, "Cleaning up GPIO interrupts:\n");

  // disable interrupts
  iowrite32(0x0, gpio_irq_ptr + GPIO_IEN);
  dev_dbg(my_device, "OK: Disabled interrupts\n");

  // unregister interrupt handler
  free_irq(GPIO_EVEN_IRQ, NULL);
  free_irq(GPIO_ODD_IRQ, NULL);
  dev_dbg(my_device, "OK: Unregistered interrupt handler\n");

  // reset interrupt registers
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIPSELL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIFALL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIRISE);
  dev_dbg(my_device, "OK: Reset interrupt registers\n");

  // unmap memory region for interrupt registers
  iounmap(gpio_irq_ptr);
  dev_dbg(my_device, "OK: Unmapped memory region for interrupt registers\n");

  // deallocate memory region for interrupt registers
  release_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);
  dev_dbg(my_device, "OK: Deallocated memory region for interrupt registers\n");

  dev_dbg(my_device, "DONE: No errors cleaning up GPIO interrupts\n");
}



////----------------------------------------------------------
//// Various: placed at EOF according to convention
////----------------------------------------------------------

// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);

// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
