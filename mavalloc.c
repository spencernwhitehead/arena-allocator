// The MIT License (MIT)
// 
// Copyright (c) 2021, 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES UTA OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)

/* The maximum entries in our linked list / array */
#define MAX_LINKED_LIST_SIZE 10000

/* *** INTERNAL USE ONLY *** Has the list been initialized */
static int initialized = 0;        

/* *** INTERNAL USE ONLY *** The array index of the node that is the current head of the list */
static int rootNode    = 0;   

// defines enum TYPE to allow for differentiation of a hole vs a process
enum TYPE
{
	H,
	P,
}; 

/**
*
* \struct Node
* 
* \brief The node in the linked list
* 
* copied from LinkedListInPlace.c with added parameters. instead of value, 
* now includes size, type, and arena. arena replaces value's function of
* the property the linked list is sorted off of, and size/type function 
* as the other properties of the allocation.
* 
*/
struct Node 
{ 
	int in_use;  
	size_t size; 
	enum TYPE type; 
	void * arena; 
	int next; 
	int previous; 
}; 
typedef struct Node Node;

// global variable time!

// memory to be allocated, all memory stuff will be done based on this address
void * mem;
// stores selected algorithm when mavalloc is initialized
enum ALGORITHM algo;
// stores index of previous insertion for next fit algorithm
int lastInsertion = -1;
// array for storing nodes as a linked list
static Node LinkedList[MAX_LINKED_LIST_SIZE]; 


// copied from LinkedListInPlace.c unchanged
 int findFreeNodeInternal()
{
	int i = 0;
	
	/**
	 *  Start searching the array beginning at the 0th element
	 *  and once we've found an element not in use we'll drop out 
	 *  and return the index that is free. 
	*/
	for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		if (LinkedList[i].in_use == 0)
		{
			return i;
		}
	}
	return -1;
}

// copied from LinkedListInPlace.c unchanged
int insertNodeInternal(int previous, int current)
{
	/**
	 *  Initialize our current links to an invalid index
	 *  -1 signifies the end of the list on either end
	 */
 	LinkedList[current].previous = -1;
	LinkedList[current].next = -1;

    /**
	 * Make sure we have a previous node.  If there
	 * was no previous node then the previous value
	 * would be -1
	 */
	if (previous >= 0)
	{
		int temp;
		/**
		 * Connect the current node with the previous node
		 * if it exists and store off previous->next.
		 * Then set previous->next to current
		 */
		LinkedList[current].previous = previous;

		temp = LinkedList[previous].next;
		LinkedList[previous].next = current;
		LinkedList[current].next = temp;
	
	}
	else if ( LinkedList[rootNode].previous == -1 && LinkedList[rootNode].next == -1)
	{
		/* Do nothing since this is the first node in the linked list. */
	}
	/**
	 * If we have a previous value -1 we're replacing the root node
	 * so after inserting it make sure to update the rootNode
	 */
	else
	{
		LinkedList[rootNode].previous = current;
		LinkedList[current].previous = -1;
		LinkedList[current].next = rootNode;
		rootNode = current;
	}

	return 0;
}

// copied from LinkedListInPlace.c unchanged
int removeNodeInternal(int node)
{
	/**
	 * Check to make sure we haven't tried to insert a node beyond the bounds of
	 * the array.  This shouldn't ever happen.
	 */
	if (node < 0 || node >= MAX_LINKED_LIST_SIZE )
	{
		printf("ERROR: Can not remove node %d because it is out of our array bounds"
			   " of 0 ... %d\n", node, MAX_LINKED_LIST_SIZE);
		return -1;
	}

    /**
	 * And make sure that the node we've been asked to remove is actually one in use
	 */
	if (LinkedList[node].in_use == 0)
	{
		//printf("ERROR: Can not remove node %d.  It is not in use\n", node);
		return -1;
	}

	/** 
	 * Mark this node as not in-use so we can reuse it if we need to allocate
	 * another node.
	 */
	LinkedList[node].in_use = 0;

    /**	
	  * If we are removing the head of the list then we also need
	  * to make sure to update the rootNode variable as well so that
	  * it points to the new head of the list.
	  */
	if (node == rootNode)
	{
		rootNode = LinkedList[rootNode].next;
	}

	/**
	 * If we have a previous node then hook up the previous nodes next value 
	 * to point to our next value. That will cause our node to be snipped out
	 * of the linked list.
	 */
	if (LinkedList[node].previous >= 0)
	{
		LinkedList[LinkedList[node].previous].next = LinkedList[node].next;
	}

    /**	
	 * Return our next and previous to default values so this node is ready 
	 * be reused
	 */
    LinkedList[node].next = -1;
	LinkedList[node].previous = -1;

	return 0;
}


// copied from LinkedListInPlace.c unchanged
int removeNode(void * arena)
{
	/**
	  * We start searching for the node to remove at the root of the linked list
      */
	int index = rootNode;

   /**
    * Iterate over the linked list and find the node
    * containing the value we are looking to remove
    */
	while (index != -1)
	{
    /**    
	 * Once we've found the node to delete then
     * remove it.
     */
		if (LinkedList[index].arena == arena)
		{
			return removeNodeInternal(index);
		}
		index = LinkedList[index].next;
	}
	return -1;
}


// copied from LinkedListInPlace.c and changed parameters to set for the new node
// changed from "value" to size, address, and type
// also included a fix for a weird bug where the root node's next is itself
// no idea where or why that happens but I can just manually reassign it so it's fine
int insertNode(size_t size, void * arena, enum TYPE type)
{
     /** Index into the array where we will put this new node */
	int index    =  findFreeNodeInternal();   

     /** Set current to the rootNode so we start searching from the beginning */
	int current  =  rootNode;     
	
	// fixes incorrect next value for root node
	if(LinkedList[current].next == current) LinkedList[current].next = -1;
	                                          
    /** Set previous to -1 since there nothing before the first node in the list */
	int previous = -1;                          
	                                          
    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1;                        
	
    /**
     * On the first node being inserted make sure that
     * the root node has been initialized and then
     * set the initialized flag to we don't do this initialization
     * again.
     */
	if (initialized == 0)
	{
		LinkedList[0].previous = -1;
		LinkedList[0].next     = -1;
		initialized            =  1;
	}

	if (index >= 0)
	{
      /**
       * Since this list is sorted, iterate over the linked list and find which node we would
       * fit behind with our value.  Once we have found a spot then the while loop will exit
       * and previous will have the index of the node we will insert behind.
       */
		while (current >= 0 && LinkedList[current].in_use && LinkedList[current].arena < arena)
		{
			previous = current;
			current  = LinkedList[current].next;
		}
		
        /** If we found a free node and we haven't run off the end of the list */
		if (previous >= -1)
		{
 			ret = insertNodeInternal(previous, index);
		}
        /** If we ran off the end of the list then insert on the tail of the list */
		else if( current == -1 )
		{
			ret = insertNodeInternal(LinkedList[previous].previous, index);
		}

    /**
     * Now that our new node is linked in lets set the value and mark this array element
     * as being used.
     */
		LinkedList[index].size = size;
		LinkedList[index].arena = arena;
		LinkedList[index].type = type;
		LinkedList[index].in_use = 1;
	}

	return ret;
}


// copied from LinkedListInPlace.c and changed values in printf function to include more info
void printList()
{
    /** Start at the root of the linked list */
	int i = rootNode;
	
    /** Iterate over the linked list in node order and print the nodes. */
	while (initialized && i != -1 && LinkedList[i].in_use)
	{
		printf("LinkedList[%d]: add:%p size:%6ld p:%d next:%2d prev:%2d\n", i, 
			LinkedList[i].arena, LinkedList[i].size, LinkedList[i].type, 
			LinkedList[i].next, LinkedList[i].previous);
		
		i = LinkedList[i].next;
	}
}

/**
 * 
 * \fn addProcess(int i, size_t requested_size)
 * 
 * \brief adds process into hole at index i
 * 
 * once an appropriate spot has been determined by an algorithm to fit a process,
 * this function will take the hole and fit in the process by changing the hole
 * to a process with size of requested_size, and adding a new node as a hole
 * with all the leftover space. if no space is left over then the hole is not added.
 * 
 * \return address of allocated process
 */
void * addProcess(int i, size_t requested_size) 
{
	// if there's any leftover space after process is added, add new hole containing that space
	if(LinkedList[i].size - requested_size != 0) 
	{
		insertNode(LinkedList[i].size - requested_size, LinkedList[i].arena + requested_size, H);
	}
	// sets current node to type process with requested size
	LinkedList[i].type = P;
	LinkedList[i].size = requested_size;
	// sets lastInsertion for next fit algorithm
	lastInsertion = i;
	
	return LinkedList[i].arena;
}

int mavalloc_init( size_t size, enum ALGORITHM algorithm ) 
{
	// error if size not positive
	if( size < 0 ) return -1;
	
	// align and create arena
	size_t requested_size = ALIGN4( size );
	mem = malloc( ALIGN4( requested_size ) ); 
	
	// error if malloc was unsuccessful
	if( mem == NULL ) return -1;\
	
	// set algorithm global for use in the alloc function
	algo = algorithm;
	
	// create initial hole spanning the arena
	insertNode(requested_size, mem, H);
	
	return 0;
}

void mavalloc_destroy( ) 
{
	/** Start at the root of the linked list */
	int i = rootNode;
	
    /** Iterate over the linked list in node order to remove each node */
	while (initialized && i != -1 && LinkedList[i].in_use)
	{
		// saves next index so it can still be accessed after current node is removed
		int temp = LinkedList[i].next;
		
		removeNode(LinkedList[i].arena);
		i = temp;
	}
	
	// our precious, singular free
	free( mem );
	
	// no longer initialized since arena was destroyed
	initialized = 0;
	
	// stop tracking last insertion since it no longer exists
	lastInsertion = rootNode;
	
	LinkedList[rootNode].next = -1;
	
	return;
}

void * mavalloc_alloc( size_t size ) 
{
	// only return NULL on failure
	
	// align the size
	size_t requested_size = ALIGN4( size );
	
	//root of linkedList for starting point of iteration in most algorithms
	int i = rootNode;
	
	// fixes incorrect next value for root node
	if(LinkedList[i].next == i) LinkedList[i].next = -1;
	
	// declaring min and max values/indices here since I foolishly decided to use switch,
	// and switches don't allow declaring variables immediately for some reason I forgot
	// min is set to INT_MAX so it will always initially be compared to something smaller
	// same with max being 0 always being compared to something bigger
	int min = INT_MAX;
	int minIndex = -1;
	
	int max = 0;
	int maxIndex = -1;
	
	// no way, a situation where switch can be applied?
	switch( algo ) 
	{
		case NEXT_FIT:
			// put process in first hole with enough space
			// and after index from last insertion
			
			/** Start at the index of last insertion
				if no previous insertion has occured,
				will start at root of the linked list */
			if(lastInsertion != -1) i = lastInsertion;
			
			// flag for allowing looping of search for space for process
			int loop = 0;
			
			/** Iterate over the linked list to find space for process */
			while (initialized && i != -1 && LinkedList[i].in_use)
			{
				// fixes incorrect next value for current node
				if(LinkedList[i].next == i) LinkedList[i].next = -1;
				
				// checks for hole big enough to fit process
				if(LinkedList[i].type == H && LinkedList[i].size >= requested_size) 
				{
					// put process in index of hole
					return addProcess(i, requested_size);
				}
				i = LinkedList[i].next;
				
				// restarts iteration once if not started from root node
				if(i == -1 && lastInsertion != -1 && !loop) 
				{
					i = rootNode;
					loop = 1;
				}
			}
			
			break;
		case BEST_FIT:
			// put process in hole with least extra space
			
			/** Iterate over the linked list to find space for process */
			while (initialized && i != -1 && LinkedList[i].in_use)
			{
				// fixes incorrect next value for current node
				if(LinkedList[i].next == i) LinkedList[i].next = -1;
				
				// checks for hole big enough to fit process
				if(LinkedList[i].type == H && LinkedList[i].size >= requested_size) 
				{
					// stores hole into min and minIndex if size is smaller than current min
					if(LinkedList[i].size < min) {
						min = LinkedList[i].size;
						minIndex = i;
					}
				}
				i = LinkedList[i].next;
			}
			
			if(minIndex != -1)
			{
				// put process in index of hole
				return addProcess(minIndex, requested_size);
			}
			
			break;
		case WORST_FIT:
			// put process in hole with most extra space
			
			/** Iterate over the linked list to find space for process */
			while (initialized && i != -1 && LinkedList[i].in_use)
			{
				// fixes incorrect next value for current node
				if(LinkedList[i].next == i) LinkedList[i].next = -1;
				
				// checks for hole big enough to fit process
				if(LinkedList[i].type == H && LinkedList[i].size >= requested_size) 
				{
					// stores hole into max and maxIndex if size is larger than current min
					if(LinkedList[i].size > max) {
						max = LinkedList[i].size;
						maxIndex = i;
					}
				}
				i = LinkedList[i].next;
			}
			
			if(maxIndex != -1){
				// put process in index of hole
				return addProcess(maxIndex, requested_size);
			}
			
			break;
		case FIRST_FIT:
			// put process in first hole with enough space
			
			/** Iterate over the linked list to find space for process */
			while (initialized && i != -1 && LinkedList[i].in_use)
			{
				// fixes incorrect next value for current node
				if(LinkedList[i].next == i) LinkedList[i].next = -1;
				
				// checks for hole big enough to fit process
				if(LinkedList[i].type == H && LinkedList[i].size >= requested_size) 
				{
					// put process in index of hole
					return addProcess(i, requested_size);
				}
				i = LinkedList[i].next;
			}
			
			break;
	}
	return NULL;
}

void mavalloc_free( void * ptr ) 
{
	
	/** Start at the root of the linked list */
	int i = rootNode;
	
    /** Iterate over the linked list in node order to find node to free */
	while (initialized && i != -1 && LinkedList[i].in_use)
	{
		// fixes incorrect next value for root node
		if(LinkedList[i].next == i) LinkedList[i].next = -1;
		
		if(LinkedList[i].arena == ptr) 
		{
			// "frees" node by setting enum to hole
			LinkedList[i].type = H;
			
			// checks next and previous nodes to merge holes if they exist
			if(LinkedList[i].next != -1 && LinkedList[(LinkedList[i].next)].type == H) 
			{
				LinkedList[i].size += LinkedList[(LinkedList[i].next)].size;
				removeNode(LinkedList[LinkedList[i].next].arena);
			}			
			if(LinkedList[i].previous != -1 && LinkedList[LinkedList[i].previous].type == H) 
			{
				LinkedList[i].size += LinkedList[(LinkedList[i].previous)].size;
				removeNode(LinkedList[(LinkedList[i].previous)].arena);
			}
			
			break;
		}
		i = LinkedList[i].next;
	}
	
	return;
}

int mavalloc_size( ) 
{
	int number_of_nodes = 0;
	
	/** Start at the root of the linked list */
	int i = rootNode;
	
	// fixes incorrect next value for root node
	// it's never happened here but I just want to be safe
	if(LinkedList[i].next == i) LinkedList[i].next = -1;
	
    /** Iterate over the linked list in node order and adds up the nodes in use */
	while (initialized && i != -1 && LinkedList[i].in_use)
	{
		number_of_nodes++;
		i = LinkedList[i].next;
	}
	
	return number_of_nodes;
}

