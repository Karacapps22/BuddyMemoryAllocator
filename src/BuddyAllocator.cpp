#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length=128){
  basic_block_size = _basic_block_size, total_memory_size = _total_memory_length;
  char* start = new char[total_memory_size];
  int new_memory_size = total_memory_size;
  void* totalMem = malloc(total_memory_size);
  int total = (int)(log2(total_memory_size/basic_block_size));
  int x = log2(total_memory_size)-log2(basic_block_size);
  
  for(int i= _basic_block_size; i< total_memory_size; i *=2){
    LinkedList* Linked = new LinkedList();
    FreeList.push_back(*Linked);
  }
  
  BlockHeader* BH = new BlockHeader();
  BH->block_size= total_memory_size;

  FreeList[x].insert(BH);
  
}

BuddyAllocator::~BuddyAllocator (){
	//delete start;
}

void* BuddyAllocator::alloc(int length) {
  
  int y=log2(total_memory_size)-log2(basic_block_size);
  for(int i =0; i<y; i++){
    if(log2(length) == y){
      if(FreeList[y].head != nullptr){
        new_memory_size += pow(y,2);
        FreeList[y].remove(FreeList[y].head);
      }
      else{
        if(abs(log2(length)-y)==1){
          if(FreeList[y+1].head != nullptr){
            FreeList[y+1].split(FreeList[y+1].head);
            new_memory_size += pow(y,2);
          }
        }
      }
    }
  }
}

//void* a is address of a Block
void BuddyAllocator::free(void* a) {
  int y = log2(total_memory_size)-log2(basic_block_size);
  BlockHeader* bH = new BlockHeader();
  bH->block_size=a;
  for(int i=0; i<y; i++){
    if(i==bH->block_size){
      FreeList[i].insert(bH);
    }
    else if(i-ceil(log2(bH->block_size))==1){
      
    }
  //::free (a);
  }
}

// BlockHeader* getbuddy (BlockHeader * addr){
//   //ptr=addr + 1;
//   //return ptr;??
// }
// 	// given a block address, this function returns the address of its buddy 
	
// bool arebuddies (BlockHeader* block1, BlockHeader* block2){
  
// }
// 	// checks whether the two blocks are buddies are not
// 	// note that two adjacent blocks are not buddies when they are different sizes

// BlockHeader* merge (BlockHeader* block1, BlockHeader* block2){}
// 	// this function merges the two blocks returns the beginning address of the merged block
// 	// note that either block1 can be to the left of block2, or the other way around

// BlockHeader* split (BlockHeader* block){
//   //split FreeList[x+1] into 2 and put 1 half in FreeList[x]
// }
// 	// splits the given block by putting a new header halfway through the block
// 	// also, the original header needs to be corrected
	

void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  for (int i=0; i<FreeList.size(); i++){
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      total_free_memory += blocksize;
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != blocksize){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;
    cout << "Amount of available free memory: " << total_free_memory << " byes" << endl;  
  }
}


