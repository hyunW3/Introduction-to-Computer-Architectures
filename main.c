/*
 * SWE3005-43 Introduction to Computer Architectures (Fall 2019)
 *
 * Instructor: Prof. Jinkyu Jeong (jinkyu@skku.edu)
 *
 * TA: Minwoo Ahn (minwoo.ahn@csi.skku.edu), Sunghwan Kim (sunghwan.kim@csi.skku.edu)
 * Office: Semiconductor Buildnig #400509
 *
 * Author: Minwoo Ahn
 * Description: CPU Cache Simulator
 *
 * Copyright (c) 2019 Sungkyunkwan Univ. CSI (Computer Systems and Intelligence Lab)
 */

#include "mem.h"
/* Global Variables */
int cycle = 0;
int hit_count[2] = {0};
int miss_count[2] = {0};

/* Variables for pa3_check */
int cache_level;
int memory_access;

int main() {
	char config_file[15];
	char input_file[20];
	char output_file[50];
	char policy[4];
	FILE *fd_config;
	FILE *fd_input;
	FILE *fd_output;
	char op;		//Read or Write of request
	unsigned int phy_addr;
	double a = log2(256);
	/* Get Input */
	strcpy(config_file, "cache.config");
	strcpy(input_file, "cache.input");
	strcpy(output_file, "cache.output");

	fd_config = fopen(config_file, "r");
	if(fd_config == NULL) {
		printf("There is no file such %s\n", config_file);
		exit(1);
	}

	fscanf(fd_config, "%d", &cache_level);
	fscanf(fd_config, "%d", &memory_access);

	fscanf(fd_config, "%d", &cache[0].size);
	fscanf(fd_config, "%d", &cache[0].num_entry);
	fscanf(fd_config, "%d", &cache[0].num_ways);

	fscanf(fd_config, "%s", policy);
	if (!strcmp(policy, "LRU"))
		cache[0].replace = LRU;
	else
		cache[0].replace = FIFO;

	fscanf(fd_config, "%s", policy);
	if (!strcmp(policy, "WB"))
		cache[0].write = WB;
	else
		cache[0].write = WT;

	fscanf(fd_config, "%d", &cache[0].access_cycle);

	cache[0].bytes_per_block = 4;
	cache[0].block_per_entry = cache[0].size / cache[0].num_ways / cache[0].num_entry / cache[0].bytes_per_block;

	if(cache_level == 2) {
		fscanf(fd_config, "%d", &cache[1].size);
		fscanf(fd_config, "%d", &cache[1].num_entry);
		fscanf(fd_config, "%d", &cache[1].num_ways);

		fscanf(fd_config, "%s", policy);
		if (!strcmp(policy, "LRU"))
			cache[1].replace = LRU;
		else
			cache[1].replace = FIFO;

		fscanf(fd_config, "%s", policy);
		if (!strcmp(policy, "WB"))
			cache[1].write = WB;
		else
			cache[1].write = WT;

		fscanf(fd_config, "%d", &cache[1].access_cycle);

		cache[1].bytes_per_block = 4;
		cache[1].block_per_entry = cache[1].size / cache[1].num_ways / cache[1].num_entry / cache[1].bytes_per_block;
	}

	initialize();

	/* Open Input & Output Files */
	fd_input = fopen(input_file, "r");
	if (fd_input == NULL) {
		printf("There is no file such %s\n", input_file);
		exit(1);
	}

	fd_output = fopen(output_file, "w+");
	/* Start Simulation */
	while(1) {
		fscanf(fd_input, " %c", &op);
		
		switch(op) {
			case 'R':	//Read Operation
				fscanf(fd_input, "%x", &phy_addr);
				printf("read_op:%x",phy_addr);
				read_op(phy_addr);
				break;
			case 'W':	//Write Operation
				fscanf(fd_input, "%x", &phy_addr);
				write_op(phy_addr);
				break;
			default:	//HALT
				goto end;
		};
	}

end:
	fclose(fd_config);
	fclose(fd_input);
	/*	Print Results	  */
	print_result(fd_output);
	fclose(fd_output);

	destroy();

	return 0;
}

void initialize() {
	int n, i, j;
	
	printf("Initialize Your Cache...\n");

	/* cache[level #].way_cache[way #][entry #]*/
	for(n = 0; n < cache_level; n++) {
		cache[n].way_cache = (struct cache_entry **)malloc(sizeof(struct cache_entry *) * cache[n].num_ways);
		
		for (i = 0; i < cache[n].num_ways; i++)
			cache[n].way_cache[i] = (struct cache_entry *)malloc(sizeof(struct cache_entry) * cache[n].num_entry);
		
		for (i = 0; i < cache[n].num_ways; i++) {
			for (j = 0; j < cache[n].num_entry; j++) {				
				/* Initialize other variables in your cache_entry structure! */
				/* e.g. valid, dirty, tag, etc.                              */
				cache[n].way_cache[i][j].valid = 0;
				cache[n].way_cache[i][j].tag = 0;
				cache[n].way_cache[i][j].dirty = 0;
				cache[n].way_cache[i][j].LRU = -1;
				///////////////////////////////////////////////////////////////
			}
		}
	}

	printf("Finished to Initialize Your Cache!\n\n");
}

void destroy() {
	int n, i;

	printf("Destroy Your Cache...\n");
	for (n = 0; n < cache_level; n++) {
		for (i = 0; i < cache[n].num_ways; i++) {
			free(cache[n].way_cache[i]);
		}
		free(cache[n].way_cache);
	}
	printf("Finished to Destroy Your Cache!\n");
}


void read_op(unsigned int addr) {
	/* You have to implement your own read_op function here! */
	printf("-----------read process\n");
	for(int n = 0; n < cache_level; n++) {
		for (int i = 0; i < cache[n].num_ways; i++) { // 2 -> check all way of certain entry
			unsigned int addr1 = addr >> 2; // without byte offset
			unsigned int num_block = (int)log2(cache[n].block_per_entry); /// 2;
			unsigned int block_offset = (addr1 & (1<<num_block-1));
			addr1 = (addr1>>num_block);
			unsigned int entry_bit = (int)(log2(cache[n].num_entry));
			//printf("%d\n",entry_bit);
			unsigned int index = addr1  & (1 << entry_bit-1);   
			printf("%d %d\n",index,num_block); 
			unsigned int tag = (addr1>>entry_bit);
			printf("in read addr:%x\n",addr);
			printf("%u %u %u %u\n",32-(entry_bit+num_block+2),entry_bit,num_block,2);
			printf("%x %x %x %x\n",tag, index, block_offset, (addr & (1<<1)));
			//check directly entries
	// cache[level #].way_cache[way #][entry #]

		}
	}
	///////////////////////////////////////////////////////////
}

void write_op(unsigned int addr) {
	/* You have to implement your own write_op function here! */
	printf("-----------write process\n");
	int used_cycle=0;
	for(int n = 0; n < cache_level; n++) {
		int not_full=0;		
		for (int i = 0; i < cache[n].num_ways; i++) { // 2 -> check all way of certain entry
			unsigned int addr1 = addr >> 2; // without byte offset
			unsigned int num_block = (int)log2(cache[n].block_per_entry); /// 2;
			unsigned int block_offset = (addr1 & (1<<num_block-1));
			addr1 = (addr1>>num_block);
			unsigned int entry_bit = (int)(log2(cache[n].num_entry));
			//printf("%d\n",entry_bit);
			unsigned int index = addr1  & (1 << entry_bit-1);   
			printf("%d %d\n",index,num_block); 
			unsigned int tag = (addr1>>entry_bit);
			printf("in write addr:%x\n",addr);
			printf("%u %u %u %u\n",32-(entry_bit+num_block+2),entry_bit,num_block,2);
			printf("%x %x %x %x\n",tag, index, block_offset, (addr & (1<<1)));
			//check directly entries
	// cache[level #].way_cache[way #][entry #]
			if(cache[n].way_cache[i][index].valid == 0){
				// first insert
				cache[n].way_cache[i][index].valid = 1;
				cache[n].way_cache[i][index].tag = tag;
				cache[n].way_cache[i][index].LRU = cycle;
				used_cycle += cache[n].access_cycle;
				not_full = 1;
				printf("write!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cycle);
				return;
			} else if(cache[n].way_cache[i][index].tag == tag){ // check whether addr same
				//update
				cache[n].way_cache[i][index].dirty =1;
				cache[n].way_cache[i][index].LRU = cycle;
				used_cycle += cache[n].access_cycle;
				printf("update!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cycle);
				return;
			}
		}
		if(not_full == 0){ // full
			// apply replacement policy
			// cache[level #].way_cache[way #][index]
			//FIFO		
			switch(cache[n].replace){
				case LRU:
					int min=cycle+1;
					int pos=-1;
					for (int i = 0; i < cache[n].num_ways; i++) {
						if(cache[n].way_cache[i][index].LRU < min) {
							min = cache[n].way_cache[i][index].LRU;
							pos = i;
						}
					}

				case FIFO:
			}
			/*
			switch(cache[n].replace){
				case LRU:
				case FIFO:
				}
			*/
		}
		if(cache[n].write == WT){
			used_cycle += cache[n].access_cycle;
		}
	} // for cache_level

	///////////////////////////////////////////////////////////
}

void print_result(FILE *in) {
	fprintf(in, "Level 1 Cache\n");
	fprintf(in, "Hit Count : %d\n", hit_count[0]);
	fprintf(in, "Miss Count : %d\n", miss_count[0]);
	fprintf(in, "Hit Ratio : %.3f\n", (double)hit_count[0] / (double)(hit_count[0] + miss_count[0]));
	if(cache_level == 2) {
		fprintf(in, "\nLevel 2 Cache\n");
		fprintf(in, "Hit Count : %d\n", hit_count[1]);
		fprintf(in, "Miss Count : %d\n", miss_count[1]);
		fprintf(in, "Hit Ratio : %.3f\n", (double)hit_count[1] / (double)(hit_count[1] + miss_count[1]));
	} else {
		hit_count[1] = 0;
		miss_count[1] = 0;
	}
	fprintf(in, "\nTotal Hit Ratio : %.3f\n", (double)(hit_count[0] + hit_count[1]) / (double)(hit_count[0] + miss_count[0]));
	fprintf(in, "Total cycle: %d\n", cycle);
}
