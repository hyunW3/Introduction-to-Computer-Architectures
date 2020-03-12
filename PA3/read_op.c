/*
struct cache {
	struct cache_entry **way_cache;
	int size;
	int num_entry;
	int num_ways;
	int block_per_entry;		// # of blocks / entry
	int bytes_per_block;		// # of bytes / block (block size)
	cache[1].block_per_entry = cache[1].size / 
						cache[1].num_ways / cache[1].num_entry 
						/ cache[1].bytes_per_block;
	enum replace_policy replace;		// LRU or FIFO
	enum write_policy write;		// WB or WT
	int access_cycle;
};
struct way_cache {
	char valid;
	char dirty;
	unsigned int tag;
	/* You can add variables for your purpose  
};
	// cache[level #].way_cache[way #][entry #]
	for(n = 0; n < cache_level; n++) {
		cache[n].way_cache = (struct cache_entry **)
			malloc(sizeof(struct cache_entry *) * cache[n].num_ways);
		
		for (i = 0; i < cache[n].num_ways; i++)
			cache[n].way_cache[i] = (struct cache_entry *)
				malloc(sizeof(struct cache_entry) * cache[n].num_entry);
	

*/
void read_op(unsigned int addr) {
	/* You have to implement your own read_op function here! */
	int used_cycle=0;	
	int i=0;
	unsigned int addr1 = addr >> 2; // without byte offset
	unsigned int num_block = (int)log2(cache[i].block_per_entry); /// 2;
	unsigned int block_offset = (addr1 & (1<<num_block-1));
	addr1 = (addr1>>num_block);
	unsigned int entry_bit = (int)(log2(cache[i].num_entry));
	//printf("%d\n",entry_bit);
	unsigned int index = addr1  & (1 << entry_bit-1);   
	printf("%d %d\n",index,num_block); 
	unsigned int tag = (addr1>>entry_bit);
	printf("in write addr:%x\n",addr);
	printf("%u %u %u %u\n",32-(entry_bit+num_block+2),entry_bit,num_block,2);
	printf("%x %x %x %x\n",tag, index, block_offset, (addr & (1<<1)));
	/*
	for(n = 0; n < cache_level; n++) {
		for (i = 0; i < cache[n].num_ways; i++) { // 2 -> check both
			//check directly entries
			if((cache[n].way_cache[i][index].valid) * (tag == 
				cache[n].way_cache[i][index].tag)){ 

			}
		}
	}	
	*/
	///////////////////////////////////////////////////////////
	cycle += used_cycle;
}

void write_op(unsigned int addr) {
	/* You have to implement your own write_op function here! */

	///////////////////////////////////////////////////////////
}