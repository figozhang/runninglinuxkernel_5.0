#include <mach/uart.h>
#include <mach/gpio.h>
#include <asm/io.h>

#define UART_REFERENCE_CLOCK  (48000000)

void uart_send(char c)
{
	/* wait for transmit FIFO to have an available slot*/
	while (readl(U_FR_REG) & (1<<5))
		;

	writel(c, U_DATA_REG);
}

char uart_recv(void)
{
	/* wait for receive FIFO to have data to read */
	while (readl(U_FR_REG) & (1<<4))
		;

	return(readl(U_DATA_REG) & 0xFF);
}

void uart_send_string(char *str)
{
	int i;

	for (i = 0; str[i] != '\0'; i++)
		uart_send((char) str[i]);
}

void uart_init(void)
{
	unsigned int selector;
	unsigned int bauddiv;
	unsigned int ibrd;
	unsigned int fbrd;
	unsigned int baud_rate = 115200;

	/* clean gpio4 */
	selector = readl(GPFSEL1);
	selector &= ~(7<<12);
	/* set alt0 for gpio14 */
	selector |= 4<<12;
	/* clean gpio15 */
	selector &= ~(7<<15);
	/* set alt0 for gpio15 */
	selector |= 4<<15;
	writel(selector, GPFSEL1);

	writel(0, GPPUD);
	delay(150);
	writel((1<<14) | (1<<15), GPPUDCLK0);
	delay(150);
	writel(0, GPPUDCLK0);

	/* disable UART until configuration is done */
	writel(0, U_CR_REG);

	bauddiv = (UART_REFERENCE_CLOCK / baud_rate) * 1000 / 16;
	ibrd = bauddiv / 1000;

	/* baud rate divisor, integer part */
	writel(ibrd, U_IBRD_REG);

	fbrd = ((bauddiv - ibrd * 1000) * 64 + 500) / 1000;
	/* baud rate divisor, fractional part */
	writel(fbrd, U_FBRD_REG);

	/* enable FIFOs and 8 bits frames */
	writel((1<<4) | (3<<5), U_LCRH_REG);

	/* mask interupts */
	writel(0, U_IMSC_REG);
	/* enable UART, receive and transmit */
	writel(1 | (1<<8) | (1<<9), U_CR_REG);
}

void putchar(char c)
{
	if (c == '\n')
		uart_send('\r');
	uart_send(c);
}
