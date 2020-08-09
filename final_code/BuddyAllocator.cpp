
/*
 File: my_allocator.cpp
 */

#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator(int _basic_block_size, int _total_memory_length)
{
    //initialize variables and allocate memory
    basic_block_size = _basic_block_size;
    total_memory_size = _total_memory_length;
    mem = new char[_total_memory_length];

    //    printf("memory space : [%llx, %llx]\n", mem,mem+_total_memory_length-1);

    //calculate how many indices
    int vecLength = log2(_total_memory_length / _basic_block_size) + 1;
    FreeList.resize(vecLength);
    //cout << FreeList.size() << endl;

    BlockHeader *block = (BlockHeader *)mem;
    block->block_size = _total_memory_length;
    block->used = false;
    block->next = nullptr;
    //cout << "ok" << endl;
    FreeList[FreeList.size() - 1].insert(block);

    debug();
}

BuddyAllocator::~BuddyAllocator()
{
    for (int i = 0; i < FreeList.size(); i++)
    {
        BlockHeader *curr = FreeList[i].getFirst();
        while (curr != nullptr)
        {
            FreeList[i].remove(curr);
            curr = curr->next;
        }
    }
    delete mem;
}

void *BuddyAllocator::alloc(int length)
{
    
    int newLength = pow(2, ceil(log2((double)length + sizeof(BlockHeader))));
    if (newLength < basic_block_size)
    {
        newLength = basic_block_size;
    }
    //cout << "wanted: " << length << ", new: " << newLength << endl;

    //find which linked lists to look in and check them for free blocks
    int index = log2(newLength / basic_block_size);
    BlockHeader *addr = nullptr;
    
    for (int i = index; i < FreeList.size(); i++)
    {
        if (FreeList[i].getFirst() != nullptr)
        {
            addr = FreeList[i].getFirst();
            break;
        }
    }

    //if it couldn't find a free block, then it returns nullptr
    if (addr == nullptr)
    {
        return nullptr;
    }

    //split block as needed
    if (addr->block_size > newLength)
    {
        int num = log2(addr->block_size / newLength);
        for (int i = 0; i < num; i++)
        {
            //cout << i << endl;
            addr = split(addr);
        }
    } //else remove the block from the appropriate list and edit its header
    else
    {
    //cout << "end of second if statement " << endl;
        int index = log2(addr->block_size)-log2(basic_block_size);
        FreeList[index].remove(addr);
        addr->used = true;
        addr->next = nullptr;
    }
    debug();

    addr = addr +1;
    return addr;
}

void *BuddyAllocator::free(void* a)
{

    BlockHeader *addr = (BlockHeader *)a;
    addr=addr-1;
    

    int index =  log2(addr->block_size)-log2(basic_block_size);
    FreeList[index].insert(addr);

    addr->used = false;
    debug();
    BlockHeader *buddy = getbuddy(addr);

    //check the size equality, and that they are free at the moment
    bool isbuddy = arebuddies(addr, buddy);
    if (isbuddy)
    {

       // printlist();
        BlockHeader *new_block = merge(addr, buddy);

       // printlist();
        while (new_block)
        {
            BlockHeader* new_addr = (BlockHeader*)new_block;
            //use new addr and find new buddy

            BlockHeader *new_buddy = getbuddy(new_addr);


            //if they are free, call merge again, else exit while loop
            isbuddy = arebuddies(new_addr, new_buddy);
            if (!isbuddy)
            {
                new_block = nullptr;
            }
            else
            {
                index +=1;

                //printlist();
                new_block = merge(new_addr, new_buddy);

                //printlist();
            }
        }
    }
    debug();
    return 0;
}

BlockHeader *BuddyAllocator::split(BlockHeader *block)
{
    //if half the block size is >= the basic_block_size size then go ahead and do the split
    if (block->block_size / 2 >= basic_block_size)
    {
        
        
        //find the linked list with the block and remove the block from the list
        int index = log2(block->block_size) - log2(basic_block_size);

        FreeList[index].remove(block);

        //place a new block header halfway through the orginal block and edit the block headers accordingly
        char *charMid = (char *)block;
        charMid = charMid + (block->block_size/ 2);
        BlockHeader * bl= (BlockHeader*)charMid;

        //edit blockheaders

        block->block_size = (block->block_size)/2;
        block->used = true;
        block->next = nullptr;

        bl->block_size = block->block_size;
    
        bl->used = false;
        bl->next = nullptr;
 
        index -= 1;
 
        FreeList[index].insert(bl);
        debug();
        return block;
    }
    return nullptr;
}

BlockHeader *BuddyAllocator::merge(BlockHeader *block1, BlockHeader *block2)
{

    //remove both blocks from their linked list

    int index = log2(block1->block_size) - log2(basic_block_size);;

    FreeList[index].remove(block1);
    FreeList[index].remove(block2);
   // cout << "after removing both blocks in merge" << endl;

    //find which block is on the left and edit it's header
    if (block1 < block2)
    {

        //set the right block to default values
        block2->block_size = 0;
        block2->used = true;
        block2->next = nullptr;
        //update left block
        block1->block_size = block1->block_size * 2;
        block1->used = false;
        block1->next = nullptr;
        FreeList[index + 1].insert(block1);
        
        debug();
        return block1;
    }
    else
    {
        //block2->edit((block2->block_size)*2, false, nullptr);
        //set right block to default values

        block1->block_size = 0;
        block1->used = true;
        block1->next = nullptr;
        //update left block
        block2->block_size = block2->block_size * 2;
        block2->used = false;
        block2->next = nullptr;

        FreeList[index + 1].insert(block2);
        debug();
        return block2;
    }
    return nullptr;
}

BlockHeader *BuddyAllocator::getbuddy(BlockHeader *addr)
{
    return (BlockHeader *)((((char *)addr - mem) ^ addr->block_size) + mem);
}

bool BuddyAllocator::arebuddies(BlockHeader *block1, BlockHeader *block2)
{
    return ((block1->block_size == block2->block_size) && (block1->used == false) && (block2->used == false));
}

void BuddyAllocator::printlist (){
  cout << "Printing the FreeList in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  for (int i=0; i<FreeList.size(); i++){
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList [i].getFirst();
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
    cout << count;
    cout << " Amount of available free memory: " << total_free_memory << " bytes" << endl;  
  }
}
  
  void BuddyAllocator::debug()
{
   
    for (int i = 0; i < FreeList.size(); i++)
    {
   
        BlockHeader *curr = FreeList[i].getFirst();
        int num = 0;
        while (curr != nullptr)
        {
            num++;
 
            curr = curr->next;
        }
   
    }

}
//};
