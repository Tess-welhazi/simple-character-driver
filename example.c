#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include<linux/fs.h>
#include<asm/uaccess.h> /* for put_user */
#define BUF_LEN 80		/* Max length of the message from the device */

/* Module info */
MODULE_LICENSE("GPL"); /* GPL licence to avoid kernel taint*/ 
MODULE_AUTHOR("Tesnime Welhazi");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

/*Major is the identifier of our driver*/ 
static int Major; 

/* prototypes, could also be seperated into an .h file */
static int example_init(void);
static void example_exit(void);

int simple_char_driver_open(struct inode *, struct file *);
int simple_char_driver_close(struct inode *, struct file *);
ssize_t simple_char_driver_read(struct file *, char *, size_t, loff_t *);
ssize_t simple_char_driver_write(struct file *, const char *, size_t, loff_t *);

/* defined in linux/fs.h holds pointers to functions defined by the driver that perform various operations on the device. */ 
struct file_operations fops = {
	
	.read    = simple_char_driver_read,
	.write   = simple_char_driver_write,
	.open    = simple_char_driver_open,
	.release = simple_char_driver_close
};

/* counter variables for how many times our driver opened and closed */
int openCount = 0;
int closeCount = 0;

static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

/* init function, used when adding the driver to the system */
static int __init example_init(void) {
 
	/*register_chrdev assigns a major number on it's own if you put "0" */ 
 Major = register_chrdev(0, "example", &fops);
 printk(KERN_INFO "Hello, World!\n");
 printk(KERN_INFO "my example device drivers major is: %d\n", Major);

  if (Major < 0) {
    printk(KERN_ALERT "Could not register device with Major %d\n", Major);
    return Major;
  }

 return 0;
}

/* called when module is removed. we free the assigned major from our driver */
static void __exit example_exit(void) {
 printk(KERN_INFO "Goodbye, World!\n");
 unregister_chrdev(Major, "example");
}

/*Called when a process tries to open the device file in my example with cat <device file name> */

int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	
	openCount++;
	printk(KERN_ALERT "OPENING Simple Character Driver. It has been opened %d times\n", openCount);

	sprintf(msg, "bonjour mp2l\n");
	msg_Ptr = msg;
	return 0;
}

ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}


	/* 
	 * read functions are supposed to return the number of bytes put into the buffer. idk why.
	 */
	return bytes_read;
}

ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){

	printk(KERN_ALERT "cannot handle write yet");
	return -EINVAL;
}


int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	closeCount++;
	printk(KERN_ALERT "CLOSING Simple Character Driver. It has been closed %d times\n", closeCount);
	return 0;
}

