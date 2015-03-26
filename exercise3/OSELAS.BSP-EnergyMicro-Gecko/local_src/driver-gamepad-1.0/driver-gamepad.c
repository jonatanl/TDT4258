#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>


// Initialize the gamepad module and insert it into kernel space
//
// Returns 0 if successfull, otherwise -1
static int __init gamepad_init(void)
{
	printk("Hello World, here is your module speaking\n");
	return 0;
}


// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
	 printk("Short life for a small module...\n");
}


// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);


// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
