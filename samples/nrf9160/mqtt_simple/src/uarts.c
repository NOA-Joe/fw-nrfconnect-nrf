#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/uart.h>


#include "uarts.h"

#include <drivers/gpio.h>
#include <sys/util.h>
#include <inttypes.h>
#include <string.h>
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

#ifndef DT_ALIAS_LED0_GPIOS_FLAGS
#define FLAGS 0
#else
#define FLAGS DT_ALIAS_LED0_GPIOS_FLAGS
#endif

/* Make sure the board's devicetree declares led0 in its /aliases. */
#ifdef DT_ALIAS_LED0_GPIOS_CONTROLLER
#define LED0	DT_ALIAS_LED0_GPIOS_CONTROLLER
#define PIN	DT_ALIAS_LED0_GPIOS_PIN
#else
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#endif
#define SLEEP_TIME_MS	1

#ifndef DT_ALIAS_SW0_GPIOS_FLAGS
#define DT_ALIAS_SW0_GPIOS_FLAGS 0
#endif

#ifndef DT_ALIAS_LED0_GPIOS_FLAGS
#define DT_ALIAS_LED0_GPIOS_FLAGS 0
#endif


static K_FIFO_DEFINE(fifo_uart_tx_data);
static K_FIFO_DEFINE(fifo_uart_rx_data);
struct uart_data_t {
	void  *fifo_reserved;
	u8_t    data[1024];
	u16_t   len;
};
static u8_t uart_buf[128];
static u8_t uart_buf1[128];
#if 0
void uart_sendCOM(struct device *x,  u8_t *Cont)
{
     
         u16_t len = strlen(Cont); 
         uart_fifo_fill(x, Cont,len );
         uart_irq_tx_enable(x);
         
    
}
#endif
void uart_cb(struct device *x)
{
	uart_irq_update(x);
	int data_length = 0;

	if (uart_irq_rx_ready(x)) {
		data_length = uart_fifo_read(x, uart_buf, sizeof(uart_buf));
		uart_buf[data_length] = 0;
	}
	printk("%s", uart_buf);
        //uart_fifo_fill(x, uart_buf ,strlen(uart_buf)); 
#if 1
       	if (uart_irq_tx_ready(x)) {
               
        	struct uart_data_t *buf =
			k_fifo_get(&fifo_uart_tx_data, K_NO_WAIT);
		u16_t written = 0;

		/* Nothing in the FIFO, nothing to send */
		if (!buf) {
			uart_irq_tx_disable(x);
			return;
		}

		while (buf->len > written) {
			written += uart_fifo_fill(x,
						  &buf->data[written],
						  buf->len - written);
		}

		while (!uart_irq_tx_complete(x)) {
			/* Wait for the last byte to get
			 * shifted out of the module
			 */
		}

		if (k_fifo_is_empty(&fifo_uart_tx_data)) {
			uart_irq_tx_disable(x);
		}

		k_free(buf);
	
		
	}

#endif
}

void uart_cb1(struct device *x1)
{
	uart_irq_update(x1);
	int data_length = 0;

	if (uart_irq_rx_ready(x1)) {
		data_length = uart_fifo_read(x1, uart_buf1, sizeof(uart_buf1));
		uart_buf1[data_length] = 0;
	}
	printk("%s", uart_buf1);
}
  #if 0
void gpio_main(void)
{

	struct device *dev;
	bool led_is_on = true;
	int ret;

	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	while (1) {
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_sleep(SLEEP_TIME_MS);
	}


}
void button_pressed(struct device *dev, struct gpio_callback *cb,
		    u32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

static struct gpio_callback button_cb_data;

void button_main(void)
{
	struct device *dev_button;
	int ret;

	dev_button = device_get_binding(DT_ALIAS_SW0_GPIOS_CONTROLLER);
	if (dev_button == NULL) {
		printk("Error: didn't find %s device\n",
			DT_ALIAS_SW0_GPIOS_CONTROLLER);
		return;
	}

	ret = gpio_pin_configure(dev_button, DT_ALIAS_SW0_GPIOS_PIN,
				 DT_ALIAS_SW0_GPIOS_FLAGS | GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d '%s'\n",
			ret, DT_ALIAS_SW0_GPIOS_PIN, DT_ALIAS_SW0_LABEL);
		return;
	}

	ret = gpio_pin_interrupt_configure(dev_button, DT_ALIAS_SW0_GPIOS_PIN,
					   GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on pin %d '%s'\n",
			ret, DT_ALIAS_SW0_GPIOS_PIN, DT_ALIAS_SW0_LABEL);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed,
			   BIT(DT_ALIAS_SW0_GPIOS_PIN));
	gpio_add_callback(dev_button, &button_cb_data);

#ifdef DT_ALIAS_LED0_GPIOS_CONTROLLER
	struct device *dev_led;

	dev_led = device_get_binding(DT_ALIAS_LED0_GPIOS_CONTROLLER);
	if (dev_led == NULL) {
		printk("Error: didn't find %s device\n",
			DT_ALIAS_LED0_GPIOS_CONTROLLER);
		return;
	}

	ret = gpio_pin_configure(dev_led, DT_ALIAS_LED0_GPIOS_PIN,
				 DT_ALIAS_LED0_GPIOS_FLAGS | GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d '%s'\n",
			ret, DT_ALIAS_LED0_GPIOS_PIN, DT_ALIAS_LED0_LABEL);
		return;
	}
#endif
	printk("Press %s on the board\n", DT_ALIAS_SW0_LABEL);

	while (0) {//1
#ifdef DT_ALIAS_LED0_GPIOS_CONTROLLER
		bool val;

		val = gpio_pin_get(dev_button, DT_ALIAS_SW0_GPIOS_PIN);
		gpio_pin_set(dev_led, DT_ALIAS_LED0_GPIOS_PIN, val);
		k_sleep(SLEEP_TIME_MS);
#endif
	}
}


/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

struct printk_data_t {
	void *fifo_reserved; /* 1st word reserved for use by fifo */
	u32_t led;
	u32_t cnt;
};

 
K_FIFO_DEFINE(printk_fifo);
struct led {
	const char *gpio_dev_name;
	const char *gpio_pin_name;
	unsigned int gpio_pin;
	unsigned int gpio_flags;
};

void blink(const struct led *led, u32_t sleep_ms, u32_t id)
{
	struct device *gpio_dev;
	int cnt = 0;
	int ret;

	gpio_dev = device_get_binding(led->gpio_dev_name);
	__ASSERT(gpio_dev != NULL, "Error: didn't find %s device\n",
			led->gpio_dev_name);

	ret = gpio_pin_configure(gpio_dev, led->gpio_pin, led->gpio_flags);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d '%s'\n",
			ret, led->gpio_pin, led->gpio_pin_name);
		return;
	}

	while (1) {
		gpio_pin_set(gpio_dev, led->gpio_pin, cnt % 2);

		struct printk_data_t tx_data = { .led = id, .cnt = cnt };

		size_t size = sizeof(struct printk_data_t);
		char *mem_ptr = k_malloc(size);
		__ASSERT_NO_MSG(mem_ptr != 0);

		memcpy(mem_ptr, &tx_data, size);

		k_fifo_put(&printk_fifo, mem_ptr);

		k_sleep(sleep_ms);
		cnt++;
	}
}
 
void blink1(void)
{
	const struct led led1 = {
		.gpio_dev_name = DT_ALIAS_LED0_GPIOS_CONTROLLER,
		.gpio_pin_name = DT_ALIAS_LED0_LABEL,
		.gpio_pin = DT_ALIAS_LED0_GPIOS_PIN,
		.gpio_flags = GPIO_OUTPUT | DT_ALIAS_LED0_GPIOS_FLAGS,
	};

	blink(&led1, 100, 0);
}

void blink2(void)
{
	const struct led led2 = {
		.gpio_dev_name = DT_ALIAS_LED1_GPIOS_CONTROLLER,
		.gpio_pin_name = DT_ALIAS_LED1_LABEL,
		.gpio_pin = DT_ALIAS_LED1_GPIOS_PIN,
		.gpio_flags = GPIO_OUTPUT | DT_ALIAS_LED1_GPIOS_FLAGS,
	};

	blink(&led2, 1000, 1);
}

void uart_out(void)
{
	while (1) {
		struct printk_data_t *rx_data = k_fifo_get(&printk_fifo, K_FOREVER);
		printk("Toggle USR%d LED: Counter = %d\n", rx_data->led, rx_data->cnt);
		k_free(rx_data);
	}
}

K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY, 0, K_NO_WAIT);
K_THREAD_DEFINE(blink2_id, STACKSIZE, blink2, NULL, NULL, NULL,
		PRIORITY, 0, K_NO_WAIT);
K_THREAD_DEFINE(uart_out_id, STACKSIZE, uart_out, NULL, NULL, NULL,
		PRIORITY, 0, K_NO_WAIT);
    #endif

#define TESTSTRING  "NOA Labs\r\n"
void uart_main(void)
{

    #if 1
    struct device *uart1 = device_get_binding("UART_1");
    uart_irq_callback_set(uart1, uart_cb);
    uart_irq_rx_enable(uart1);

    uart_fifo_fill(uart1, TESTSTRING ,sizeof(TESTSTRING)); 
    //uart_tx(uart,TESTSTRING,sizeof(TESTSTRING),100);  // no work
    printk("UART loopback start!\n");
    while (0) {
	k_cpu_idle();
    }
    #endif
}