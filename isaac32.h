/*
 *  "isaac32.h" Implements the ISAAC PRNG using the original 32-bit algorithm
 *  Originally created by Bob Jenkins, March 1996
 *  Modified as "readable.c" by Bob Jenkins in May 2008 
 *  Refactored by Polywoky as "isaac32.h" in May 2020
 * 
 *  Original algorithm and code released by Bob Jenkins as Public Domain
 *  and can be found here: http://burtleburtle.net/bob/rand/isaacafa.html
 * 
 *  This version released by Polywoky under CC0 (Creative Commons 0) 
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

void isaac32_seed(const void* SEED, int seedSize, ISAAC32 state);
    // This function seeds ISAAC32 with 1024 bytes regardless of seed length
    // Seeds with less than 1024 bytes will be padded with zero-value bytes
    // Seeds with more than 1024 bytes will be truncated
    
void isaac32_init(ISAAC32 state);
    // Seeds ISAAC32 with zeros
    // Same as isaac_seed("", 0, state);

void isaac32_copyState(ISAAC32 duplicate, ISAAC32 original);
	// Creates a copy of the state


uint32_t isaac32(ISAAC32 state)
{
	register uint32_t temp;
	
	// For slightly more speed:
	register uint32_t state256, state259;
	state256 = state[256];
	state259 = state[259]&255; // Just a counter from 0 to 255
	
	if (!state259)
	{
		state[258] += 1; // Gets incremented once per 256 results
		state[257] = state[257] + state[258]; // Then combined with state[257] 
	}
	
	temp = state[state259];
	
	switch (state259%4)
	{
		case 0: state256 = state256^(state256<<13);	break;
		case 1: state256 = state256^(state256>>6); break;
		case 2: state256 = state256^(state256<<2); break;
		case 3: state256 = state256^(state256>>16); break;
	}

	state256 = state[(state259+128)%256] + state256;
	state[state259] = state[(temp>>2)%256] + state256 + state[257];
	state[257] = state[(state[state259]>>10)%256] + temp;
	state[256] = state256;
	state[259] = state259+1;
	return state[257];
}



void isaac32_seed(const void* SEED, int seedSize, ISAAC32 state)
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
	for (i=0; i<8; i++) mix[i]=0x9e3779b9;

	// Scramble the mixing values
	for (i=0; i<4; ++i) 
	{
		for (k=0; k<8; k++)
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

			for (k=0; k<8; k++)
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
	for (int i = 0; i < ISAAC32_STATESIZE; i++)
	{
		duplicate[i] = original[i];
	}
}



#endif
