#pragma once

void compression_init();
// Return val: input bytes processed
int compression_compress(FILE* input, FILE* output);
// Return val: input bytes processed
int compression_uncompress(FILE* input, FILE* output);
void compression_finalize();
