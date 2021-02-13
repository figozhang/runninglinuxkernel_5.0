#include <uart.h>
#include <asm/sysregs.h>

void kernel_main(void)
{
	int el;

	uart_init();
	uart_send_string("=Welcome BenOS!\r\n");
	uart_send_string("i am benshushu!\r\n");

	uart_send_string("running on EL");

	el = get_currentel() >> 2;
	uart_send(48+el);

	while (1)
		uart_send(uart_recv());
}
