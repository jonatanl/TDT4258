#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>     // alloc_crdev_region, unregister_chrdev_region
#include <linux/cdev.h>   // cdev_alloc, cdev_init, cdev_add
#include <linux/err.h>    // IS_ERR
#include <linux/device.h> // class_create, device_create
#include <linux/ioport.h> // request_mem_region, ioremap_nocache
#include <linux/io.h>
#include <asm-generic/uaccess.h>  // ioread32


#define DRIVER_NAME "driver-gamepad"
#define DEVICE_NAME "driver-gamepad0"

#define DEVICE_LENGTH 1 // Length of the device in bytes


// GPIO memory region, port C
#define GPIO_PC_BASE 0x40006048
#define GPIO_PC_LENGTH 0x24

// GPIO registers, port C
#define GPIO_PC_CTRL     ((void*)(GPIO_PC_BASE + 0x00))
#define GPIO_PC_MODEL    ((void*)(GPIO_PC_BASE + 0x04))
#define GPIO_PC_MODEH    ((void*)(GPIO_PC_BASE + 0x08))
#define GPIO_PC_DOUT     ((void*)(GPIO_PC_BASE + 0x0c))
#define GPIO_PC_DOUTSET  ((void*)(GPIO_PC_BASE + 0x10))
#define GPIO_PC_DOUTCLR  ((void*)(GPIO_PC_BASE + 0x14))
#define GPIO_PC_DOUTTGL  ((void*)(GPIO_PC_BASE + 0x18))
#define GPIO_PC_DIN      ((void*)(GPIO_PC_BASE + 0x1c))
#define GPIO_PC_PINLOCKN ((void*)(GPIO_PC_BASE + 0x20))


// necessary function prototypes
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static loff_t gamepad_llseek(struct file *filp, loff_t offp, int whence);


// the first device number
static dev_t dev;

// character device
static struct cdev my_cdev;

// file operations structure
static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .open = gamepad_open,
  .release = gamepad_release,
  .read = gamepad_read,
  .llseek = gamepad_llseek,
};

// class and device structure used when creating device structure
static struct class *my_class;
static struct device *my_device;

// GPIO memory region
struct resource *gpio_region;
void *gpio_ptr;

// last error
static int err;


// Prints an error code together with a custom message
//
// TODO: Print the name of the corresponding error
static void print_error(int err, const char* message)
{
  printk(KERN_ALERT "%s: %d\n", message, err);
} 


// Called when a user process calls read()
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  if(*offp < DEVICE_LENGTH){

    // copy the first byte of GPIO_PC_DIN into the user buffer
    uint8_t value = ioread32(GPIO_PC_DIN) ^ 0xff;
    copy_to_user((void*)buff, (void*)&value, 1);
    (*offp)++;
    return 1;
  }else{

    // file offset beyond EOF
    return 0;
  }
} 

// Called when a user process calls open()
static int gamepad_open(struct inode *inode, struct file *filp)
{
  printk("gamepad_open()\n");

  // TODO
  return 0;
} 

// Called when the last user process calls write()
static int gamepad_release(struct inode *inode, struct file *filp)
{
  printk("gamepad_release()\n");

  // TODO
  return 0;
}

// Called when the last user process calls lseek()
static loff_t gamepad_llseek(struct file *filp, loff_t offp, int whence)
{
  // calculate new offset
  loff_t newpos;
  switch(whence)
  {
    case SEEK_SET:
      newpos = offp;
      break;

    case SEEK_CUR:
      newpos = filp->f_pos + offp;
      break;
      
    case SEEK_END:
      newpos = DEVICE_LENGTH + offp;
      break;
    
    default: // invalid whence
      return EINVAL;
  }

  if(newpos < 0) {
    return EINVAL;
  }
  filp->f_pos = newpos;
  return newpos;
}

static int setup_gpio(void)
{
  printk("Setting up GPIO:\n");

  // Allocate GPIO memory region for Port C
  printk("allocating GPIO memory ...\n");
  gpio_region = request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH, DEVICE_NAME);
  if(gpio_region == NULL){
    print_error(PTR_ERR(gpio_region), "Error allocating GPIO memory region");
    return -1; // TODO: Handle error
  }

  // Map GPIO memory region into virtual memory space
  printk("mapping GPIO memory into virtual memory space ...\n");
  gpio_ptr = ioremap_nocache(GPIO_PC_BASE, GPIO_PC_LENGTH);

  // Set up GPIO registers
  printk("setting up GPIO registers ...\n");
  iowrite32(0x33333333, GPIO_PC_MODEL); // Set pins 0-7 to input
  iowrite32(0xff, GPIO_PC_DOUT); // Enable pull-up

  printk("OK: No errors setting up GPIO.\n");
  return 0;
}

static void cleanup_gpio(void)
{
  printk("Cleaning up the GPIO:\n");

  // Reset GPIO registers
  printk("resetting GPIO registers ...\n");
  iowrite32(0x00000000, GPIO_PC_MODEL);
  iowrite32(0x00000000, GPIO_PC_DOUT);

  // Unmap GPIO memory region
  printk("unmapping GPIO memory ...\n");
  iounmap((void*)GPIO_PC_BASE);

  // Deallocate GPIO memory region
  printk("deallocating GPIO memory ...\n");
  release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);

  printk("OK: No errors during GPIO cleanup.\n");
  return;
}


// Initialize the gamepad module and insert it into kernel space
//
// Returns 0 if successfull, otherwise -1
static int __init gamepad_init(void)
{
  printk("Initializing the module:\n");

  setup_gpio();

  // Allocate device numbers
  printk("allocating device numbers ...\n");
  err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
  if(err){
    print_error(err, "Error allocating device numbers");
    return -1; // TODO: Handle error
  }

  // Set up a character device
  printk("setting up character device ...\n");
  cdev_init(&my_cdev, &my_fops);
  err = cdev_add(&my_cdev, dev, 1);
  if(err){
    print_error(err, "Error adding char device to the kernel");
    return -1; // TODO: Handle error
  }

  // Create a device file
  printk("creating device file...\n");
  my_class = class_create(THIS_MODULE, DEVICE_NAME);
  if(IS_ERR(my_class)){
    print_error(PTR_ERR(my_class), "Error creating class structure for device file");
    return -1; // TODO: Handle error
  }
  my_device = device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
  if(IS_ERR(my_device)){
    print_error(PTR_ERR(my_device), "Error creating device structure for device file");
    return -1; // TODO: Handle error
  }

  // Print OK and return
  printk("OK: No errors during module initialization.\n");
	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
  printk("Cleaning up the module:\n");

  // Destroy structures used to create the device file
	printk("Destroying structures used to create device file ...\n");
  device_destroy(my_class, dev);
  class_destroy(my_class);

  // Remove the character device from the kernel
	printk("Removing the character device from the kernel ...\n");
  cdev_del(&my_cdev);

  // Deallocate device numbers
	printk("Deallocating device numbers ...\n");
  unregister_chrdev_region(dev, 1);

  cleanup_gpio();

  printk("OK: No errors during module cleanup.\n");
  return;
}


// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);


// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
