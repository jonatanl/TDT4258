#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>     // alloc_crdev_region, unregister_chrdev_region
#include <linux/cdev.h>   // cdev_alloc, cdev_init, cdev_add


// necessary function prototypes
static int my_open(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
static int my_release(struct inode *inode, struct file *filp);



// major and minor device numbers
static dev_t dev;

// character device
static struct cdev my_cdev;

// file operations structure
static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_release,
  .read = my_read,
  .write = my_write,
};

// last error
static int err;



// Prints an error code together with a custom message
//
// TODO: Print the name of the corresponding error
static void print_error(int error, const char* message)
{
  printk(KERN_ALERT "%s: %d\n", message, error);
} 

// Called when a user process calls open()
static int my_open(struct inode *inode, struct file *filp)
{
  // TODO
  return -1;
} 

// Called when the last user process calls close()
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  // TODO
  return -1;
}

// Called when a user process calls read()
static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
  // TODO
  return -1;
} 

// Called when the last user process calls write()
static int my_release(struct inode *inode, struct file *filp)
{
  // TODO
  return -1;
}

// Initialize the gamepad module and insert it into kernel space
//
// Returns 0 if successfull, otherwise -1
static int __init gamepad_init(void)
{
	printk("Hello World, here is your module speaking\n");


  // Allocate device numbers
  err = alloc_chrdev_region(&dev, 0, 1, "driver-gamepad");
  if(err){
    print_error(err, "Error allocating device numbers");
    return -1; // TODO: Handle error
  }

  // Set up a character device
  cdev_init(&my_cdev, &my_fops);
  err = cdev_add(&my_cdev, dev, 1);
  if(err){
    print_error(err, "Error adding char device to the kernel");
    return -1; // TODO: Handle error
  }

	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
	printk("Short life for a small module...\n");

  // Remove the character device from the kernel
  cdev_del(&my_cdev);

  // Deallocate device numbers
  unregister_chrdev_region(dev, 1);
}



// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);



// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
