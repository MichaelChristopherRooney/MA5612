#include <stdio.h>
#include <math.h>

struct cache {
	// Both of these are in bytes
	int total_size;
	int line_size;
	// 1 for direct mapped, 0 for fully associative (?)
	int associativity;
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

int simulate_direct_mapped_cache(){
	int num_blocks = c.total_size / c.line_size;
	int num_block_bits = find_number_power_of_two(num_blocks);
	int num_offset_bits = find_number_power_of_two(c.line_size);
	printf("%d, %d, %d\n", num_blocks, num_block_bits, num_offset_bits);
	return 0;
}

int main(){
	set_cache_settings_from_args();
	if(c.associativity == 1){
		simulate_direct_mapped_cache();
	}
}
