enum{
	RED,
	BLACK
};

void _RedBlackTree_onSetBalance(BinaryTree* self, TreeNode* inserted);
void _RedBlackTree_onDelBalance(BinaryTree* self, TreeNode* child);

void _RedBlackTree_rotate(BinaryTree* self, TreeNode* node);
void _RedBlackTree_swapParent(BinaryTree* self, TreeNode* inserted);

int _RedBlackTree_delCase2(BinaryTree* self, TreeNode* child);
int _RedBlackTree_delCase3(BinaryTree* self, TreeNode* child);
int _RedBlackTree_delCase4(BinaryTree* self, TreeNode* child);
int _RedBlackTree_delCase5(BinaryTree* self, TreeNode* child);
int _RedBlackTree_delCase6(BinaryTree* self, TreeNode* child);


void _RedBlackTree_onSetBalance(BinaryTree* self, TreeNode* inserted){
	while (inserted != self->root){
		TreeNode* parent = inserted->parent;
		if (parent->color == RED && inserted->color == RED){
			TreeNode* grandparent = parent->parent;
			int child = parent->key < grandparent->key? 1:0;
			TreeNode* uncle = grandparent->children[child];
			if (uncle != NULL && uncle->color == RED){
				// recolor
				uncle->color = BLACK;
				parent->color = BLACK;
				grandparent->color = RED;
			}
			else{
				if (inserted == parent->children[child]){ // inserted node is on same side as uncle
					_RedBlackTree_swapParent(self, inserted); // swap parent and child
					inserted = inserted->children[!child]; // rotation is now done on parent
				}
				// rotate
				parent = inserted->parent;
				grandparent = parent->parent;
				_RedBlackTree_rotate(self, inserted->parent);
				// recolor
				parent->color = BLACK;
				grandparent->color = RED;
			}
		}
		inserted = inserted->parent;
	}
	self->root->color = BLACK; // root must always be black
}

void _RedBlackTree_onDelBalance(BinaryTree* self, TreeNode* child){
	if (child == NULL) return;
	if (child!=NULL && child->color == RED){ // child is red
		child->color = BLACK; // recolor
		return;
	}
	else if (child == self->root){ // case 1: new root, nothing to do
		return;
	}
	else{
		_RedBlackTree_delCase2(self, child);
		if (!_RedBlackTree_delCase3(self, child))
			if (!_RedBlackTree_delCase4(self, child))
				if (!_RedBlackTree_delCase5(self, child));
					_RedBlackTree_delCase6(self, child);
	}
}

void _RedBlackTree_rotate(BinaryTree* self, TreeNode* node){
	TreeNode* parent = node->parent;
	TreeNode* grandparent = parent->parent;
	int child = node->key > parent->key? 1:0;
	TreeNode* aux = node->children[!child];
	parent->children[child] = aux;
	if (aux!=NULL) aux->parent = parent;
	node->children[!child] = parent;
	parent->parent = node;
	node->parent = grandparent;
	if (grandparent == NULL) self->root = node;
	else{
		if (grandparent->children[0] == parent) grandparent->children[0] = node;
		else grandparent->children[1] = node;
	}
}

void _RedBlackTree_swapParent(BinaryTree* self, TreeNode* inserted){
	TreeNode* parent = inserted->parent;
	TreeNode* grandparent = parent->parent;
	int child = inserted->key < parent->key? 1:0;
	TreeNode* aux = inserted->children[child];
	grandparent->children[child] = inserted;
	parent->children[!child] = aux;
	inserted->children[child] = parent;
	parent->parent = inserted;
	inserted->parent = grandparent;
	if (aux!=NULL) aux->parent = parent;
}

int _RedBlackTree_delCase2(BinaryTree* self, TreeNode* child){
	TreeNode* parent = child->parent;
	if (parent != NULL && parent->color == BLACK){
		TreeNode* S = (parent->children[0]==child)? parent->children[1] : parent->children[0];
		if (S->color == RED){
			parent->color = RED;
			S->color = BLACK;
			_RedBlackTree_rotate(self, S);
			return 1;
		}
	}
	return 0;
}

int _RedBlackTree_delCase3(BinaryTree* self, TreeNode* child){
	TreeNode* parent = child->parent;
	if (parent != NULL && parent->color == BLACK){
		TreeNode* S = (parent->children[0]==child)? parent->children[1] : parent->children[0];
		if (S->color == BLACK){
			S->color = RED;
			_RedBlackTree_onDelBalance(self, parent);
			return 1;
		}
	}
	return 0;
}

int _RedBlackTree_delCase4(BinaryTree* self, TreeNode* child){
	TreeNode* parent = child->parent;
	if (parent != NULL && parent->color == RED){
		TreeNode* S = (parent->children[0]==child)? parent->children[1] : parent->children[0];
		if (S->color == BLACK &&
				(S->children[0] == NULL || S->children[0]->color == BLACK) &&
				(S->children[1] == NULL || S->children[1]->color == BLACK)){
			parent->color = BLACK;
			S->color = RED;
			return 1;
		}
	}
	return 0;
}

int _RedBlackTree_delCase5(BinaryTree* self, TreeNode* child){
	TreeNode* parent = child->parent;
	if (parent != NULL && parent->color == BLACK){
		int c = child->key > parent->key? 1:0;
		TreeNode* S = parent->children[!c];
		if (S->color == BLACK &&
				(S->children[c] == NULL || S->children[c]->color == BLACK) &&
				(S->children[!c] != NULL && S->children[!c]->color == RED)){
			S->children[!c]->color = BLACK;
			S->color = RED;
			_RedBlackTree_rotate(self, S->children[!c]);
			return 1;
		}
	}
	return 0;
}

int _RedBlackTree_delCase6(BinaryTree* self, TreeNode* child){
	TreeNode* parent = child->parent;
	if (parent != NULL){
		int c = child->key > parent->key? 1:0;
		TreeNode* S = parent->children[!c];
		if (S->color == BLACK &&
				(S->children[c] != NULL || S->children[c]->color == RED)){
			S->color = parent->color;
			parent->color = BLACK;
			S->children[c]->color = BLACK;
			_RedBlackTree_rotate(self, S);
			return 1;
		}
	}
	return 0;
}
