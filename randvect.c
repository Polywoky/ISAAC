/*
 * This program should produce the same output as the original C programs that
 * the creator of the isaac PRNG algorithm wrote, so that the output could be
 * compared against a copy stored in "randvect.txt" to ensure that the program
 * was working correctly.
 */

#include "isaac32.h"
#include <stdio.h>

int main(void)
{
	ISAAC32 state;
	
	// Initialize state
	isaac32_init(state);

	// Discard first 256 values (I think this was originally unintended)
	for (int i = 0; i < 256; i++)
	{
		isaac32(state);
	}
	
	// Display next 512 values
	for (int i = 0; i < 512; i++)
	{
		printf("%.8x",isaac32(state));
		
		// Insert newline after every eighth value (64th hex digit)
		if ((i%8)==7)
		{
			printf("\n");
		}
	}
	return 0;
}
