#include <unordered_map>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>
#include "compression.h"

using namespace std;
static unordered_map<string, int> forward_map;
static unordered_map<int, string> backward_map;
static int current_bit;
static int token_count;
static int max_bit = 12;

void compression_init(){
	string s = " ";
	forward_map.clear();
	backward_map.clear();
	for(int i = 0; i < 256; i++){
		s[0] = i;
		forward_map[s] = i;
		backward_map[i] = s;
	}
	current_bit = 8;
	token_count = 256;
}

static int count_bit(int a){
	int bit = 0;
	while(a){
		a>>=1;
		bit++;
	}
	return bit;
}

static vector<bool> output_bit_buffer;

static void output_flush(FILE* output){
	char t;
	for(int i = 0; i < output_bit_buffer.size(); i++){
		if((i&7) == 0)
			t = 0;
		t |= output_bit_buffer[i] << (7-(i&7));
		if(((i+1)&7) == 0)
			fputc(t, output);
	}
	if((output_bit_buffer.size() & 7) != 0)
		fputc(t, output);
	output_bit_buffer.clear();
}

static void output_put(FILE* output, int len, int n){
	for(int i = len-1; i >= 0; i--)
		output_bit_buffer.push_back((n>>i)&1);
	if(output_bit_buffer.size() > 8192){
		char* buf = (char*)malloc(sizeof(char) * 1024);
		for(int i = 0; i < 1024; i++)
			buf[i] = 0;
		for(int i = 0; i < 8192; i++)
			buf[i>>3] |= output_bit_buffer[i] << (7-(i&7));
		output_bit_buffer.erase(output_bit_buffer.begin(), output_bit_buffer.begin() + 8192);
		fwrite(buf, sizeof(char), 1024, output);
	}
}

int compression_compress(FILE* input, FILE* output){
	if(feof(input))
		return EXIT_SUCCESS;

	string token = "", next_token;

	while(!feof(input)){
		next_token = token;
		next_token.resize(next_token.size() + 1);
		next_token[token.size()] = fgetc(input);
		if(forward_map.find(next_token) == forward_map.end()){
			output_put(output, current_bit, forward_map.at(token));
			current_bit = count_bit(token_count);
			if(current_bit > max_bit){
				current_bit = max_bit;
			}else{
				forward_map[next_token] = current_bit++;
			}
			token = " ";
			token[0] = next_token[next_token.size()-1];
		}else{
			token = next_token;
		}
	}
	assert(token.size() > 0);
	output_put(output, current_bit, forward_map.at(token));
	output_flush(output);
	return EXIT_SUCCESS;
}

static vector<bool> input_get_cache;
static int input_get(FILE* input, int count_bit){
	int ret_val = 0;
	while(input_get_cache.size() < count_bit){
		char t = fgetc(input);
		for(int i = 7; i >= 0; i--)
			input_get_cache.push_back((t>>i)&1);
	}
	for(int i = 0; i < count_bit; i++)
		ret_val |= input_get_cache[i] << (count_bit-i-1);
	input_get_cache.erase(input_get_cache.begin(), input_get_cache.begin()+count_bit);
	return ret_val;
}
static void output_string(FILE* output, string s){
	fwrite(s.data(), sizeof(char), s.size(), output);
}

int compression_uncompress(FILE* input, FILE* output){
	if(feof(input))
		return EXIT_SUCCESS;
	string previous_token, current_token, combined_token;

	assert(previous_token.size() == 0);

	while(!feof(input)){
		int k = input_get(input, current_bit);
		current_token = backward_map.at(k);
		output_string(output, current_token);
		if(previous_token.size() != 0){
			combined_token = previous_token + current_token.substr(0, 1);
			if(forward_map.find(combined_token) == forward_map.end()){
				if(count_bit(token_count) <= max_bit){
					forward_map[combined_token] = token_count;
					backward_map[token_count] = combined_token;
					current_bit = count_bit(token_count);
					token_count++;
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
void compression_finalize(){
	forward_map.clear();
	backward_map.clear();
}
