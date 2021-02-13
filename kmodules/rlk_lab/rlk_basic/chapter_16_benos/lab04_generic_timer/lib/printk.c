#include <stdarg.h>
#include <asm/uart.h>
#include <type.h>
#include <string.h>

#define CONSOLE_PRINT_BUFFER_SIZE 1024
static char print_buf[CONSOLE_PRINT_BUFFER_SIZE];

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define do_div(n, base) ({					\
	unsigned int __base = (base);				\
	unsigned int  __rem;						\
	__rem = ((u64)(n)) % __base;			\
	(n) = ((u64)(n)) / __base;				\
	__rem;							\
})

static const char *scan_number(const char *string, int *number)
{
	int tmp = 0;

	while (is_digit(*string)) {
		tmp *= 10;
		tmp += *(string++) - '0';
	}

	*number = tmp;
	return string;
}

static char *number(char *str, u64 num, int base, int size, int precision
	, int type)
{
	char c, sign = 0, tmp[128];
	s64 snum;

	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type & SMALL)
		digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ';

	if (type & SIGN) {
		snum = (s64)num;
		if (snum < 0) {
			sign = '-';
			num = -snum;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}

	if (type & SPECIAL)
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;

	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0)
			tmp[i++] = digits[do_div(num, base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while (size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;

	if (type & SPECIAL)
		if (base == 8)
			*str++ = '0';
		else if (base == 16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';

	return str;
}

/*
 * printf function
 *
 * 1. flags:
 *  - : 左对齐
 *  + : 加号或减号,
 *  # : specifier 是 o、x、X 时，增加前缀0x
 *  0 : 使用零 填充字段宽度
 *
 * 2. 最小宽度width
 * 3. 类型长度
 *  h : short, short int
 *  l : long, unsigned long, long int
 *  ll: long long, unsigned long long
 */
static int myprintf(char *string, unsigned int size,
		const char *fmt, va_list arg)
{
	char *pos;
	int flags;
	int field_width; /* width of output field */
	int precision;
	int qualifier;
	char *ip;
	char *s;
	int i;
	int len;
	u64 num;
	int base;

	pos = string;

	for (; *fmt; fmt++) {
		if (*fmt != '%') {
			*pos++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
repeat:
		++fmt; /* skip first % */
		switch (*fmt) {
		case '-':
			flags |= LEFT;
			goto repeat;
		case '+':
			flags |= PLUS;
			goto repeat;
		case ' ':
			flags |= SPACE;
			goto repeat;
		case '#':
			flags |= SPECIAL;
			goto repeat;
		case '0':
			flags |= ZEROPAD;
			goto repeat;
		}

		/* 最小宽度（width）用于控制显示字段的宽度 */
		field_width = -1;
		if (is_digit(*fmt)) {
			fmt = scan_number(fmt, &field_width);
		} else if (*fmt == '*') {
			field_width = va_arg(arg, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* 精度（.precision）用于指定输出精度 */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				fmt = scan_number(fmt, &precision);
			else if (*fmt == '*')
				precision = va_arg(arg, int);
			if (precision < 0)
				precision = 0;
		}

		/*
		 * 类型长度用于控制待输出数据的
		 * 数据类型长度
		 */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;

			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		switch (*fmt) {
		/* 输出字符型*/
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*pos++ = ' ';
			*pos++ = (unsigned char)va_arg(arg, int);
			while (--field_width > 0)
				*pos++ = ' ';
			/* */
			continue;

		/* 输出字符串*/
		case 's':
			s = va_arg(arg, char *);
			if (!s)
				s = "<NULL>";
			len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--)
					*pos++ = ' ';
			for (i = 0; i < len; i++)
				*pos++ = *s++;
			while (len < field_width--)
				*pos++ = ' ';
			continue;

		/*
		 * 到此字符之前为止，一共输出的字符个数，
		 * 不输出文本
		 */
		case 'n':
			ip = (char *)va_arg(arg, int *);
			*ip = (pos - string);
			continue;

		/* 处理数字*/

		/* 输出类型输出指针*/
		case 'p':
			if (field_width == -1) {
				field_width = 2 * sizeof(void *);
				flags |= ZEROPAD;
			}
			pos = number(pos,
				(unsigned long)va_arg(arg, void *),
				16, field_width, precision, flags);
			continue;

		/* 以八进制表示的整数*/
		case 'o':
			base = 8;
			break;
		/* 以十六进制表示的整数*/
		case 'x':
			flags |= SMALL;
		case 'X':
			base = 16;
			break;
		/* 有符号的十进制有符号整数*/
		case 'd':
		case 'i':
			flags |= SIGN;
		/* 无符号十进制整数*/
		case 'u':
			base = 10;
			break;

		default:
			if (*fmt != '%')
				*pos++ = '%';
			if (*fmt)
				*pos++ = *fmt;
			else
				--fmt;
			break;
		}

		if (qualifier == 'L') {
			if (flags & SIGN)
				num = va_arg(arg, s64);
			else
				num = va_arg(arg, u64);
		} else if (qualifier == 'h') {
			if (flags & SIGN)
				num = (short)va_arg(arg, int);
			else
				num = (unsigned short)va_arg(arg, unsigned int);
		} else if (qualifier == 'l') {
			if (flags & SIGN)
				num = va_arg(arg, long);
			else
				num = va_arg(arg, unsigned long);
		} else {
			if (flags & SIGN)
				num = (int)va_arg(arg, int);
			else
				num = va_arg(arg, unsigned int);
		}
		pos = number(pos, num, base, field_width, precision, flags);
	}

	*pos = '\0';
	return pos - string;
}

int printk(const char *fmt, ...)
{
	va_list arg;
	int len;
	int i;

	va_start(arg, fmt);
	len = myprintf(print_buf, sizeof(print_buf), fmt, arg);
	va_end(arg);
	for (i = 0; i < len; i++) {
		putchar(print_buf[i]);
		if (i > sizeof(print_buf))
			break;
	}
	return len;
}
