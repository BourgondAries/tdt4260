#include "interface.hh"
#include <stdlib.h>


    // Delta Correlated Prediction Table 


// IndexTable 

const int maxSize = 100; 
const int deltaBufferSize = 20; 
const int posiblePrefetchesSize = 20;

struct entry{

   Addr deltaBuffer[deltaBufferSize]; // buffer of delata stored
   int deltaBufferPosition; // first free position in the delta buffer 
   Addr key; // Program Counter 
   Addr lastaddr; // last addres 
   Addr lastprefetch; // last addres prefetched 

}; 

struct table {
      struct entry table[maxSize]; 
      int elements; 
      
};


struct table DCPTtable; 





 void refresh_or_insert(AccessStat stat) {
       
       for( int i=0; i<indexTable.elements; i++){
			if (DCPTtable.table[i].key == stat.pc){
   
                entry temp =  DCPTtable[i];
				for (int j = i - 1; j >= 0; --j) {
					DCPTtable.table[j+1] = DCPTtable.table[j];
				}
                DCPTtable.table[0] = temp; 
                loop_prefetch(stat);
                return;
			}
		}


	    for (int i= maxSize-1; i > 0; i--){
             	DCPTtable.table[i] = indexTable.table[i-1];
            }

			DCPTtable.table[0].key= stat.pc;


	    if(DCPTtable.elements < maxSize){DCPTtable.elements++;}
			// insert pointer 
		
		

 }

	
   void loop_prefetch(AccessStat stat) {
          
          if (DCPTtable.table[0].lastaddr==0){// fist time this pc appears
          	

          	DCPTtable.table[0].lastaddr= stat.mem_addr; 


          }else {// NOT fist time this pc appears

          	 Addr  newDelta = DCPTtable.table[0].lastaddr - stat.mem_addr; // generate the new delta 
          	// if(newDelta == 0) return; 
          	 DCPTtable.table[0].lastaddr = stat.mem_addr; // store the new last addres 

          	 if (deltaBufferPosition < 3) {  // deltas less than 3 so no pattern matching is made.
          	 	DCPTtable.table[0].deltaBuffer[DCPTtable.table[0].deltaBufferPosition]= newDelta;
          	 	DCPTtable.table[0].deltaBufferPosition++;
          	 	


          	 }else {
                 
                 Addr lastDelta = DCPTtable.table[0].deltaBuffer[DCPTtable.table[0].deltaBufferPosition-1]; // create a pair of deltas

                 for (int i = 0; i <= deltaBufferSize; i++){ // pattern matching 

                 	if (DCPTtable.table[0].deltaBuffer[i%deltaBufferSize] == lastDelta && DCPTtable.table[0].deltaBuffer[(i+1)%deltaBufferSize] == newDelta){


                            Addr posiblePrefetches[posiblePrefetchesSize] ;
                            int posPattern = i+2;
                            Addr lastPosiblePrefetch = stat.mem_addr; 

                            int alreadyPrefetched = 0; 

                            for (int j= 0 ; j < posiblePrefetchesSize ; j++){

                            	

                                 posiblePrefetches[j] = lastPosiblePrefetch + DCPTtable.table[0].deltaBuffer[(posPattern+j)%deltaBufferSize];
                                 lastPosiblePrefetch = posiblePrefetches[j]; // update posible prefetch 

                                 if(lastPosiblePrefetch == DCPTtable.table[0].lastprefetch) alreadyPrefetched = j;  // check if the posible prefetches are already prefetch 
                            	
                            	
                            }
                            
                            for (int i = j; i< posiblePrefetchesSize ; i++){
                                     Addr toprefetch= posiblePrefetches[j];
                                   if (get_prefetch_bit(toprefetch)== false){

             			                   issue_prefetch(toprefetch);
             			                   set_prefetch_bit(toprefetch);
             			                   DCPTtable.table[0].lastprefetch= toprefetch;
             		               }
                            }
                            

                 	}

                 }

                DCPTtable.table[0].deltaBuffer[DCPTtable.table[0].deltaBufferPosition]= newDelta;
          	 	DCPTtable.table[0].deltaBufferPosition++;   

          	 }

          }



   } 



	void prefetch_init(void)
	{
		for( int i=0; i<maxSize; i++){
			DCPTtable.table[i].key = 0;
			DCPTtable.table[i].lastaddr = 0;
			DCPTtable.table[i].lastprefetch= 0;

			DCPTtable.table[i].deltaBufferPosition=0;
            DCPTtable.elements=0;

            for(int j = 0 ; j < deltaBufferSize; j++){
            	DCPTtable.table[i].deltaBuffer[j]=0;
            }

		}
	    

	   
	}


	void prefetch_access(AccessStat stat)
	{
      if(stat.miss == 1){refresh_or_insert(stat);}
		  return;
		  
	}



	void prefetch_complete(Addr addr) {
	   
		DPRINTF(HWPrefetch, "The prefeteched was COMPLETED: %d\n", addr);
	}
