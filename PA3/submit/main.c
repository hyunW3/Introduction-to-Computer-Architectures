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
				printf("\nread_op:%x",phy_addr);
				read_op(phy_addr);
				break;
			case 'W':	//Write Operation
				fscanf(fd_input, "%x", &phy_addr);
				printf("\nwrite_op:%x",phy_addr);
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
	int hit[2],miss[2]; //update to hit_count,miss_count
	int used_cycle=0;
	unsigned int num_block,block_offset,entry_bit,index,tag;
	for(int n = 0; n < cache_level; n++) {
		hit[n]=miss[n]=0;
		printf("-----level:%d-----\n",n+1);
		int not_full=0;		
		for (int i = 0; i < cache[n].num_ways; i++) { // 2 -> check all way of certain entry
			unsigned int addr1 = addr >> 2; // without byte offset
			num_block = (int)log2(cache[n].block_per_entry); /// 2;
			block_offset = (addr1 & (1<<num_block-1));
			addr1 = (addr1>>num_block);
			entry_bit = (int)(log2(cache[n].num_entry));
			//printf("%d\n",entry_bit);
			index = addr1  & (1 << entry_bit-1);   
			//printf("%d %d\n",index,num_block); 
			tag = (addr1>>entry_bit);
			printf("%u %u %u %u\n",32-(entry_bit+num_block+2),entry_bit,num_block,2);
			printf("%x %x %x %x\n",tag, index, block_offset, (addr & (1<<1)));
			//check directly entries
			// cache[level #].way_cache[way #][entry #]
			if((cache[n].way_cache[i][index].valid == 1) &&
				(cache[n].way_cache[i][index].tag == tag)){
				printf("hit!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cycle);
				hit[n]=1;
				used_cycle += cache[n].access_cycle;

				hit_count[n] += hit[n];
				miss_count[n] += miss[n];
				printf("hit:%d,miss:%d\n",hit[n],miss[n]);

				printf("total Level_1 : hit:%d,miss:%d\n",hit_count[0],miss_count[0]);
				printf("total Level_2: hit:%d,miss:%d\n",hit_count[1],miss_count[1]);
				cycle += used_cycle;
				printf("%x used_cycle :%d\n",addr,used_cycle);
				used_cycle=0;
				return; // find the data
			}else {
				//printf("i:%d vs num_ways-1:%d\n",i,cache[n].num_ways-1);
				if(i >= cache[n].num_ways-1){ // search all ways, but no data
					printf("miss!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cycle);
					miss[n]=1;
					used_cycle += cache[n].access_cycle;					
				}
			}

		} // for i num_ways
		printf("hit:%d,miss:%d\n",hit[n],miss[n]);
		hit_count[n] += hit[n];
		miss_count[n] += miss[n];
		printf("total Level_1 : hit:%d,miss:%d\n",hit_count[0],miss_count[0]);
		printf("total Level_2: hit:%d,miss:%d\n",hit_count[1],miss_count[1]);
	} // for n, cachelevel
	// escape loop mean : cannot find in the cache
	// then find in the mem & update to cache
	if((miss[0]*miss[1] ==1) ){ 
		used_cycle += memory_access;
		printf("MEM Fetch! of %u\n",memory_access);
		for(int n = 0; n < cache_level; n++) {
			for (int i = 0; i < cache[n].num_ways; i++) { // 2 -> check all way of certain entry
				// find vocant place or replace
				//update
				if(cache[n].way_cache[i][index].valid == 0){
					cache[n].way_cache[i][index].valid = 1;
					cache[n].way_cache[i][index].tag = tag;
					cache[n].way_cache[i][index].LRU = cycle;
					break;
				}
				if(i == cache[n].num_ways-1){
					//find all ways, but no place to put -> replace
					int min= cycle+10; // set max
					int pos=-1;
					for (int i = 0; i < cache[n].num_ways; i++) {
						if(cache[n].way_cache[i][index].LRU < min){
							min = cache[n].way_cache[i][index].LRU;
							pos =i;
						}
					}
					//replace
					if(cache[n].way_cache[pos][index].dirty == 1){
					// WB evicted data should be update
						if(cache[n].write == WB){
						used_cycle += memory_access;
						printf("WB!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,pos+1,memory_access);
						}
					}
					// insert at min LRU cycle
					cache[n].way_cache[pos][index].valid = 1;
					cache[n].way_cache[pos][index].tag = tag;
					cache[n].way_cache[pos][index].LRU = cycle;
					printf("REPLACE!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,pos+1,cache[n].access_cycle);
				} // replace end
			}
		}
	}
	cycle += used_cycle;
	printf("%x used_cycle :%d\n",addr,used_cycle);
	used_cycle=0;
	///////////////////////////////////////////////////////////
}

void write_op(unsigned int addr) {
	/* You have to implement your own write_op function here! */
	printf("-----------write process\n");
	int hit[2],miss[2]; // do not update to hit_count,miss_count _> false
	int used_cycle=0;
	int pos=-1; // for WT or WB 
	unsigned int num_block,block_offset,entry_bit,index,tag;
	for(int n = 0; n < cache_level; n++) {
		hit[n]=miss[n]=0;
		printf("-----level:%d-----\n",n+1);
		int not_full=0;		
		for (int i = 0; i < cache[n].num_ways; i++) { // 2 -> check all way of certain entry
			unsigned int addr1 = addr >> 2; // without byte offset
			num_block = (int)log2(cache[n].block_per_entry); /// 2;
			block_offset = (addr1 & (1<<num_block-1));
			addr1 = (addr1>>num_block);
			entry_bit = (int)(log2(cache[n].num_entry));
			//printf("%d\n",entry_bit);
			index = addr1  & (1 << entry_bit-1);   
			printf("%d %d\n",index,num_block); 
			tag = (addr1>>entry_bit);
			printf("%u %u %u %u\n",32-(entry_bit+num_block+2),entry_bit,num_block,2);
			printf("%x %x %x %x\n",tag, index, block_offset, (addr & (1<<1)));
			//check directly entries
			// cache[level #].way_cache[way #][entry #]
			if(cache[n].way_cache[i][index].valid == 0){
				// first insert(miss)
				cache[n].way_cache[i][index].valid = 1;
				cache[n].way_cache[i][index].tag = tag;
				cache[n].way_cache[i][index].LRU = cycle;
				used_cycle += cache[n].access_cycle;
				not_full = 1;
				pos = i;
				miss[n]++;
				printf("write!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cache[n].access_cycle);
				//if(n==1) return; // insert level 1&2 cache 
				break;
			} else if(cache[n].way_cache[i][index].tag == tag){ // check whether addr same
				//update (hit)
				if(cache[n].write == WB) cache[n].way_cache[i][index].dirty =1;
				if(cache[n].replace == LRU) cache[n].way_cache[i][index].LRU = cycle;
				used_cycle += cache[n].access_cycle;
				not_full = 1;
				hit[n]++;
				printf("update!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,i+1,cache[n].access_cycle);
				//if(n==1) return; //update data end
				if(cache[n].write == WT ){ 
					if((cache_level==2)&& (n==0)) {
						used_cycle += cache[1].access_cycle;
						printf("WT!  @level:%d,index:%d at %u\n",n+1,index,cache[1].access_cycle);
						if(cache[1].write == WB) cache[1].way_cache[i][index].dirty = 1;
					}
					else if((cache_level==1)||(n==1)) {
						used_cycle += memory_access;
						printf("WT!  @level:%d,index:%d at %u\n",n+1,index,memory_access);
					}
				}
				
				printf("hit:%d,miss:%d\n",hit[n],miss[n]);
				hit_count[n] += hit[n];
				miss_count[n] += miss[n];
				printf("total Level_1 : hit:%d,miss:%d\n",hit_count[0],miss_count[0]);
				printf("total Level_2: hit:%d,miss:%d\n",hit_count[1],miss_count[1]);
				cycle += used_cycle;
				printf("%x used_cycle :%d\n",addr,used_cycle);
				used_cycle=0;				
				return;
				
			}
		} // end for (int i = 0; i< num_ways..
		if(not_full == 0){ // full (miss) -> replace
			// apply replacement policy
			// cache[level #].way_cache[way #][index]
			/*if FIFO, update data don't update LRU(cycle when data access)
			so, small LRU cycle == First in
			if LRU, update data mean update LRU
			so, small LRU cycle == least recently used
			anyway find the min(LRU cycle)
			*/
			int min= cycle+10; // set max
			pos=-1;
			for (int i = 0; i < cache[n].num_ways; i++) {
				if(cache[n].way_cache[i][index].LRU < min){
					min = cache[n].way_cache[i][index].LRU;
					pos =i;
				}
			}
			//replace
			if(cache[n].write == WB){
				// WB evicted data should be update
				if(cache[n].way_cache[pos][index].dirty == 1){
					used_cycle += memory_access;
					printf("WB!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,pos+1,memory_access);
				}
			}
			
			// insert at min LRU cycle
			cache[n].way_cache[pos][index].valid = 1;
			cache[n].way_cache[pos][index].tag = tag;
			cache[n].way_cache[pos][index].LRU = cycle;
			used_cycle += cache[n].access_cycle;
			miss[n]++;
			printf("REPLACE!  @level:%d,index:%d,ways:%d at %u\n",n+1,index,pos+1,cache[n].access_cycle);
			
			//if(n==1) return; // replace end
		} // end if(not_full)
		printf("hit:%d,miss:%d\n",hit[n],miss[n]);
	} // for cache_level
	//miss case (not in cache)  
	if((cache_level == 2)&&(miss[0]*miss[1] ==1)){
		used_cycle += memory_access;
		printf("MEM Fetch! of %u\n",memory_access);
		if(cache[0].write == WT){
			used_cycle += cache[1].access_cycle;
			printf("WT!(level 1) of %u\n",cache[1].access_cycle);
			if(cache[1].write == WB) cache[1].way_cache[pos][index].dirty = 1;
			else {
				used_cycle += memory_access;
				printf("WT!(level 2) of %u\n",memory_access);
			}
			// if WT -> access mem?
		}		
	}else if((cache_level == 1)&&(miss[0]==1)){ 
		used_cycle += memory_access;
		printf("MEM Fetch! of %u\n",memory_access);

	}
	for(int n = 0; n < cache_level; n++) {
		hit_count[n] += hit[n];
		miss_count[n] += miss[n];
	}
		printf("total Level_1 : hit:%d,miss:%d\n",hit_count[0],miss_count[0]);
		printf("total Level_2: hit:%d,miss:%d\n",hit_count[1],miss_count[1]);		
	cycle += used_cycle;
	printf("%x used_cycle :%d\n",addr,used_cycle);
	used_cycle=0;
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
