#include "bst.h"

#ifdef USE_RED_BLACK
#include "rbt.c"
#endif

BinaryTree* BinaryTree_New(){
	BinaryTree* self = (BinaryTree*)malloc(sizeof(BinaryTree));
	self->root = NULL;
	self->size = 0;
	return self;
}

TreeNode* _BinaryTree_Set(BinaryTree* self, size_t key, void* value){
	TreeNode* tnode = self->root;
	if (tnode==NULL){
		self->size++;
		tnode = (TreeNode*)malloc(sizeof(TreeNode));
		tnode->key = key;
		tnode->value = value;
		tnode->children[0] = tnode->children[1] = NULL;
		tnode->parent = NULL;
		#ifdef USE_RED_BLACK
		tnode->color = BLACK;
		#endif
		self->root = tnode;
		return tnode;
	}
	else{
		for (;;){
			if (tnode->key == key){
				tnode->value = value;
				break;
			}
			else{
				int c = tnode->key < key? 1:0;
				if (tnode->children[c] == NULL){
					self->size++;
					TreeNode* child = (TreeNode*)malloc(sizeof(TreeNode));
					tnode->children[c] = child;
					child->parent = tnode;
					child->key = key;
					child->value = value;
					child->children[0] = NULL;
					child->children[1] = NULL;
					#ifdef USE_RED_BLACK
					child->color = RED;
					#endif
					return child;
					
				}
				else tnode = tnode->children[c];
			}
		}
	}
	return NULL;
}

void BinaryTree_Set(BinaryTree* self, size_t key, void* value){
	TreeNode* inserted = _BinaryTree_Set(self, key, value);
	#ifdef USE_RED_BLACK
	if (inserted!=NULL) _RedBlackTree_onSetBalance(self, inserted);
	#endif
}

void* BinaryTree_Get(BinaryTree* self, size_t key){
	TreeNode* tnode = self->root;
	while (tnode != NULL){
		if (tnode->key == key){return tnode->value;}
		else{
			int child = tnode->key < key? 1:0;
			tnode = tnode->children[child];
		}
	}
	return NULL;
}

void* BinaryTree_Del(BinaryTree* self, size_t key){
	TreeNode* tnode = self->root;
	int pchild;
	while (tnode != NULL){
		if (tnode->key == key){
			void* ret = tnode->value;
			TreeNode* aux = tnode;
			TreeNode* child = NULL;
			if (tnode->children[0] == NULL || tnode->children[1] == NULL){ // at most one child
				if ((child=tnode->children[0])==NULL) child = tnode->children[1];
				if (child != NULL) child->parent = tnode->parent;
				if (tnode->parent != NULL){ // is not root
					int c = (tnode->key>tnode->parent->key)? 1:0;
					tnode->parent->children[c] = child;
				}
				else self->root = child;
			}
			else{ // two children
				aux = tnode->children[1];
				while (aux->children[0] != NULL){
					aux = aux->children[0];
				}
				child = aux->children[1];
				tnode->value = aux->value;
				tnode->key = aux->key;
				if (aux->parent != tnode){
					aux->parent->children[0] = child;
					if (child!=NULL) child->parent = aux->parent;
				}
				else{
					tnode->children[1] = child;
					if (child!=NULL) child->parent = tnode;
				}
			}
			self->size--;
			#ifdef USE_RED_BLACK
			if (aux->color == BLACK){
				_RedBlackTree_onDelBalance(self, child);
			}
			#endif
			free(aux);
			return ret;
		}
		else{
			pchild = tnode->key < key? 1:0;
			tnode = tnode->children[pchild];
		}
	}
	return NULL;
}

size_t BinaryTree_Size(BinaryTree* self){
	return self->size;
}

static void BinaryTree_DestroyNode(TreeNode* tnode){
	if (tnode != NULL){
		BinaryTree_DestroyNode(tnode->children[0]);
		BinaryTree_DestroyNode(tnode->children[1]);
		free(tnode);
	}
}

void BinaryTree_Destroy(BinaryTree* self){
	BinaryTree_DestroyNode(self->root);
	free(self);
}

static void printTnode(TreeNode* tnode){
	if (tnode == NULL) printf("(nil)");
	else{
		printf("(");
		#ifdef USE_RED_BLACK
		printf("%c: ", (tnode->color == RED)? 'R':'B');
		#endif
		printf("<%zu,%p>, ", tnode->key, tnode->value);
		printTnode(tnode->children[0]);
		printf(", ");
		printTnode(tnode->children[1]);
		printf(")");
	}
}

void BinaryTree_Print(BinaryTree* self){
	printTnode(self->root);
}
