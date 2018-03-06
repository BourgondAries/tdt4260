/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"
#include <stdlib.h>

// IDEAS:
//  * since the prefetching efficiency varies during the execution of a program, we propose to adapt the number of pref etched blocks according to a//    dynamic measure of prefetching effectiveness.  Can we access the stats whule running the excution? If the first test has already finished can //    we use it to predict the effectiveness of the next test. Using also the queue size (100).
//
//  
void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

const size TABLE_SIZE = 20;

struct Stride {
  Addr pc; // instead of addss use AccessStat 
	Addr access[3];
};

Stride table[TABLE_SIZE];

void refresh_or_insert(AccessStat stat) { // checks for loops in the program with the pc 
	for (int i = 0; i < TABLE_SIZE; ++i) {
		if (table[i].pc == stat.pc) {
			Stride temp = table[i];
			for (int j = i - 1; j >= 0; --j) {
				table[j] = table[j + 1];
			}
			table[0] = temp;
			return;
		}
	}
	for (int j = TABLE_SIZE - 2; j >= 0; --j) {
		table[j] = table[j + 1];
	}
	table[0].pc = stat.pc;
	// if (table[0].access[0]!= null/0){
	//                if (table[0].access[1]!=0/null){
	//                     int distance= table[0].access[0]-table[0].access[1];
        //                    if(distance==table[0].access[1]-stat.mem_addre){
	//                          if(!in_cache(stat.mem_address+ distance)){ // maybe a loop to prefetch the posible next x blocks.
	//                               issue_prefetch(stat.mem_address+distance);
	//                          }
        //                     }
	//
	//                    for(int i=0; i>2; i++){
        //                         table[0].access[i]=table[0].access[i+1];
        //                    }
        //                    table[0].access[0] = stat.mem_addr;
	//
	//                 }
	//                 else {table[0].access[1]=stat.mem_address}
        // }
	// else {table[0].acess[0]= stat.mem_address}
	//
	//
	//
	
}

void prefetch_access(AccessStat stat)
{
	if (get_prefetch_bit(stat.mem_addr) == 0) {
		set_prefetch_bit(stat.mem_addr);
		for (int i = 0; i < 5; ++i) {
			issue_prefetch(stat.mem_addr + i * BLOCK_SIZE);
		}
	}
	// 0.998789

	// // Tagged prefetch
	// if (stat.miss) {
	// 	// Demand fetched
	// 	issue_prefetch(stat.mem_addr + BLOCK_SIZE);
	// } else {
	// 	// Prefetched
	// 	if (get_prefetch_bit(stat.mem_addr) == 0) {
	// 		set_prefetch_bit(stat.mem_addr);
	// 		issue_prefetch(stat.mem_addr + BLOCK_SIZE);
	// 	}
	// }
        //
	// // Prefetch on miss
	// Addr pf_addr = stat.mem_addr + BLOCK_SIZE;
	// if (stat.miss && !in_cache(pf_addr)) {
	// 	issue_prefetch(pf_addr);
	// }
}

// 0.999164

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
	DPRINTF(HWPrefetch, "Complete: %d\n", addr);
}
