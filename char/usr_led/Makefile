KERN_DIR = /work/100ask_imx6ull-sdk/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules 
	$(CROSS_COMPILE)gcc -o ledtest led_test.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f ledtest

100ask_led-y := drv_usr_led.o board_100ask_imx6ull.o
obj-m	+= 100ask_led.o

