#include "uart.h"

void kernel_main(void)
{
	uart_init();
	uart_send_string("Welcome BenOS!\r\n");

	while (1) {
		uart_send(uart_recv());
	}
}
