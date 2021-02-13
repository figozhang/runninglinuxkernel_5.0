#include <uart.h>

void kernel_main(void)
{
	uart_init();
	uart_send_string("=Welcome BenOS!\r\n");
	uart_send_string("i am benshushu!\r\n");

	while (1)
		uart_send(uart_recv());
}
