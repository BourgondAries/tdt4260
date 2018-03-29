	
	#include "interface.hh"
	#include <stdlib.h>

	// IDEAS:
	//  * since the prefetching efficiency varies during the execution of a program, we propose to adapt the number of pref etched blocks according to a
	//    dynamic measure of prefetching effectiveness.  Can we access the stats whule running the excution? If the first test has already finished can 
	//    we use it to predict the effectiveness of the next test. Using also the queue size (100).
	//
	//  * Once the dcpt works try to speed uo the search with lookup algorithms, and ordeanmiento algorithms 

	const size_t TABLE_SIZE = 100;

	struct Stride {
	        Addr pc; // instead of addss use AccessStat 
		    Addr lastaddres;
		    Addr delta;
		    int state;
	} ;

	struct Stride table[TABLE_SIZE];


	void prefetch_init(void)
	{
	    

	  for (int i = 0; i <TABLE_SIZE ; i++){
	    table[i].pc=0;
	    table[i].lastaddres=0;
	    table[i].delta=0;   // diference between the last two addresses. (to catch loops)
	    table[i].state=0; // can be rather 0 , 1(is in the table but has no delta yet) , 2 (third time this pc produces a miss, there is alredy a delta)
	    
	  }

	    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
	    // printf("Probando 1.. 2.. 3..\n");
	}


	void loop_prefetch(AccessStat stat) {


           Addr NewDelta= stat.mem_addr - table[0].lastaddres;

           if(table[0].state== 1){
                  table[0].delta= NewDelta;
                  
                  table[0].state=2;
           }
           else if (table[0].state == 2 ){

             
           	if( NewDelta == table[0].delta && get_prefetch_bit(stat.mem_addr + NewDelta)==false){
           		issue_prefetch(stat.mem_addr + NewDelta); 
           		 set_prefetch_bit(stat.mem_addr + NewDelta);
           	}
           	else table[0].delta = NewDelta;
          

           } 	
           
           table[0].lastaddres = stat.mem_addr;

	  
	   //      if ( table[0].access[0]!=0 && table[0].access[0]!= stat){
		  //             if (table[0].access[1]!=0 && table[0].access[0]!= stat){
		  //                       Addr distance= table[0].access[1]-table[0].access[0];
	   //                          if(distance==stat.mem_addr-table[0].access[1]){
				            
				//                   for(int i=1; i>1; i++){
				//                   	  Addr temporal= stat.mem_addr+i*distance; 
				//                       if(!in_cache(temporal)&& get_prefeteched_bit(temporal)) { // maybe a loop to prefetch the posible next x blocks.
		  //                                 issue_prefetch(temporal);
		  //                                 set_prefetched_bit(temporal);
		  //                             }
				//                   }
	   //                          }
		
		  //                       for(int i=0; i>2; i++){
	   //                               table[0].access[i]=table[0].access[i+1];
	   //                          }

	   //                          table[0].access[2] = stat.mem_addr;
		
		  //               }
				// else {table[0].access[1]=stat.mem_addr;}
	   //            }
	   //      	else {table[0].access[0]= stat.mem_addr;}
	       
	      return;
	} 




	void refresh_or_insert(AccessStat stat) { // checks for loops in the program with the pc 
	

		//Tries to find the pc in the table
		for (int i = 0; i < TABLE_SIZE; ++i) {
			if (table[i].pc == stat.pc) {
			      
				Stride temp = table[i];
				for (int j = i - 1; j >= 0; --j) {
					table[j+1] = table[j];
				}
				table[0] = temp;
				loop_prefetch(stat);
				return;
			}
		}
		


		// If the pc is not found in the table it is added.
	     for (int j = TABLE_SIZE - 1; j > 0; --j) {
			table[j] = table[j - 1];
	       	}
		
	       	table[0].pc = stat.pc;
	       	table[0].lastaddres= stat.mem_addr;
	       	table[0].state=1; 
          
		
		
		
	}

	void prefetch_access(AccessStat stat)
	{

		  if(stat.miss == 1){refresh_or_insert(stat);}
		  return;
	}



	void prefetch_complete(Addr addr) {
	   
		DPRINTF(HWPrefetch, "The prefeteched was COMPLETED: %d\n", addr);
	}
