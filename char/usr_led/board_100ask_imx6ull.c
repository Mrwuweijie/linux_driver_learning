#include "board.h"
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
#include <asm/io.h>

#define BOARD_LED_NUM_MAX		(1)
#define LED_ACTIVE_LEVEL		(false)

//@TODO: add lock.
static volatile unsigned int *CCM_CCGR1;
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
static volatile unsigned int *GPIO5_GDIR;
static volatile unsigned int *GPIO5_DR;

static int board_usr_led_init(int led_index);
static int board_led_control(int led_index, int led_onoff);

static BOARD_LED_OPERATIONS_T board_led_operations = {
	.led_num_max		= BOARD_LED_NUM_MAX,
	.init				= board_usr_led_init,
	.control			= board_led_control,
};

static int board_usr_led_init(int led_index)
{
	unsigned int val;
	if (led_index > (BOARD_LED_NUM_MAX - 1))
		return -EINVAL;

	if (led_index == 0)
	{
		if (!CCM_CCGR1)
		{
		    CCM_CCGR1                               = ioremap(0x20C406C, 4);
		    IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290014, 4);
		    GPIO5_GDIR                              = ioremap(0x020AC000 + 0x4, 4);
		    GPIO5_DR                                = ioremap(0x020AC000 + 0, 4);
		}
	}
	/* GPIO5_IO03 */
	/* a. 使能GPIO5
	 * set CCM to enable GPIO5
	 * CCM_CCGR1[CG15] 0x20C406C
	 * bit[31:30] = 0b11
	 */
	*CCM_CCGR1 |= (3<<30);

	/* b. 设置GPIO5_IO03用于GPIO
	 * set IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3
	 *      to configure GPIO5_IO03 as GPIO
	 * IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3  0x2290014
	 * bit[3:0] = 0b0101 alt5
	 */
	val = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
	val &= ~(0xf);
	val |= (5);
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = val;


	/* b. 设置GPIO5_IO03作为output引脚
	 * set GPIO5_GDIR to configure GPIO5_IO03 as output
	 * GPIO5_GDIR  0x020AC000 + 0x4
	 * bit[3] = 0b1
	 */
	*GPIO5_GDIR |= (1<<3);
	
	return 0;
}

static int board_led_control (int led_index, int led_onoff)
{
	if (led_index > (BOARD_LED_NUM_MAX - 1))
		return -EINVAL;
		
    if (led_index== 0)
    {
        if (led_onoff) /* on: output 0*/
        {
            /* d. 设置GPIO5_DR输出低电平
             * set GPIO5_DR to configure GPIO5_IO03 output 0
             * GPIO5_DR 0x020AC000 + 0
             * bit[3] = 0b0
             */
            *GPIO5_DR &= ~(1<<3);
        }
        else  /* off: output 1*/
        {
            /* e. 设置GPIO5_IO3输出高电平
             * set GPIO5_DR to configure GPIO5_IO03 output 1
             * GPIO5_DR 0x020AC000 + 0
             * bit[3] = 0b1
             */ 
            *GPIO5_DR |= (1<<3);
        }
    
    }
    return 0;
}

__inline BOARD_LED_OPERATIONS_T *board_get_led_info(void)
{
	return &board_led_operations;
}

