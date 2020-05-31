/*
 *  ISAAC32 Version 1.0.0
 * 
 *  An implementation of the 32-bit Isaac PRNG
 * 
 *  Original algorithm and code created and released by Bob Jenkins as
 *  Public Domain: http://burtleburtle.net/bob/rand/isaacafa.html
 * 
 *  This version released by Polywoky under CC0 (Creative Commons 0) 
 *  Latest version available here: https://github.com/Polywoky/ISAAC
 */

#ifndef ISAAC32_H
#define ISAAC32_H
#include <stdint.h>

#define ISAAC32_STATESIZE 260


typedef uint32_t ISAAC32[ISAAC32_STATESIZE];
	// Creates new state for use with isaac32, similar to creating a filehandle
	// Creating multiple states allows multiple simultaneous instances
	// MUST BE SEEDED BEFORE USE!

uint32_t isaac32(ISAAC32 state);
	// Returns one unsigned 32-bit value from isaac

void isaac32_read(void *DATA, int dataSize, ISAAC32 state);
	// Produces dataSize number of bytes and stores them in DATA
	// If dataSize is not a multiple of four, a multiple of four bytes will
	// be generated and the unused bytes will be discarded.

void isaac32_seed(const void *SEED, int seedSize, ISAAC32 state);
    // Seeds ISAAC32 with 1024 bytes
    // Less than 1024 bytes will be padded with zero-value bytes
    // More than 1024 bytes will be truncated
    
void isaac32_init(ISAAC32 state);
    // Seeds ISAAC32 with zeros
    // Same as isaac_seed("", 0, state);

void isaac32_copyState(ISAAC32 duplicate, ISAAC32 original);
	// Creates a copy of the state



uint32_t isaac32(ISAAC32 state)
{
	uint32_t singleValue[1];
	isaac32_read(singleValue, 4, state);
	return singleValue[0];
}



void isaac32_read(void *DATA, int dataSize, ISAAC32 state)
{
	int loopCheck, loopsNeeded, singleBytes;
	uint32_t *data32bit =  (uint32_t *) DATA;
	unsigned char *data8bit = (unsigned char *) DATA;
	
	// How many loops do we need?
	loopCheck = dataSize/4;
	singleBytes = dataSize-(loopCheck*4);
	loopsNeeded = loopCheck;
	if (singleBytes > 0)
		loopsNeeded++;

	
	// Using register for slightly more speed:
	register uint32_t temp;
	register uint32_t state256, state257, state258, state259;
	state256 = state[256];
	state257 = state[257];
	state258 = state[258];
	state259 = state[259]&255; // Just a counter from 0 to 255

	for (int loop = 0; loop < loopsNeeded; loop++)
	{
		if (!state259)
		{
			state258 += 1; // Gets incremented once per 256 results
			state257 = state257 + state258; // Then combined with state[257] 
		}
	
		temp = state[state259];
	
		switch (state259%4)
		{
			case 0: state256 = state256^(state256<<13); break;
			case 1: state256 = state256^(state256>> 6); break;
			case 2: state256 = state256^(state256<< 2); break;
			case 3: state256 = state256^(state256>>16); break;
		}

		state256 = state[(state259+128)%256] + state256;
		state[state259] = state[(temp>>2)%256] + state256 + state257;
		state257 = state[(state[state259]>>10)%256] + temp;
		state259 = (state259+1)&255;
		
		if (loop == loopCheck)
		{
			// Only a partial value goes here!
			uint32_t value = state257;
			unsigned char *bytes = (unsigned char *) &value;
			for (int i = 0; i < singleBytes; i++)
			{
				data8bit[(4*loop)+i] = bytes[i];
			}
		} else {
			data32bit[loop] = state257;
		}
	}

	state[256] = state256;
	state[257] = state257;
	state[258] = state258;
	state[259] = state259;

	return;
}



void isaac32_seed(const void *SEED, int seedSize, ISAAC32 state)
{
	int i, j, k;
	uint32_t *seed;
	uint32_t seedCopy[256];
	uint32_t mix[8];
	const int shift[8] = { 11, 2, 8, 16, 10, 4, 8, 9 };
	
	if (seedSize >= 1024)
	{
		// SEED is long enough, so use it directly.
		seed = (uint32_t*) SEED;
	} else {

		// SEED is shorter than needed, so copy into larger memory location
		unsigned char *source = (unsigned char*) SEED; // Copy from here
		unsigned char *dest = (unsigned char*) seedCopy; // Copy to here
		for (i=0; i < seedSize; ++i) dest[i] = source[i];
		
		// Fill the rest of that space with zeros
		for (i=seedSize; i < 1024; ++i) dest[i] = 0;

		// Use this larger zero-padded copy as the seed
		seed = (uint32_t*) seedCopy;
	}
	
	// Clear the supplemental state values
	for (i=256; i<260; ++i) state[i] = 0;
	
	// Fill the mixing values with golden ratio
	for (i=0; i<8; ++i) mix[i]=0x9e3779b9;

	// Scramble the mixing values
	for (i=0; i<4; ++i) 
	{
		for (k=0; k<8; ++k)
		{
			if (k%2) mix[k] ^= mix[(k+1)%8] >> shift[k];
			else     mix[k] ^= mix[(k+1)%8] << shift[k];
			mix[(k+3)%8] += mix[k];
			mix[(k+1)%8] += mix[(k+2)%8];
		}
	}

	// Fill in state[] with messy stuff
	for (i=0; i<2; ++i)
	{
		for (j=0; j<256; j+=8)
		{
			for (k=0; k<8; ++k) mix[k]+=seed[j+k];

			for (k=0; k<8; ++k)
			{
				if (k%2) mix[k] ^= mix[(k+1)%8] >> shift[k];
				else     mix[k] ^= mix[(k+1)%8] << shift[k];
				mix[(k+3)%8] += mix[k];
				mix[(k+1)%8] += mix[(k+2)%8];
			}

			for (k=0; k<8; ++k) state[j+k]=mix[k];
		}
		seed=state; // Do a second pass using new state as seed
	}
}



void isaac32_init(ISAAC32 state)
{
	isaac32_seed("", 0, state);
}



void isaac32_copyState(ISAAC32 duplicate, ISAAC32 original)
{
	for (int i = 0; i < ISAAC32_STATESIZE; ++i)
	{
		duplicate[i] = original[i];
	}
}



#endif
