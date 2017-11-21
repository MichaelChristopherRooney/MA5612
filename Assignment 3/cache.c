#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct cache {
	// Both of these are in bytes
	int total_size;
	int line_size;
	// 1 for direct mapped, 0 for fully associative (?)
	// Also is the number of sets the cache has.
	int associativity;
	int num_blocks;
	int num_block_bits;
	int num_offset_bits;
	int **blocks;
};

struct cache c;

// TODO: read from file
int addresses[] = {
	0x0000,
	0x0004,
	0x000c,
	0x2200,
	0x00d0,
	0x00e0,
	0x1130,
	0x0028,
	0x113c,
	0x2204,
	0x0010,
	0x0020,
	0x0004,
	0x0040,
	0x2208,
	0x0008,
	0x00a0,
	0x0004,
	0x1104,
	0x0028,
	0x000c,
	0x0084,
	0x000c,
	0x3390,
	0x00b0,
	0x1100,
	0x0028,
	0x0064,
	0x0070,
	0x00d0,
	0x0008,
	0x3394
};

// TODO:
int set_cache_settings_from_args(){
	c.total_size = 128;
	c.line_size = 16;
	c.associativity = 1;
	return 0;
}

int find_number_power_of_two(int num){
	int result = 0;
	int i;
	for(i = 0; i < 1000; i++){
		result = pow(2, i);
		if(result == num){
			return i;
		}
	}
	return -1;
}

int init_cache(){
	set_cache_settings_from_args();
	c.num_blocks = c.total_size / c.line_size;
	c.num_block_bits = find_number_power_of_two(c.num_blocks);
	c.num_offset_bits = find_number_power_of_two(c.line_size);
	c.blocks = malloc(sizeof(int *) * c.associativity);
	int *b = malloc(sizeof(int) * c.associativity * c.num_blocks);
	int i;
	for(i = 0; i < c.associativity; i++){
		c.blocks[i] = &(b[i * c.num_blocks]);
	}
	printf("Num blocks: %d, num block bits: %d, num offset bits: %d\n", c.num_blocks, c.num_block_bits, c.num_offset_bits);
	return 0;
}

// We want to extract the block bits from the address
// The address is decomposed into [tag | block | offset]
int extract_block_bits(int address){
	unsigned int temp = address; // Needs to be unsigned for right shift to work
	printf("0x%08x\n", temp);
	// First clear the tag bits by shifting left
	temp = temp << (32 - c.num_block_bits - c.num_offset_bits);
	printf("0x%08x\n", temp);
	// Now clear the offset bits by shifting right
	temp = temp >> (32 - c.num_block_bits);
	printf("0x%08x\n", temp);
	return temp;
}

int simulate_direct_mapped_cache(){
	int num = 0x5B;
	int block_bits = extract_block_bits(num);
	printf("Block bits are: %d\n", block_bits);
	printf("%d is in the block already\n", c.blocks[0][block_bits]);
	c.blocks[0][block_bits] = num;
	num = 0x5C;
	block_bits = extract_block_bits(num);
	printf("Block bits are: %d\n", block_bits);
	printf("%d is in the block already\n", c.blocks[0][block_bits]);
	c.blocks[0][block_bits] = num;
	return 0;
}

int main(){
	init_cache();
	if(c.associativity == 1){
		simulate_direct_mapped_cache();
	}
}
