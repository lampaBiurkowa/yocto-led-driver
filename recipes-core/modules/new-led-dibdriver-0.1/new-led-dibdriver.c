#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lampaBiutkowa");
MODULE_DESCRIPTION("Driver for built-in led diodes");

#define LLL_MAX_USER_SIZE 2

static struct proc_dir_entry *control_led_proc = NULL;
static struct gpio_desc *led1, *led2 = NULL;

static char data_buffer[LLL_MAX_USER_SIZE + 1] = {};

static int on_probe(struct platform_device *pdev);
static int on_remove(struct platform_device *pdev);
static ssize_t set_leds(struct file *File, const char *user_buffer, size_t count, loff_t *offs);

//based on https://github.com/PacktPublishing/Linux-Device-Drivers-Development/blob/master/Chapter14/gpio-descriptor-module.c
static const struct of_device_id gpiod_dt_ids[] =
{
    { .compatible = "packt,gpio-descriptor-sample", },
    { /* sentinel */ }
};

static struct platform_driver led_driver = {
	.probe = on_probe,
	.remove = on_remove,
	.driver =
	{
		.name = "gpio-descriptor-sample",
		.of_match_table = gpiod_dt_ids,
	},
};

static struct proc_ops fops =
{
	.proc_write = set_leds,
};


static void set_led(struct gpio_desc *diode, uint8_t value)
{
	if (value == 0 || value == 1)
		gpiod_set_value(diode, value);
}

static ssize_t set_leds(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
	memset(data_buffer, 0x0, sizeof(data_buffer));
	if (count > LLL_MAX_USER_SIZE)
		count = LLL_MAX_USER_SIZE;

	if (copy_from_user(data_buffer, user_buffer, count))
		return 0;

	set_led(led1, data_buffer[0] - '0');
	if (count > 1)
		set_led(led2, data_buffer[1] - '0');
	
	return count;
}

static int on_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    led1 = devm_gpiod_get_index_optional(dev, "led", 0, GPIOD_OUT_LOW);
    led2 = devm_gpiod_get_index_optional(dev, "led", 1, GPIOD_OUT_LOW);
	if(IS_ERR(led1))
	{
		printk("Failure getting led 1\n");
		return -1 * IS_ERR(led1);
	}

	if(IS_ERR(led2))
	{
		printk("Failure getting led 2\n");
		return -1 * IS_ERR(led1);
	}
	
    pr_info("Device probed\n");
    return 0;
}

static int on_remove(struct platform_device *pdev)
{
    gpiod_put(led1);
    gpiod_put(led2);
	proc_remove(control_led_proc);
    pr_info("Device removed\n");
    return 0;
}

static int __init my_init(void)
{
	printk("Loading the driver\n");
	if(platform_driver_register(&led_driver))
	{
		printk("Error loading the driver\n");
		return -1;
	}
	
	control_led_proc = proc_create("control-led", 0666, NULL, &fops); //set permissions
	if (control_led_proc == NULL)
		return -2;

	return 0;
}

static void __exit my_exit(void)
{
	platform_driver_unregister(&led_driver);
	printk("Driver unloaded");
}

module_init(my_init);
module_exit(my_exit);