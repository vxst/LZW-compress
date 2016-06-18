#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compression.h"

static const size_t IO_BUF_SIZE = 256 * 1024;

int main(int argc, char** argv){
	char *program_name = argv[0];
	char *input_file_name, *output_file_name;
	FILE *input_file, *output_file;
	char *input_buffer, *output_buffer;
	int i;

	assert(sizeof(char) == 1);

	for(i = strlen(program_name)-1; i >= 0; i--){
		if(program_name[i] == '/'){
			program_name += i+1;
			break;
		}
	}

	if(strcmp(program_name, "compress") != 0 && strcmp(program_name, "uncompress") != 0)
		return EXIT_FAILURE;

	if(argc != 3){
		printf("usage: %s [input file] [output file]\n", argv[0]);
		return EXIT_FAILURE;
	}

	input_file_name = argv[1];
	output_file_name = argv[2];

	input_file = fopen(input_file_name, "rb");
	output_file = fopen(output_file_name, "wb");
	input_buffer = (char*) malloc(sizeof(char) * IO_BUF_SIZE);
	output_buffer = (char*) malloc(sizeof(char) * IO_BUF_SIZE);

	setvbuf(input_file, input_buffer, _IOFBF, IO_BUF_SIZE);
	setvbuf(output_file, output_buffer, _IOFBF, IO_BUF_SIZE);

	compression_init();

	if(strcmp(program_name, "compress") == 0)
		compression_compress(input_file, output_file);
	if(strcmp(program_name, "uncompress") == 0)
		compression_uncompress(input_file, output_file);

	compression_finalize();

	free(input_buffer);
	free(output_buffer);
	fclose(input_file);
	fclose(output_file);

	return 0;
}
