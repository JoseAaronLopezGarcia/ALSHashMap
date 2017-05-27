#include "linkedlist.h"

static int findNode(LinkedList* self, size_t key, ListNode** node, ListNode** prev){
	*node = *prev = NULL;
	if (self != NULL){
		ListNode* p = NULL;
		ListNode* n = self->start;
		while (n != NULL){
			if (n->key == key){
				*node = n;
				*prev = p;
				return 1;
			}
			p = n;
			n = n->next;
		}
	}
	return 0;
}

LinkedList* LinkedList_New(){
	LinkedList* self = (LinkedList*)malloc(sizeof(LinkedList));
	self->start = NULL;
	self->size = 0;
	return self;
}

void LinkedList_Set(LinkedList* self, size_t key, void* value){
	if (self != NULL && value != NULL){
		ListNode *prev, *node;
		if (findNode(self, key, &node, &prev)){
			node->value = value;
		}
		else{
			ListNode* node = (ListNode*)malloc(sizeof(ListNode));
			node->key = key;
			node->value = value;
			node->next = self->start;
			self->start = node;
			self->size++;
		}
	}
}

void* LinkedList_Get(LinkedList* self, size_t key){
	ListNode *prev, *node;
	if (findNode(self, key, &node, &prev))
		return node->value;
	return NULL;
	
}

void* LinkedList_Del(LinkedList* self, size_t key){
	ListNode *node, *prev;
	void* value = NULL;
	if (findNode(self, key, &node, &prev)){
		if (prev==NULL) self->start = node->next;
		else prev->next = node->next;
		value = node->value;
		free(node);
		self->size--;
	}
	return value;
}

size_t LinkedList_Size(LinkedList* self){
	return self->size;
}

void LinkedList_Destroy(LinkedList* self){
	ListNode* node = self->start;
	while (node != NULL){
		ListNode* aux = node;
		node = node->next;
		free(aux);
	}
	free(self);
}
