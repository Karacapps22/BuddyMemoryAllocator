
#include <stdlib.h>
#include <stdio.h>
#include "Ackerman.h"
#include "BuddyAllocator.h"
//#include "BuddyAllocator.cpp"
//#include "Ackerman.cpp"
#include <getopt.h>


void easytest(BuddyAllocator* ba){

    ba->printlist();
    cout << "allocating 1 byte" << endl;
    char * mem = (char *) ba->alloc (1);
    ba->printlist();
    cout << endl;
    
    cout << "allocating 100 bytes" << endl;
    char * mem1 = (char *) ba->alloc (100);
    ba->printlist();
    cout << endl;
    
    cout << "allocating 200 bytes" << endl;
    char * mem3 = (char *) ba->alloc (200);
    ba->printlist();
    cout << endl;

    
    
  cout << "freeing 1 byte " << endl;
  ba->free (mem);
    ba->printlist();
    cout << endl;
    
    
  cout << "freeing 100 byte " << endl;
  ba->free (mem1);
    ba->printlist();
    cout << endl;
    
    cout << "freeing 200 byte " << endl;
  ba->free (mem3);
    ba->printlist();
    cout << endl;
    
    

  
  cout << "***test complete" << endl;
  ba->printlist(); // shouldn't the list now look like as in the beginning




}



int main(int argc, char** argv) {
    
    int opt=0;
    int basic_block_size=128, memory_length=512*1024;
    
    while((opt=getopt(argc, argv, "b:s:")) !=-1){
        switch(opt){
            case 'b':
                basic_block_size = atoi(optarg);
                break;
            case 's':
                memory_length = atoi(optarg);
                break;
        }//end block for switch
      }  //end block for whil

    cout << basic_block_size <<endl;
    cout << memory_length << endl;

  // create memory manager

  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

  // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
  easytest (allocator);


  // stress-test the memory manager, do this only after you are done with small test cases
    cout << endl;
    cout << "testing ackerman" << endl;
    
  Ackerman* am = new Ackerman ();
  am->test(allocator);
    


    
    

  
  // destroy memory manager
  delete allocator;
}
