#include "interface.hh"
#include <stdlib.h>


    // Delta Correlated Prediction Table 




const int maxSize = 100; 
const int deltaBufferSize = 20; 
const int posiblePrefetchesSize = 10;

int prueba= 0; 

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

void loop_prefetch(AccessStat stat) {
          
         

       Addr  newDelta = stat.mem_addr - DCPTtable.table[0].lastaddr;  // generate the new delta 
       //if(newDelta == 0) return; 
       DCPTtable.table[0].lastaddr = stat.mem_addr; // store the new last addres 

            
             if (DCPTtable.table[0].deltaBufferPosition > 3){

                   
                 
                 Addr lastDelta = DCPTtable.table[0].deltaBuffer[DCPTtable.table[0].deltaBufferPosition-1 % deltaBufferSize]; // create a pair of deltas

                 for (int i = 0; i <= deltaBufferSize; i++){ // pattern matching 
                        

                         if (DCPTtable.table[0].deltaBuffer[i] == lastDelta && DCPTtable.table[0].deltaBuffer[i+1] == newDelta){
                             

                            Addr posiblePrefetches[posiblePrefetchesSize] ;
                            int posPattern = i+2;
                            Addr lastPosiblePrefetch = stat.mem_addr; 

                            int alreadyPrefetched = 0; 

                            for (int j= 0 ; j < posiblePrefetchesSize ; j++){

                             

                                 posiblePrefetches[j] = lastPosiblePrefetch + DCPTtable.table[0].deltaBuffer[(posPattern+j)%deltaBufferSize];
                                 lastPosiblePrefetch = posiblePrefetches[j]; // update posible prefetch 

                                 if(lastPosiblePrefetch == DCPTtable.table[0].lastprefetch) alreadyPrefetched = j;  // check if the posible prefetches are already prefetch 
                              
                              // DPRINTF(HWPrefetch, "5: \n");
                            }
                            
                            // DPRINTF(HWPrefetch, "int J: %d int posiblePrefetches %d \n" j, posiblePrefetches);

                            for (int i = alreadyPrefetched; i< posiblePrefetchesSize ; i++){

                              DPRINTF(HWPrefetch, "5: \n");
                                     Addr toprefetch= posiblePrefetches[j];

                                   if (get_prefetch_bit(toprefetch)== false && toprefetch < MAX_PHYS_MEM_ADDR){
                                        
                                     issue_prefetch(toprefetch);
                                     set_prefetch_bit(toprefetch);
                                     DCPTtable.table[0].lastprefetch= toprefetch;
                               }
                            }
                            
                            break;

                         }

                    }

               }

          
        DCPTtable.table[0].deltaBuffer[DCPTtable.table[0].deltaBufferPosition % deltaBufferSize]= newDelta;
        DCPTtable.table[0].deltaBufferPosition++;



   } 





 void refresh_or_insert(AccessStat stat) {
       
      
       for( int i=0; i<DCPTtable.elements; i++){
			      if (DCPTtable.table[i].key == stat.pc){
                 
                  // DPRINTF(HWPrefetch, "MATCH Found PC: %d number of entries in the table: %d\n", stat.pc, DCPTtable.elements);
                  // DPRINTF(HWPrefetch, "Array Of PC: %d %d %d %d %d %d %d %d %d\n\n\n  ",DCPTtable.table[0].key,DCPTtable.table[1].key,DCPTtable.table[2].key,DCPTtable.table[3].key,DCPTtable.table[4].key,DCPTtable.table[5].key,DCPTtable.table[6].key,DCPTtable.table[7].key,DCPTtable.table[8].key);
             
               
                entry temp =  DCPTtable.table[i];
				        for (int j = i - 1; j >= 0; --j) {
				         	DCPTtable.table[j+1] = DCPTtable.table[j];
				         }
                
                DCPTtable.table[0] = temp; 
                loop_prefetch(stat);
                return;
			    }
		    }

     // DPRINTF(HWPrefetch, "NO MATCH Found PC: %d number of entries in the table: %d\n", stat.pc, DCPTtable.elements);
     // DPRINTF(HWPrefetch, "Array Of PC: %d %d %d %d %d %d %d %d %d\n\n\n  ",DCPTtable.table[0].key,DCPTtable.table[1].key,DCPTtable.table[2].key,DCPTtable.table[3].key,DCPTtable.table[4].key,DCPTtable.table[5].key,DCPTtable.table[6].key,DCPTtable.table[7].key,DCPTtable.table[8].key);
	  
      for (int i= maxSize-1; i > 0; --i){
             

             	DCPTtable.table[i] = DCPTtable.table[i-1];
            }

			DCPTtable.table[0].key= stat.pc;
      DCPTtable.table[0].lastaddr = stat.mem_addr;


	    if(DCPTtable.elements < maxSize){DCPTtable.elements++;}
			// insert pointer 
		
		

 }

	
   

	void prefetch_init(void)
	{
		for( int i=0; i<maxSize; i++){

      // set all variable of the each entry to 0;
			DCPTtable.table[i].key = 0;
			DCPTtable.table[i].lastaddr = 0;
			DCPTtable.table[i].lastprefetch= 0;
			DCPTtable.table[i].deltaBufferPosition=0;
      for(int j = 0 ; j < deltaBufferSize; j++){
              DCPTtable.table[i].deltaBuffer[j]=0;
      }


      // no elements in the table yet 
      DCPTtable.elements=0;

      

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
