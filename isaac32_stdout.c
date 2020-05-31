#include "isaac32.h"
#include <stdio.h>

unsigned long sizeOfOutput(char *arg);
int showUsage();

int main(int argc, char *argv[])
{
	unsigned long size;
	const unsigned long max = (unsigned long) -1;
	uint32_t out;
	
	// Allocate memory for state of PRNG
	ISAAC32 state;
	
	// Wrong number of arguments.
	if (argc < 2 || argc > 3)
		return showUsage();


	// Initialize state
	if (argc == 3)
	{
		// Initialize state with seed text in argv[2]
		int seed_len;
		for (seed_len=0; argv[2][seed_len] != 0; seed_len++);
		isaac32_seed(argv[2], seed_len, state);
	} else {
		// No seed text provided, initialize without seed
		isaac32_init(state);
	}

	
	// Determine how many bytes of output are required
	size = sizeOfOutput(argv[1]);


	// Generate output
	while (size)
	{
		out = isaac32(state);
		if (size > 4)
		{
			// At least four more bytes out needed, so output a 32-bit value
			fwrite(&out, 4, 1, stdout);				
			if (size != max) size -= 4;
		} else {
			// Less than four more bytes needed, so output partial 32-bit value
			fwrite(&out, 1, size, stdout);
			size = 0;
		}
	}
		
	return 0;
}



int showUsage()
{
	// Usage info varies slightly if using Windows
	char run[3] = "./";
	#ifdef _WIN32
		run[0] = '\0';
	#endif

	// Printing to stderr in case stdout is being piped or redirected elsewhere
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, "       Send to stdout: %sisaac32_stdout <bytes>\n", run);
	fprintf(stderr, "       Seeded output:  %sisaac32_stdout <bytes> <seed text>\n", run);
	fprintf(stderr, "       Save to file:   %sisaac32_stdout <bytes> > <filename> \n", run);
	fprintf(stderr, "       Seeded to file: %sisaac32_stdout <bytes> <seed text> > <filename> \n", run);
	fprintf(stderr, "\n");
	fprintf(stderr, "Value of <bytes> can be modified with the following:\n");
	fprintf(stderr, "        KB  KiB  MB  MiB  GB  GiB  TB  TiB\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "<bytes> can be replaced with 'max' for unlimited output.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Examples: \n");
	fprintf(stderr, "          %sisaac32_stdout 23MiB > 23MiB.bin\n", run);
	fprintf(stderr, "          %sisaac32_stdout 1GB \"swordfish\" > 1GB_seeded.bin\n", run);
	fprintf(stderr, "          %sisaac32_stdout max | %sRNG_test stdin32\n", run, run);
	fprintf(stderr, "\n");
	
	return 1;
}



unsigned long sizeOfOutput(char *arg)
{
	int i;
	int kilo = 1000;
	unsigned long size = 0;
	
	if (arg[0] == 'm' && arg[1] == 'a' && arg[2] == 'x' && arg[3] == '\0')
	{
		return (unsigned long) -1;
	}
	
	if (arg[0] < '0' || arg[0] > '9')
	{
		fprintf(stderr, "Invalid output size.\n");
		return 0;
	}
	
	for (i=0; arg[i] >= '0' && arg[i] <= '9'; i += 1)
	{
		size *= 10;
		size += arg[i] - '0';
	}
	
	if (arg[i] != '\0')
	{		
		if (arg[i+1] == 'i')
		{
			kilo = 1024;
		}

		switch(arg[i])
		{	
			case 't':
			case 'T':
				size *= kilo;
				__attribute__ ((fallthrough));
	
			case 'g':
			case 'G':
				size *= kilo;
				__attribute__ ((fallthrough));
	
			case 'm':
			case 'M':
				size *= kilo;
				__attribute__ ((fallthrough));
	
			case 'k':
			case 'K':
				size *= kilo;
				break;
			
			default:
				fprintf(stderr, "Invalid output size.\n");
				return 0;
		}
		
		i += 1;
	}

	if (arg[i] == 'i' && arg[i+1] != '\0')
	{
		i += 1;
	}
	
	if (arg[i] == 'b' || arg[i] == 'B')
	{
		i += 1;
	}
	
	if (arg[i] != '\0')
	{
		fprintf(stderr, "Invalid output size.\n");
		return 0;
	}
	
	return size;
}
