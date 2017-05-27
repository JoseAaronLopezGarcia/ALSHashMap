#ifndef ALS_HASHMAP
#define ALS_HASHMAP

/* CONFIG OPTIONS */
// use binary search tree
//#define USE_TREE
//#define EXTENDED_PROBING
//#define UNIQUE_AUX

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef USE_TREE
#include "bst.h"
#else
#include "linkedlist.h"
#endif

typedef enum{
	E, // empty
	L, // contains item belonging here
	B, // contains item not belonging here
	A // contains data structure
}ALSBucketType;

typedef struct ALSNode{
	ALSBucketType type; // node type
	union{
		// <key, value> pair
		struct{
			size_t key;
			void* value;
		} pair;
		// auxiliary data structure
		#ifdef UNIQUE_AUX
		size_t NA;
		#else
		#ifdef USE_TREE
		BinaryTree* col;
		#else
		LinkedList* col;
		#endif
		#endif
		
	} bucket;
}ALSNode;

typedef struct ALSHashMap{
	ALSNode* table;
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	BinaryTree* col;
	#else
	LinkedList* col;
	#endif
	#endif
	size_t T; // table size
	size_t N; // number of elements
	size_t R; // probe range
	// stats
	size_t CRC; // counter of nodes that can be reallocated
	size_t NA; // number of auxiliary nodes
	size_t NE; // number of empty nodes
	size_t MA; // maximum number of items in auxiliary data structures
}ALSHashMap;

// create a new (empty) table
ALSHashMap* ALSHashMap_New();

// add or update an element
void ALSHashMap_Set(ALSHashMap*, size_t key, void* value);

// obtain an element
void* ALSHashMap_Get(ALSHashMap*, size_t key);

// delete an element
void* ALSHashMap_Del(ALSHashMap*, size_t key);

// get number of elements
size_t ALSHashMap_Size(ALSHashMap*);

// destroy a table (free all memory)
void ALSHashMap_Destroy(ALSHashMap*);

void ALSHashMap_PrintStats(ALSHashMap*);

#ifdef __cplusplus
}
#endif

#endif
