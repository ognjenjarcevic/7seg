#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()
//#include <string.h>
#include "gpio.h"

MODULE_LICENSE("Dual BSD/GPL");

#define DEV_NAME "gpio_bitbang"

#define DATA_BUFF_LEN 10

#define DEV_MAJOR 260

static char data_buffer[DATA_BUFF_LEN];

static int gpio_bitbang_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "GPIO_BITBANG Driver file opened");
	return 0;
}

static int gpio_bitbang_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "GPIO_BITBANG Driver file closed");
	return 0;
}

static ssize_t gpio_bitbang_read(struct file* filp, char* buf, size_t len, loff_t* f_pos)
{
	return 0;
}

static ssize_t gpio_bitbang_write(struct file* filp, const char *buf, size_t len, loff_t *f_pos)
{
	uint16_t instr;
	
	memset(data_buffer, 0, DATA_BUFF_LEN);
	
	if (copy_from_user(data_buffer, buf, len) != 0)
	{
		printk(KERN_INFO "GPIO_BITBANG Driver error reading");
		return -EFAULT;
	}
    else
    {
		//spi instruction incoming
		if (len == 2)
		{
			//printk(KERN_INFO "GPIO_BITBANG Driver: good instruction");
			instr = ((uint16_t)data_buffer[1]<<8) | (uint16_t)data_buffer[0];
			printk(KERN_INFO "bbb: %x|%x\n", data_buffer[1], data_buffer[0]);
			gpio__spi_instruction(instr);
		}
		else
		{
			printk(KERN_INFO "GPIO_BITBANG Driver: wrong instruction");
			return -1;
		}
	}

	return 2;
}

static struct file_operations gpio_bitbang_fops = {
	.open = gpio_bitbang_open,
	.release = gpio_bitbang_release,
	.read  = gpio_bitbang_read,
	.write = gpio_bitbang_write
};

int gpio_bitbang_init(void)
{
    int status = register_chrdev(DEV_MAJOR, DEV_NAME, &gpio_bitbang_fops);
	if(status < 0)
    {
		printk(KERN_INFO DEV_NAME": cannot obtain major number %d!\n", DEV_MAJOR);
		return status;
	}

    status = gpio__init();
	if(status){
		printk(KERN_INFO DEV_NAME": gpio__init() failed!\n");
		unregister_chrdev(DEV_MAJOR, DEV_NAME);
	}
    else
    {
        printk(KERN_INFO DEV_NAME": Inserting module successful.\n");
    }
	gpio__steer_pinmux(BITBANG_CLK_PIN, GPIO__OUT);
	gpio__steer_pinmux(BITBANG_DIN_PIN, GPIO__OUT);
	gpio__steer_pinmux(BITBANG_LOAD_PIN, GPIO__OUT);
    return status;
}

void gpio_bitbang_exit(void)
{
    printk(KERN_INFO DEV_NAME": Removing %s module\n", DEV_NAME);
    gpio__exit();
    unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

module_init(gpio_bitbang_init);
module_exit(gpio_bitbang_exit);
