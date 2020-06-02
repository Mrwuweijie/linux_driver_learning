#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

#include "board.h"

static int led_major;
static struct class *led_class ;
static BOARD_LED_OPERATIONS_T *p_board_led_operation ;

static int usr_led_open(struct inode *p_inode, struct file *p_file);
static ssize_t usr_led_write(struct file *p_file, const char __user *p_data, size_t length, loff_t *p_offset);
static int usr_led_release(struct inode *p_inode, struct file *p_file);

static struct file_operations led_ops = {
	.owner			= THIS_MODULE,
	.open			= usr_led_open,
	.write			= usr_led_write,
	.release		= usr_led_release,
};

static int usr_led_open(struct inode *p_inode, struct file *p_file)
{
	int minor = iminor(p_inode);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 根据次设备号初始化LED */
	p_board_led_operation->init(minor);
	return 0;
}

static ssize_t usr_led_write(struct file *p_file, const char __user *p_data, size_t length, loff_t *p_offset)
{
	int err;
	char status;
	struct inode *inode = file_inode(p_file);
	int minor = iminor(inode);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(&status, p_data, 1);

	/* 根据次设备号和status控制LED */
	p_board_led_operation->control(minor, status);
	
	return 0;
}

static int usr_led_release(struct inode *p_inode, struct file *p_file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static __init int drv_usr_led_init(void)
{
	int err_code;
	int i;
	
	printk(KERN_DEBUG "%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	led_major = register_chrdev(0, "usr_100ask_led", &led_ops);

	led_class = class_create(THIS_MODULE, "usr_100ask_led_class");
	if (IS_ERR(led_class))
	{
		unregister_chrdev(led_major, "usr_100ask_led");
		return -ENODEV;
	}

	p_board_led_operation = board_get_led_info();
	
	for (i = 0; i < p_board_led_operation->led_num_max;  i ++)
	{
		device_create(led_class, NULL, MKDEV(led_major, i), NULL, "usr_led%d", i);
	}
	
	return 0;
}

static __exit void drv_usr_led_exit(void)
{
	int i;
	
	printk(KERN_DEBUG "%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	for (i = 0; i < p_board_led_operation->led_num_max;  i ++)
	{
		device_destroy(led_class, MKDEV(led_major, i));
	}
	class_destroy(led_class);
	unregister_chrdev(led_major, "usr_100ask_led");
}

module_init(drv_usr_led_init);
module_exit(drv_usr_led_exit);
MODULE_LICENSE("GPL");

