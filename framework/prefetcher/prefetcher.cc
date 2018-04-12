#include "interface.hh"
#include <stdlib.h>

// Delta Correlated Prediction s

const int maxSize = 200;
const int deltaBufferSize = 50;


struct entry {
	Addr deltaBuffer[deltaBufferSize]; // buffer of delata stored
	int deltaBufferPosition; // first free position in the delta buffer
	Addr key; // Program Counter
	Addr lastaddr; // last addres
	Addr lastprefetch; // last addres prefetched
};

struct s {
	struct entry s[maxSize];
	int elements;
};


struct s t;

void loop_prefetch(AccessStat stat) {
	Addr  newDelta = stat.mem_addr - t.s[0].lastaddr;  // generate the new delta
	// if(newDelta == 0) return;
	t.s[0].lastaddr = stat.mem_addr; // store the new last addres
	if (t.s[0].deltaBufferPosition > 3) {

		Addr lastDelta = t.s[0].deltaBuffer[t.s[0].deltaBufferPosition-1]; // create a pair of deltas
    int patternMatchingPointer = t.s[0].deltaBufferPosition;
		for (int i = 0; i < deltaBufferSize-1 && patternMatchingPointer!= t.s[0].deltaBufferPosition-1; ++i) { // pattern matching

			if (t.s[0].deltaBuffer[patternMatchingPointer] == lastDelta && t.s[0].deltaBuffer[patternMatchingPointer+1] == newDelta){




        int posiblePrefetchesSize = 0;

        if (patternMatchingPointer > t.s[0].deltaBufferPosition){
          posiblePrefetchesSize = deltaBufferSize-1 - patternMatchingPointer + t.s[0].deltaBufferPosition -2;

        }else{
           posiblePrefetchesSize = t.s[0].deltaBufferPosition -2 - patternMatchingPointer;
           if(posiblePrefetchesSize<0) posiblePrefetchesSize =0;
        }


				Addr posiblePrefetches[posiblePrefetchesSize] ;
				int posPattern = patternMatchingPointer + 2;
				Addr lastPosiblePrefetch = stat.mem_addr;
				int alreadyPrefetched = 0;

				for (int j = 0 ; j < posiblePrefetchesSize; ++j) {

					posiblePrefetches[j] = lastPosiblePrefetch + t.s[0].deltaBuffer[posPattern];
					lastPosiblePrefetch = posiblePrefetches[j]; // update posible prefetch
					if(lastPosiblePrefetch == t.s[0].lastprefetch) {
						alreadyPrefetched = j; // check if the posible prefetches are already prefetch
					}
          posPattern = (posPattern+1)%deltaBufferSize;
					// DPRINTF(HWPrefetch, "5: \n");
				}
				// DPRINTF(HWPrefetch, "int J: %d int posiblePrefetches %d \n" j, posiblePrefetches);
				for (int i = alreadyPrefetched; i< posiblePrefetchesSize ; i++){
					DPRINTF(HWPrefetch, "5: \n");
					Addr toprefetch= posiblePrefetches[i];
					if (get_prefetch_bit(toprefetch)== false && toprefetch < MAX_PHYS_MEM_ADDR){
						issue_prefetch(toprefetch);
						set_prefetch_bit(toprefetch);
						t.s[0].lastprefetch= toprefetch;
					}
				}
				break;
			}

      patternMatchingPointer= (patternMatchingPointer+1)% deltaBufferSize;
		}
	}
	t.s[0].deltaBuffer[t.s[0].deltaBufferPosition]= newDelta;
	t.s[0].deltaBufferPosition= (t.s[0].deltaBufferPosition+1)% deltaBufferSize;
}

void refresh_or_insert(AccessStat stat) {
	for (int i = 0; i < t.elements; ++i) {
	      if (t.s[i].key == stat.pc) {

	// DPRINTF(HWPrefetch, "MATCH Found PC: %d number of entries in the s: %d\n", stat.pc, t.elements);
	// DPRINTF(HWPrefetch, "Array Of PC: %d %d %d %d %d %d %d %d %d\n\n\n  ",t.s[0].key,t.s[1].key,t.s[2].key,t.s[3].key,t.s[4].key,t.s[5].key,t.s[6].key,t.s[7].key,t.s[8].key);
			entry temp =  t.s[i];
			for (int j = i - 1; j >= 0; --j) {
				t.s[j+1] = t.s[j];
			}

			t.s[0] = temp;
			loop_prefetch(stat);
			return;
		}
	}

	// DPRINTF(HWPrefetch, "NO MATCH Found PC: %d number of entries in the s: %d\n", stat.pc, t.elements);
	// DPRINTF(HWPrefetch, "Array Of PC: %d %d %d %d %d %d %d %d %d\n\n\n  ",t.s[0].key,t.s[1].key,t.s[2].key,t.s[3].key,t.s[4].key,t.s[5].key,t.s[6].key,t.s[7].key,t.s[8].key);
	for (int i= maxSize-1; i > 0; --i){
		t.s[i] = t.s[i-1];
	}

	t.s[0].key= stat.pc;
	t.s[0].lastaddr = stat.mem_addr;

  for(int i = 0; i < deltaBufferSize; i++){
    t.s[0].deltaBuffer[i]= 0;
  }
  t.s[0].lastprefetch=0;
  t.s[0].deltaBufferPosition=0;

	if(t.elements < maxSize) {
		t.elements++;
	}
}

void prefetch_init(void) {
	for (int i = 0; i < maxSize; ++i){
		// set all variable of the each entry to 0;
		t.s[i].key = 0;
		t.s[i].lastaddr = 0;
		t.s[i].lastprefetch= 0;
		t.s[i].deltaBufferPosition=0;
		for(int j = 0 ; j < deltaBufferSize; j++){
			t.s[i].deltaBuffer[j]=0;
		}
		// no elements in the s yet
		t.elements=0;
	}
}

void prefetch_access(AccessStat stat)
{
	if (stat.miss == 1) {
		refresh_or_insert(stat);
	}
}

void prefetch_complete(Addr addr) {
	DPRINTF(HWPrefetch, "The prefeteched was COMPLETED: %d\n", addr);
}
