#include "alsmap.h"

#define INITIAL_SIZE 8
#define INITIAL_RANGE 4

// Table's growth policies
#define CRC_THRESHOLD 0.5f
#ifdef USE_TREE
#define NA_THRESHOLD 0.3f
#define MA_THRESHOLD 1.5f
#else
#define NA_THRESHOLD 0.3f
#define MA_THRESHOLD 1.5f
#endif

// table's shrink policies
#define MIN_LF 0.7f

#define min(x, y) ((x<y)? x:y)
#define max(x, y) ((x>y)? x:y)

// Private functions
static void resizeTable(ALSHashMap* self, size_t newsize);
static void redistributeAux(ALSHashMap* self);
static void reallocBuckets(ALSHashMap* self, int i);
static ALSNode* probeBuckets(ALSHashMap* self, size_t key, int i);

ALSHashMap* ALSHashMap_New(){
	ALSHashMap* self = (ALSHashMap*)malloc(sizeof(ALSHashMap));
	self->table = (ALSNode*)malloc(sizeof(ALSNode)*INITIAL_SIZE);
	self->T = INITIAL_SIZE;
	self->N = 0;
	self->R = INITIAL_RANGE;
	self->CRC = 0;
	self->NA = 0;
	self->NE = INITIAL_SIZE;
	self->MA = 0;
	int i=0; for (; i<INITIAL_SIZE; i++){
		self->table[i].type = E;
	}
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	self->col = BinaryTree_New();
	#else
	self->col = LinkedList_New();
	#endif
	#endif
	return self;
}

void ALSHashMap_Set(ALSHashMap* self, size_t key, void* value){
	if (self != NULL && value != NULL){
		int i = key%self->T;
		ALSNode* node = &(self->table[i]);
		switch (node->type){
			case E: // empty bucket, insert directly
				node->bucket.pair.key = key;
				node->bucket.pair.value = value;
				node->type = L;
				self->N++;
				self->NE--;
				break;
			case L: // occupied by item that belongs here
				{
					ALSNode* probe;
					if (node->bucket.pair.key == key){ // update value
						node->bucket.pair.value = value;
					}
					else if ((probe = probeBuckets(self, key, i))!=NULL){ // probe bucket
						if (probe->type == E){
							self->NE--;
							self->N++;
							probe->type = B;
							probe->bucket.pair.key = key;
						}
						probe->bucket.pair.value = value;
					}
					else{ // use auxiliary data structure
						#ifdef USE_TREE
						#ifdef UNIQUE_AUX
						BinaryTree* aux = self->col;
						#else
						BinaryTree* aux = BinaryTree_New();
						#endif
						BinaryTree_Set(aux, key, value);
						BinaryTree_Set(aux, node->bucket.pair.key, node->bucket.pair.value);
						#else
						#ifdef UNIQUE_AUX
						LinkedList* aux = self->col;
						#else
						LinkedList* aux = LinkedList_New();
						#endif
						LinkedList_Set(aux, key, value);
						LinkedList_Set(aux, node->bucket.pair.key, node->bucket.pair.value);
						#endif
						#ifndef UNIQUE_AUX
						node->bucket.col = aux;
						#endif
						node->type = A;
						self->N++;
						reallocBuckets(self, i);
						if (aux->size > self->MA) self->MA = aux->size;
						self->NA++;
					}
				}
				break;
			case B: // occupied by item that doesn't belong here -> do cuckoo hashing
				{
					size_t auxkey = node->bucket.pair.key;
					void* auxvalue = node->bucket.pair.value;
					node->bucket.pair.key = key;
					node->bucket.pair.value = value;
					node->type = L;
					ALSHashMap_Set(self, auxkey, auxvalue);
				}
				break;
			case A: // add to auxiliary data structure
				{
					if ((int)(key%(2*self->T))!=i) self->CRC++;
					#ifdef USE_TREE
					#ifdef UNIQUE_AUX
					BinaryTree* aux = self->col;
					#else
					BinaryTree* aux = node->bucket.col;
					#endif
					size_t list_size = BinaryTree_Size(aux);
					BinaryTree_Set(aux, key, value);
					size_t newsize = BinaryTree_Size(aux);
					#else
					#ifdef UNIQUE_AUX
					LinkedList* aux = self->col;
					#else
					LinkedList* aux = node->bucket.col;
					#endif
					size_t list_size = LinkedList_Size(aux);
					LinkedList_Set(aux, key, value);
					size_t newsize = LinkedList_Size(aux);
					#endif
					self->N += newsize-list_size;
					if (newsize > self->MA) self->MA = newsize;
				}
				break;
		}
	}
	
	int grow = 0;
	
	#ifdef UNIQUE_AUX
	grow = ((float)self->CRC/self->N >= CRC_THRESHOLD ||
			(float)self->NA/self->T >= NA_THRESHOLD ||
			(float)self->MA >= self->T);
	#else
	grow = ((float)self->CRC/self->N >= CRC_THRESHOLD ||
			(float)self->NA/self->T >= NA_THRESHOLD ||
			(float)self->MA/self->R >= MA_THRESHOLD);
	#endif
	
	if (grow)
		resizeTable(self, 2*self->T);
}

void* ALSHashMap_Get(ALSHashMap* self, size_t key){
	if (self != NULL){
		int i = key%self->T;
		ALSNode* node = &(self->table[i]);
		switch (node->type){
			case E: // empty bucket, item not found
				return NULL;
				break;
			case L: // occupied by item that belongs here
				{
					ALSNode* probe;
					if (node->bucket.pair.key == key) // found right here
						return node->bucket.pair.value;
					else if ((probe = probeBuckets(self, key, i))!=NULL){ // probe bucket
						if (probe->type == B){
							return probe->bucket.pair.value;
						}
					}
					// not found
					return NULL;
				}
				break;
			case B: // occupied by item that doesn't belong here -> not found
				{
					return NULL;
				}
				break;
			case A: // search in auxiliary data structure
				{
					#ifdef USE_TREE
					#ifdef UNIQUE_AUX
					BinaryTree* aux = self->col;
					#else
					BinaryTree* aux = node->bucket.col;
					#endif
					return BinaryTree_Get(aux, key);
					#else
					#ifdef UNIQUE_AUX
					LinkedList* aux = self->col;
					#else
					LinkedList* aux = node->bucket.col;
					#endif
					return LinkedList_Get(aux, key);
					#endif
				}
				break;
		}
	}
	return NULL;
}

void* ALSHashMap_Del(ALSHashMap* self, size_t key){
	void* ret = NULL;
	if (self != NULL){
		int i = key%self->T;
		ALSNode* node = &(self->table[i]);
		switch (node->type){
			case E: // empty bucket, item not found
				break;
			case L: // occupied by item that belongs here
				{
					ALSNode* probe;
					if (node->bucket.pair.key == key){ // found right here
						node->type = E;
						ret = node->bucket.pair.value;
						reallocBuckets(self, i);
						self->N--;
						self->NE++;
					}
					else if ((probe = probeBuckets(self, key, i))!=NULL){ // probe bucket
						if (probe->type == B){
							probe->type = E;
							ret = probe->bucket.pair.value;
							self->N--;
							self->NE++;
						}
					}
				}
				break;
			case B: // occupied by item that doesn't belong here -> not found
				{
					return NULL;
				}
				break;
			case A: // search in auxiliary data structure
				{
					#ifdef USE_TREE
					#ifdef UNIQUE_AUX
					BinaryTree* aux = self->col;
					#else
					BinaryTree* aux = node->bucket.col;
					#endif
					ret = BinaryTree_Del(aux, key);
					#ifndef UNIQUE_AUX
					if (BinaryTree_Size(aux)==0){
						BinaryTree_Destroy(aux);
						self->NA--;
						node->type = E;
						self->NE++;
					}
					#endif
					#else
					#ifdef UNIQUE_AUX
					LinkedList* aux = self->col;
					#else
					LinkedList* aux = node->bucket.col;
					#endif
					ret = LinkedList_Del(aux, key);
					#ifndef UNIQUE_AUX
					if (LinkedList_Size(node->bucket.col)==0){
						LinkedList_Destroy(node->bucket.col);
						self->NA--;
						node->type = E;
						self->NE++;
					}
					#endif
					#endif
					if (ret != NULL) self->N--;
				}
				break;
		}
	}
	if ((float)(self->T-self->NE)/self->T < MIN_LF){
		redistributeAux(self);
		if ((float)(self->T-self->NE)/self->T < MIN_LF && self->T > INITIAL_SIZE){
			resizeTable(self, self->T/2);
		}
	}
	return ret;
}

void ALSHashMap_Destroy(ALSHashMap* self){
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	BinaryTree_Destroy(self->col);
	#else
	LinkedList_Destroy(self->col);
	#endif
	#else
	int i=0; for (; i<self->T; i++){
		if (self->table[i].type == A){
			#ifdef USE_TREE
			BinaryTree_Destroy(self->table[i].bucket.col);
			#else
			LinkedList_Destroy(self->table[i].bucket.col);
			#endif
		}
	}
	#endif
	free(self->table);
	free(self);
}

size_t ALSHashMap_Size(ALSHashMap* self){
	return self->N;
}

void ALSHashMap_PrintStats(ALSHashMap* self){
	int i;
	size_t SA=0, S=0, NL=0;
	printf("[");
	for (i=0; i<self->T; i++){
		switch (self->table[i].type){
			case E: printf(" "); break;
			case L: printf("L"); NL++; break;
			case B: printf("B"); break;
			case A:
				#ifdef UNIQUE_AUX
				printf("A"); break;
				#else
				#ifdef USE_TREE
				S = BinaryTree_Size(self->table[i].bucket.col);
				#else
				S = LinkedList_Size(self->table[i].bucket.col);
				#endif
				printf("%zu", S); SA+=S; break;
				#endif
		}
		if (i<self->T-1) printf(", ");
	}
	printf("]\n");
	printf("CRC: %zu\n", self->CRC);
	printf("MA: %zu\n", self->MA);
	printf("T: %zu\n", self->T);
	printf("R: %zu\n", self->R);
	printf("NE: %zu\n", self->NE);
	printf("NA: %zu -> %f\n", self->NA, (double)self->NA/self->T);
	printf("NL: %zu -> %f\n", NL, (double)NL/self->T);
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	printf("SA: %zu\n", BinaryTree_Size(self->col));
	#else
	printf("SA: %zu\n", LinkedList_Size(self->col));
	#endif
	#else
	printf("SA: %zu\n", SA);
	#endif
	printf("N: %zu\n", self->N);
	printf("Table Usage: %f\n", (double)(self->T-self->NE)/self->T);
	printf("Load Factor: %f\n", (double)self->N/self->T);
	#ifdef USE_TREE
	#ifdef USE_RED_BLACK
	printf("Using Red-Black Tree\n");
	#else
	printf("Using BinaryTree\n");
	#endif
	#else
	printf("Using LinkedList\n");
	#endif
	#ifdef EXTENDED_PROBING
	printf("Using Extended Probing\n");
	#else
	printf("Using Limited Probing\n");
	#endif
	#ifdef UNIQUE_AUX
	printf("Using unique auxiliary structure\n");
	#endif
}


// Private functions
#ifdef USE_TREE
static void redistributeTree(ALSHashMap* self, TreeNode* tnode){
	if (tnode != NULL){
		redistributeTree(self, tnode->children[0]);
		redistributeTree(self, tnode->children[1]);
		ALSHashMap_Set(self, tnode->key, tnode->value);
		free(tnode);
	}
}
#else
static void redistributeList(ALSHashMap* self, ListNode* node){
	while (node != NULL){
		ALSHashMap_Set(self, node->key, node->value);
		ListNode* aux = node;
		node = node->next;
		free(aux);
	}
}
#endif


// change the size of the table
static void resizeTable(ALSHashMap* self, size_t newsize){
	//printf("resize %zu -> %zu\n", self->T, newsize);
	int i;
	// backup table
	ALSNode* table = self->table;
	size_t T = self->T;
	
	// reset values and allocate new table
	if (newsize > T) self->R++;
	else self->R--;
	self->MA = 0;
	self->CRC = 0;
	self->NA = 0;
	self->NE = newsize;
	self->N = 0;
	self->T = newsize;
	self->table = (ALSNode*)malloc(sizeof(ALSNode)*newsize);
	for (i=0; i<newsize; i++)
		self->table[i].type = E;
	
	// iterate over old table and add to new table
	for (i=0; i<T; i++){
		if (table[i].type != E){
			if (table[i].type == A){
				#ifndef UNIQUE_AUX
				#ifdef USE_TREE
				redistributeTree(self, table[i].bucket.col->root);
				free(table[i].bucket.col);
				#else
				ListNode* node = table[i].bucket.col->start;
				redistributeList(self, node);
				free(table[i].bucket.col);
				#endif
				#endif
			}
			else{
				ALSHashMap_Set(self, table[i].bucket.pair.key, table[i].bucket.pair.value);
			}
		}
	}
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	BinaryTree* tree = self->col;
	self->col = BinaryTree_New();
	redistributeTree(self, tree->root);
	free(tree);
	#else
	LinkedList* list = self->col;
	self->col = LinkedList_New();
	redistributeList(self, list->start);
	free(list);
	#endif
	#endif
	free(table);
}

// attempt to redistribute auxiliary structures
static void redistributeAux(ALSHashMap* self){
	//printf("Redistribute aux\n");
	int i;
	self->MA = 0;
	#ifdef UNIQUE_AUX
	#ifdef USE_TREE
	BinaryTree* tree = self->col;
	self->col = BinaryTree_New();
	redistributeTree(self, tree->root);
	free(tree);
	#else
	LinkedList* list = self->col;
	self->col = LinkedList_New();
	redistributeList(self, list->start);
	free(list);
	#endif
	#else
	for (i=0; i<self->T; i++){
		if (self->table[i].type == A){
			self->table[i].type = E;
			self->NE++;
			self->NA--;
			#ifdef USE_TREE
			BinaryTree* tree = self->table[i].bucket.col;
			self->N -= BinaryTree_Size(tree);
			redistributeTree(self, tree->root);
			free(tree);
			#else
			LinkedList* list = self->table[i].bucket.col;
			self->N -= LinkedList_Size(list);
			redistributeList(self, list->start);
			free(list);
			#endif
		}
	}
	#endif
}

// move a node into its real position, returns 1 if can't allocate more
static int reallocNode(ALSHashMap* self, int i, ALSNode* node, ALSNode* into){
	if (node->type == B && node->bucket.pair.key%self->T == i){
		if (into->type == E){
			into->bucket.pair.key = node->bucket.pair.key;
			into->bucket.pair.value = node->bucket.pair.value;
			into->type = L;
			node->type = E;
			return 1;
		}
		else if (into->type == A){
			#ifdef USE_TREE
			#ifdef UNIQUE_AUX
			BinaryTree* tree = self->col;
			#else
			BinaryTree* tree = into->bucket.col;
			#endif
			BinaryTree_Set(tree, node->bucket.pair.key, node->bucket.pair.value);
			#else
			#ifdef UNIQUE_AUX
			LinkedList* list = self->col;
			#else
			LinkedList* list = into->bucket.col;
			#endif
			LinkedList_Set(list, node->bucket.pair.key, node->bucket.pair.value);
			#endif
			node->type = E;
			self->NE++;
		}
	}
	return 0;
}

// do a probe and reallocate buckets to their original position
static void reallocBuckets(ALSHashMap* self, int i){
	int p1 = i-1;
	int p2 = i+1;
	int p1bound = max(0, (int)i - (int)(self->R));
	int p2bound = min(self->T-1, i+self->R);
	ALSNode* into = &(self->table[i]);
	while (p1>=p1bound || p2<=p2bound){
		if (p1>=p1bound){
			ALSNode* node = &(self->table[p1]);
			if (reallocNode(self, i, node, into)){
				return;
			}
			p1--;
		}
		if (p2<=p2bound){
			ALSNode* node = &(self->table[p2]);
			if (reallocNode(self, i, node, into)){
				return;
			}
			p2++;
		}
	}
	#ifdef EXTENDED_PROBING
	int inc = (int)self->T / (int)self->R;
	while (p1>=0 || p2<self->T){
		if (p1>=0){
			ALSNode* node = &(self->table[p1]);
			if (reallocNode(self, i, node, into)){
				return;
			}
			p1-=inc;
		}
		if (p2<self->T){
			ALSNode* node = &(self->table[p2]);
			if (reallocNode(self, i, node, into)){
				return;
			}
			p2+=inc;
		}
	}
	#endif
}

// check if node is empty or contains key
// if it contains key and there is empty bucket closer to its real position, reallocate it
static int probeNode(ALSNode** node, ALSNode** empty, size_t key){
	ALSNode* e = *empty;
	ALSNode* n = *node;
	int ret = 0;
	if (e == NULL && n->type == E) e = n; // found first empty node
	else if (n->type == B && n->bucket.pair.key == key){ // found node containing key
		if (e != NULL){ // move node to empty bucket closer to its real position
			e->type = B;
			e->bucket.pair.key = n->bucket.pair.key;
			e->bucket.pair.value = n->bucket.pair.value;
			n->type = E;
			n = e;
		}
		ret = 1;
	}
	*node = n;
	*empty = e;
	return ret;
}

// probe for an empty bucket or bucket containing key
static ALSNode* probeBuckets(ALSHashMap* self, size_t key, int i){
	int p1 = i-1;
	int p2 = i+1;
	int p1bound = max(0, (int)i - (int)(self->R));
	int p2bound = min(self->T-1, i+self->R);
	ALSNode* empty = NULL;
	while (p1>=p1bound || p2<=p2bound){
		if (p1>=p1bound){
			ALSNode* node = &(self->table[p1]);
			if (probeNode(&node, &empty, key))
				return node;
			p1--;
		}
		if (p2<=p2bound){
			ALSNode* node = &(self->table[p2]);
			if (probeNode(&node, &empty, key))
				return node;
			p2++;
		}
	}
	#ifdef EXTENDED_PROBING
	int inc = (int)self->T / (int)self->R;
	while (p1>=0 || p2<self->T){
		if (p1>=0){
			ALSNode* node = &(self->table[p1]);
			if (probeNode(&node, &empty, key))
				return node;
			p1-=inc;
		}
		if (p2<self->T){
			ALSNode* node = &(self->table[p2]);
			if (probeNode(&node, &empty, key))
				return node;
			p2+=inc;
		}
	}
	#endif
	return empty;
}
