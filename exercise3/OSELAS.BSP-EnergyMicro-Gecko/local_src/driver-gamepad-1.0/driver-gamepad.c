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


#define DEVICE_NAME "driver-gamepad"



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

// Prints an error pointer together with a custom message
//
// TODO: Print a name corresponding to the error instead of using cast
static void print_error_ptr(void *err_ptr, const char* message)
{
  printk(KERN_ALERT "%s: %d\n", message, (int)err_ptr);
} 

// Called when a user process calls read()
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  // Copy the first byte of GPIO_PC_DIN into the user buffer
  uint8_t value = ioread32(GPIO_PC_DIN) ^ 0xff;
  copy_to_user((void*)buff, (void*)&value, 1);
  return 1;
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

static int setup_gpio(void)
{
  // Allocate GPIO memory region for Port C
  gpio_region = request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH, DEVICE_NAME);
  if(gpio_region == NULL){
    print_error_ptr(NULL, "Error allocating GPIO memory region");
    return -1;
  }

  // Map GPIO memory region into virtual memory space
  gpio_ptr = ioremap_nocache(GPIO_PC_BASE, GPIO_PC_LENGTH);

  // Set pins 0-7 to input
  iowrite32(0x33333333, GPIO_PC_MODEL);

  // Enable pull-up
  iowrite32(0xff, GPIO_PC_DOUT);
}

static void teardown_gpio(void)
{
  // Reset register values
  iowrite32(0x00000000, GPIO_PC_MODEL);
  iowrite32(0x00000000, GPIO_PC_DOUT);

  // Unmap GPIO memory region
  iounmap((void*)GPIO_PC_BASE);

  // Deallocate GPIO memory region
  release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);
}


// Initialize the gamepad module and insert it into kernel space
//
// Returns 0 if successfull, otherwise -1
static int __init gamepad_init(void)
{
  setup_gpio();

  // Allocate device numbers
  printk("Allocating device numbers ...\n");
  err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
  if(err){
    print_error(err, "Error allocating device numbers");
    return -1; // TODO: Handle error
  }
  printk("Allocated device numbers from (%d,%d).\n", MAJOR(dev), MINOR(dev));

  // Set up a character device
  printk("Setting up a character device ...\n");
  cdev_init(&my_cdev, &my_fops);
  err = cdev_add(&my_cdev, dev, 1);
  if(err){
    print_error(err, "Error adding char device to the kernel");
    return -1; // TODO: Handle error
  }
  printk("No error setting up a character device.\n");

  // Create a device file
  printk("Create a device file...\n");
  my_class = class_create(THIS_MODULE, DEVICE_NAME);
  if(IS_ERR(my_class)){
    print_error_ptr(my_class, "Error creating class structure for device file");
    return -1; // TODO: Handle error
  }
  my_device = device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
  if(IS_ERR(my_device)){
    print_error_ptr(my_device, "Error creating device structure for device file");
    return -1; // TODO: Handle error
  }
  printk("No error creating device file.\n");

  printk("Done setting up the module resources.\n");
	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
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

  teardown_gpio();

  printk("Done cleaning up the module resources.\n");
  return;
}



// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);



// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
