#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

struct line {
	int initialised;
	int cur_tag;
	int *addresses;
};

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
	struct line **lines;
};

struct cache c;

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
	c.lines = malloc(sizeof(struct line *) * c.associativity);
	struct line *b = malloc(sizeof(struct line) * c.associativity * c.num_blocks);
	int i;
	for(i = 0; i < c.associativity; i++){
		c.lines[i] = &(b[i * c.num_blocks]);
	}
	// TODO: non-direct mapped cache
	for(i = 0; i < c.num_blocks; i++){
		c.lines[0][i].addresses = malloc(c.line_size); // in bytes, no need for sizeof(int)
		c.lines[0][i].initialised = 0;
		c.lines[0][i].cur_tag = 0;
	}
	printf("Num blocks: %d, num block bits: %d, num offset bits: %d\n", c.num_blocks, c.num_block_bits, c.num_offset_bits);
	return 0;
}

// We want to extract the block bits from the address
// The address is decomposed into [tag | block | offset]
int extract_block_bits(int address){
	unsigned int temp = address; // Needs to be unsigned for right shift to work
	// First clear the tag bits by shifting left
	temp = temp << (32 - c.num_block_bits - c.num_offset_bits);
	// Now clear the offset bits by shifting right
	temp = temp >> (32 - c.num_block_bits);
	return temp;
}

// We want to extract the tag bits from the address
// The address is decomposed into [tag | block | offset]
int extract_tag_bits(int address){
	unsigned int temp = address; // Needs to be unsigned for right shift to work
	temp = temp >> (c.num_block_bits + c.num_offset_bits);
	return temp;
}

// TODO: non-direct
void load_addresses_into_block(int tag, int block){
	int i;
	if(c.lines[0][block].initialised == 0){
		printf("Line 0x%08x not yet initialised - compulsory miss!\n", block);
	} else if(c.lines[0][block].cur_tag == tag){
		printf("Line 0x%08x contains tag 0x%08x - hit!\n", block, tag);
	} else {
		printf("Line 0x%08x does NOT contain tag 0x%08x - miss!\n", block, tag);
	}
	for(i = 0; i < c.line_size; i++){
		c.lines[0][block].addresses[i] = tag + i;
	}
	c.lines[0][block].initialised = 1;
}

// Only for direct
void print_block_contents(int block){
	int i;
	for(i = 0; i < c.line_size; i++){
		//printf("Accessing %d in block %d\n", i, block);
		printf("0x%08x", c.lines[0][block].addresses[i]);
		if(i != c.line_size - 1){
			printf(" | ");
		}
	}
	printf("\n");
}

// TODO: don't hardcode size
int simulate_direct_mapped_cache(){
	int i;
	for(i = 0; i < 32; i++){
		int address = addresses[i];
		int tag_bits = extract_tag_bits(address);
		int block_bits = extract_block_bits(address);
		printf("Address is: 0x%08x\n", address);
		//printf("Block bits are: 0x%08x\n", block_bits);
		//printf("Tag bits are: 0x%08x\n", tag_bits);
		//print_block_contents(block_bits);
		load_addresses_into_block(tag_bits, block_bits);
		//printf("Block currently contains address: 0x%08x\n", c.blocks[0][block_bits]);
		//c.blocks[0][block_bits] = extract_tag_bits(address);
	}
	return 0;
}

int main(){
	init_cache();
	if(c.associativity == 1){
		simulate_direct_mapped_cache();
	}
	return 0;
}
