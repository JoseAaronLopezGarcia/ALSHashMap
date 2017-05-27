#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdint.h>

typedef struct ListNode{
	size_t key;
	void* value;
	struct ListNode* next;
}ListNode;

typedef struct LinkedList{
	ListNode* start;
	size_t size;
}LinkedList;

LinkedList* LinkedList_New();
void LinkedList_Set(LinkedList*, size_t key, void* value);
void* LinkedList_Get(LinkedList*, size_t key);
void* LinkedList_Del(LinkedList*, size_t key);
size_t LinkedList_Size(LinkedList*);
void LinkedList_Destroy(LinkedList*);

#ifdef __cplusplus
}
#endif

#endif
