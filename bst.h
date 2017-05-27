#ifndef BST_H
#define BST_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdint.h>

typedef struct TreeNode{
	size_t key;
	void* value;
	struct TreeNode* parent;
	struct TreeNode* children[2];
	#ifdef USE_RED_BLACK
	int color;
	#endif
}TreeNode;

typedef struct BinaryTree{
	TreeNode* root;
	size_t size;
}BinaryTree;

BinaryTree* BinaryTree_New();
void BinaryTree_Set(BinaryTree*, size_t key, void* value);
void* BinaryTree_Get(BinaryTree*, size_t key);
void* BinaryTree_Del(BinaryTree*, size_t key);
size_t BinaryTree_Size(BinaryTree*);
void BinaryTree_Destroy(BinaryTree*);
void BinaryTree_Print(BinaryTree*);

#ifdef __cplusplus
}
#endif

#endif
