#include <mach/mini_uart.h>
#include <mach/gpio.h>
#include <asm/io.h>

void uart_send(char c)
{
	while (1) {
		if (readl(AUX_MU_LSR_REG)&0x20)
			break;
	}
	writel(c, AUX_MU_IO_REG);
}

char uart_recv(void)
{
	while (1) {
		if (readl(AUX_MU_LSR_REG)&0x01)
			break;
	}
	return(readl(AUX_MU_IO_REG)&0xFF);
}

void uart_send_string(char *str)
{
	int i;

	for (i = 0; str[i] != '\0'; i++)
		uart_send((char)str[i]);
}

void dbg_puts(char *s)
{
	while (*s != '\0') {
		while (!(readl(AUX_MU_LSR_REG) & 0x20))
			;
		writel(*s, AUX_MU_IO_REG);
		s++;
	}
}

void uart_init(void)
{
	unsigned int selector;

	selector = readl(GPFSEL1);
	selector &= ~(7<<12);               /*clean gpio14*/
	selector |= 2<<12;                  /*set alt5 for gpio14*/
	selector &= ~(7<<15);               /*clean gpio15*/
	selector |= 2<<15;                  /*set alt5 for gpio15*/
	writel(selector, GPFSEL1);

	writel(0, GPPUD);
	delay(150);
	writel((1<<14)|(1<<15), GPPUDCLK0);
	delay(150);
	writel(0, GPPUDCLK0);


	/*
	 * Enable mini uart (this also enables
	 * access to it registers)
	 */
	writel(1, AUX_ENABLES);

	/*Disable auto flow control and disable
	 * receiver and transmitter (for now)
	 */
	writel(0, AUX_MU_CNTL_REG);

	/*
	 * Disable receive and transmit interrupts
	 */
	writel(0, AUX_MU_IER_REG);

	/*Enable 8 bit mode*/
	writel(3, AUX_MU_LCR_REG);

	/*Set RTS line to be always high*/
	writel(0, AUX_MU_MCR_REG);

	/*Set baud rate to 115200*/
	writel(270, AUX_MU_BAUD_REG);/*Set baud rate to 115200*/

	/*
	 * Finally, enable transmitter and receiver
	 */
	writel(3, AUX_MU_CNTL_REG);
}
