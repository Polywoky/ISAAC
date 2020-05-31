#include "isaac32.h"
#include <stdio.h>

const int bufferValues = 1024;
const int bufferSize = bufferValues * 4;

int showUsage();

int main(int argc, char *argv[])
{
//	FILE *file_in, *file_out;
	
	// Allocate memory for state of PRNG
	ISAAC32 state;
	
	// Wrong number of arguments.
	if (argc != 4)
		return showUsage();
	
	// Check that source and destination are different files
	for (int i=0; argv[1][i] == argv[2][i]; i++)
	{
		if (argv[1][i] == '\0')
		{
			printf("Error: Source file is same as destination\n");
			return 1;
		}
	}
	
	// Initialize state with seed text in argv[3]
	int seedLen;
	for (seedLen=0; argv[3][seedLen] != 0; seedLen++);
	isaac32_seed(argv[3], seedLen, state);

	// Open source file
	FILE *file_in = fopen(argv[1], "rb");
	if (file_in == NULL)
	{
		printf("Error reading file: %s\n", argv[1]);
		return 1;
	}

	// Open destination file
	FILE *file_out = fopen(argv[2], "wb");
	if (file_out == NULL)
	{
		printf("Error reading file: %s\n", argv[2]);
		return 1;
	}
	
	// Create variables needed for ciphering
	unsigned char buffer[bufferSize];
	uint32_t *buffer32 = (uint32_t *)buffer;
	unsigned int dataSize;
	
	// Cipher from file_in to file_out
	dataSize = fread(buffer, 1, bufferSize, file_in);
	while (dataSize > 0)
	{
		for (int i = 0; i < bufferValues; i++)
			buffer32[i] ^= isaac32(state);
		
		if (fwrite(buffer, 1, dataSize, file_out) != dataSize)
			printf("Error while writing to %s\n", argv[2]);
			
		dataSize = fread(buffer, 1, bufferSize, file_in);
	}
	
	fclose(file_in);
	fclose(file_out);
	
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
	fprintf(stderr, "\nUNFINISHED CODE!!!\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, "       %sisaac32_cipher <source file> <coded file> <password>\n", run);
	fprintf(stderr, "       %sisaac32_cipher <coded file> <decoded file> <password>\n", run);
	fprintf(stderr, "\n");
	fprintf(stderr, "Examples: \n");
	fprintf(stderr, "         Encode: %sisaac32_cipher myFile.txt myFile.isaac32 \"swordfish\"\n", run);
	fprintf(stderr, "         Decode: %sisaac32_cipher myFile.isaac32 mySecrets.txt \"swordfish\"\n", run);
	fprintf(stderr, "\n");
	return 1;
}
