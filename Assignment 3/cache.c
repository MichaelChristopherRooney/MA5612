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
	int last_used;
	unsigned char *addresses;
};

struct cache {
	// Both of these are in bytes
	int total_size;
	int line_size;
	// 1 for direct mapped, 0 for fully associative (?)
	// Also is the number of sets the cache has.
	int associativity;
	// The number of blocks in the cache.
	// For direct mapped caches it is equal to total_size / line_size
	// For fully associative caches it is equal to 1
	// For other caches it is equal to 
	int num_blocks;
	int num_lines;
	// Addresses are decomposed into [tag | block | offset].
	// These are the number of bits that should be used for the block and offset.
	int num_block_bits;
	int num_offset_bits;
	// 2D array of the lines
	// If direct mapped then the first dimension is the same as num_blocks and the second dimension is 1.
	// If fully associative then the first dimension is 1 and the second dimension is the same as num_blocks.
	struct line **lines;
};

struct cache c;

int NUM_HITS = 0;
int NUM_MISSES = 0;

// TODO:
int set_cache_settings_from_args() {
	c.total_size = 128;
	c.line_size = 16;
	c.associativity = 8;
	return 0;
}

int find_number_power_of_two(int num) {
	int result = 0;
	int i;
	for (i = 0; i < 1000; i++) {
		result = pow(2, i);
		if (result == num) {
			return i;
		}
	}
	return -1;
}

void print_line_contents(int block, int set) {
	int i;
	for (i = 0; i < c.line_size; i++) {
		unsigned char val = c.lines[block][set].addresses[i];
		printf("0x%02x", val);
		if (i != c.line_size - 1) {
			printf(" | ");
		}
	}
	printf("\n");
}


int init_cache() {
	set_cache_settings_from_args();
	// TODO: fully associative represented with 0
	// TODO: increase this to 1 if it falls below that limit.
	c.num_lines = c.total_size / c.line_size;
	c.num_blocks = c.num_lines / c.associativity;
	c.num_block_bits = find_number_power_of_two(c.num_blocks);
	c.num_offset_bits = find_number_power_of_two(c.line_size);
	c.lines = calloc(1, sizeof(struct line *) * c.num_blocks);
	struct line *temp = calloc(1, sizeof(struct line) * c.num_lines);
	int i;
	for (i = 0; i < c.num_blocks; i++) {
		c.lines[i] = &(temp[i * c.associativity]);
	}
	for (i = 0; i < c.num_blocks; i++) {
		int n;
		for (n = 0; n < c.associativity; n++) {
			c.lines[i][n].addresses = calloc(1, c.line_size); // in bytes, no need for sizeof(int)
			c.lines[i][n].initialised = 0;
			c.lines[i][n].cur_tag = 0;
		}
	}
	printf("Num blocks: %d, num lines %d, num block bits: %d, num offset bits: %d\n", c.num_blocks, c.num_lines, c.num_block_bits, c.num_offset_bits);
	return 0;
}

// We want to extract the block bits from the address
// The address is decomposed into [tag | block | offset]
int extract_block_bits(int address) {
	unsigned int temp = address; // Needs to be unsigned for right shift to work
	// First clear the tag bits by shifting left
	temp = temp << (32 - c.num_block_bits - c.num_offset_bits);
	// Now clear the offset bits by shifting right
	temp = temp >> (32 - c.num_block_bits);
	return temp;
}

// We want to extract the tag bits from the address
// The address is decomposed into [tag | block | offset]
int extract_tag_bits(int address) {
	unsigned int temp = address; // Needs to be unsigned for right shift to work
	temp = temp >> (c.num_block_bits + c.num_offset_bits);
	return temp;
}

int block_contains_address(int tag, int block) {	
	int i;
	for (i = 0; i < c.associativity; i++) {
		if (c.lines[block][i].initialised == 0) {
			continue;
		}
		if (c.lines[block][i].cur_tag == tag) {
			printf("Line 0x%08x contains tag 0x%08x - hit!\n", block, tag);
			NUM_HITS++;
			return 1;
		}
	}
	printf("Block 0x%08x does NOT contain tag 0x%08x - miss!\n", block, tag);
	NUM_MISSES++;
	return 0;
}

void load_addresses_into_block_direct_mapped(int tag, int block) {
	if (c.lines[block][0].initialised == 1) {
		printf("Evicting tag 0x%08x from block 0x%08x\n", tag, block);
	}
	int i;
	for (i = 0; i < c.line_size; i++) {
		c.lines[block][0].addresses[i] = tag + i;
	}
	c.lines[block][0].initialised = 1;
	c.lines[block][0].cur_tag = tag;
}

void load_addresses_into_set_n_way(int tag, struct line *l, int count) {
	if (l->initialised == 1) {
		printf("Evicting tag 0x%08x from line\n", tag);
	}
	int i;
	for (i = 0; i < c.line_size; i++) {
		l->addresses[i] = tag + i;
	}
	l->initialised = 1;
	l->cur_tag = tag;
	l->last_used = i + 1; // avoid zero case
}

struct line * find_least_recently_used_line(struct line *block) {
	int i;
	struct line *cur = &block[0];
	if (cur->initialised == 0) {
		return cur;
	}
	for (i = 1; i < c.associativity; i++) {
		if (block[i].initialised == 0) {
			return &block[i];
		}
		if (block[i].last_used < cur->last_used) {
			cur = &block[i];
		}
	}
	return cur;
}

// TODO: fully associative?
void load_addresses_into_block(int tag, int block, int count) {
	if (c.associativity == 1) {
		load_addresses_into_block_direct_mapped(tag, block);
	} else {
		struct line *line_block = c.lines[block];
		struct line *l = find_least_recently_used_line(line_block);
		load_addresses_into_set_n_way(tag, l, count);
	}

}

// TODO: don't hardcode size
void simulate_cache() {
	int i;
	for (i = 0; i < 32; i++) {
		int address = addresses[i];
		int tag_bits = extract_tag_bits(address);
		int block_bits = extract_block_bits(address);
		printf("Address is: 0x%08x\n", address);
		//printf("Block bits are: 0x%08x\n", block_bits);
		//printf("Tag bits are: 0x%08x\n", tag_bits);
		if (block_contains_address(tag_bits, block_bits) == 0) {
			load_addresses_into_block(tag_bits, block_bits, i);
		}
		printf("\n");
		//print_line_contents(i, n);
	}
}

int main() {
	init_cache();
	simulate_cache();
	printf("Hits: %d, Misses: %d\n", NUM_HITS, NUM_MISSES);
	return 0;
}

