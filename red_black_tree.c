#include "red_black_tree.h"

/***********************************************************************/
/*  FUNCTION:  RBTreeCreate */
/**/
/*  INPUTS:  All the inputs are names of functions.  CompFunc takes to */
/*  void pointers to keys and returns 1 if the first arguement is */
/*  "greater than" the second.   DestFunc takes a pointer to a key and */
/*  destroys it in the appropriate manner when the node containing that */
/*  key is deleted.  InfoDestFunc is similiar to DestFunc except it */
/*  recieves a pointer to the info of a node and destroys it. */
/*  PrintFunc recieves a pointer to the key of a node and prints it. */
/*  PrintInfo recieves a pointer to the info of a node and prints it. */
/*  If RBTreePrint is never called the print functions don't have to be */
/*  defined and NullFunction can be used.  */
/**/
/*  OUTPUT:  This function returns a pointer to the newly created */
/*  red-black tree. */
/**/
/*  Modifies Input: none */
/***********************************************************************/

rb_red_blk_tree* RBTreeCreate( int (*CompFunc) (const void*,const void*),
			      void (*DestFunc) (void*),
			      void (*InfoDestFunc) (void*),
			      void (*PrintFunc) (const void*),
			      void (*PrintInfo)(void*),
                     double (*DistFunc)(const void*, const void*),
                     void* dfparam) {
  rb_red_blk_tree* newTree;
  rb_red_blk_node* temp;

  newTree=(rb_red_blk_tree*) SafeMalloc(sizeof(rb_red_blk_tree));
  newTree->Compare=  CompFunc;
  newTree->DestroyKey= DestFunc;
  newTree->PrintKey= PrintFunc;
  newTree->PrintInfo= PrintInfo;
  newTree->DestroyInfo= InfoDestFunc;
  newTree->DistFunc = DistFunc;
  newTree->dfparam = dfparam;

  /*  see the comment in the rb_red_blk_tree structure in red_black_tree.h */
  /*  for information on nil and root */
  temp=newTree->nil= (rb_red_blk_node*) SafeMalloc(sizeof(rb_red_blk_node));
  temp->parent=temp->left=temp->right=temp;
  temp->red=0;
  temp->key=0;
  temp->children = 0.0;
  temp=newTree->root= (rb_red_blk_node*) SafeMalloc(sizeof(rb_red_blk_node));
  temp->parent=temp->left=temp->right=newTree->nil;
  temp->key=0;
  temp->red=0;
  temp->children = 0.0;
  return(newTree);
}

/***********************************************************************
 * update the sum for a subtree (convenience function)
 ***********************************************************************/
static inline void TreeUpdateSum(rb_red_blk_tree* tree, rb_red_blk_node* x) {
     x->children = x->left->children + x->right->children + tree->DistFunc(x->key,tree->dfparam);
}

/***********************************************************************/
/*  FUNCTION:  LeftRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input: tree, x */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/

void LeftRotate(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* y;
  rb_red_blk_node* nil=tree->nil;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls LeftRotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when RBDeleteFixUP */
  /*  calls LeftRotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  y=x->right;
  x->right=y->left;

  if (y->left != nil) y->left->parent=x; /* used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */
  
  y->parent=x->parent;   

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  if( x == x->parent->left) {
    x->parent->left=y;
  } else {
    x->parent->right=y;
  }
  y->left=x;
  x->parent=y;

/************************************
 * TODO
 * nem változnak: x->parent, x->left és lefelé, y->left és lefelé, y->right és lefelé
 * változnak: y, y->children = x->children; x, x->children = x->left->children + x->right->children + 1
 * megjegyzés: a nil node-ra a children értéke 0 kell legyen
 * megjegyzés2: a children-be az adott node is bele kell számítson, így lesz különbség a nil és
 *   a leaf node-ok között
 ************************************/
 TreeUpdateSum(tree,x); /* first we need to update x */
 TreeUpdateSum(tree,y); /* y->left == x, we use the result of the last calculation here */

#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not red in LeftRotate");
  Assert((tree->nil->children == 0.0),"nil->children != 0 in LeftRotate!\n");
  Assert((tree->root->children == 0.0),"root->children != 0 in LeftRotate!\n");
#endif
}


/***********************************************************************/
/*  FUNCTION:  RighttRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input?: tree, y */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/

void RightRotate(rb_red_blk_tree* tree, rb_red_blk_node* y) {
  rb_red_blk_node* x;
  rb_red_blk_node* nil=tree->nil;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls LeftRotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when RBDeleteFixUP */
  /*  calls LeftRotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  x=y->left;
  y->left=x->right;

  if (nil != x->right)  x->right->parent=y; /*used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  x->parent=y->parent;
  if( y == y->parent->left) {
    y->parent->left=x;
  } else {
    y->parent->right=x;
  }
  x->right=y;
  y->parent=x;

/************************************
 * TODO
 * nem változnak: y->parent, x->left és gyerekei, x->right és gyerekei, y->right és gyerekei
 * változnak: x,y; x->children = y->children, y->children = y->left->children + y->right->childre + 1
 ************************************/
 TreeUpdateSum(tree,y);
 TreeUpdateSum(tree,x);

#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not red in RightRotate");
  Assert((tree->nil->children == 0.0),"nil->children != 0 in RightRotate!\n");
  Assert((tree->root->children == 0.0),"root->children != 0 in RightRotate!\n");
#endif
}

/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
/**/
/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
/**/
/*  OUTPUT:  none */
/**/
/*  Modifies Input:  tree, z */
/**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the RBTreeInsert function and not by the user */
/***********************************************************************/

void TreeInsertHelp(rb_red_blk_tree* tree, rb_red_blk_node* z) {
  /*  This function should only be called by InsertRBTree (see above) */
  rb_red_blk_node* x;
  rb_red_blk_node* y;
  rb_red_blk_node* nil=tree->nil;
  rb_red_blk_node* root = tree->root;
  
  z->left=z->right=nil;
  y=tree->root;
  x=tree->root->left;
  while( x != nil) {
    y=x;
    if (1 == tree->Compare(x->key,z->key)) { /* x.key > z.key */
      x=x->left;
    } else { /* x,key <= z.key */
      x=x->right;
    }
  }
  z->parent=y;
  if ( (y == tree->root) ||
       (1 == tree->Compare(y->key,z->key))) { /* y.key > z.key */
    y->left=z;
  } else {
    y->right=z;
  }

/************************************
 TODO: beillesztett node: z, z->children = DistFunc(z),
 * ezt felfele rekurzívan hozzá kell adni minden node-hoz
*************************************/
  z->children = tree->DistFunc(z->key,tree->dfparam);
  {
       rb_red_blk_node* w = z->parent;
       while(w != root) {
            w->children += z->children;
            w = w->parent;
       }
  }

#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not red in TreeInsertHelp");
  Assert((tree->nil->children == 0.0),"nil->children != 0 in TreeInsertHelp!\n");
  Assert((tree->root->children == 0.0),"root->children != 0 in TreeInsertHelp!\n");
#endif
}

/*  Before calling Insert RBTree the node x should have its key set */

/***********************************************************************/
/*  FUNCTION:  RBTreeInsert */
/**/
/*  INPUTS:  tree is the red-black tree to insert a node which has a key */
/*           pointed to by key and info pointed to by info.  */
/**/
/*  OUTPUT:  This function returns a pointer to the newly inserted node */
/*           which is guarunteed to be valid until this node is deleted. */
/*           What this means is if another data structure stores this */
/*           pointer then the tree does not need to be searched when this */
/*           is to be deleted. */
/**/
/*  Modifies Input: tree */
/**/
/*  EFFECTS:  Creates a node node which contains the appropriate key and */
/*            info pointers and inserts it into the tree. */
/***********************************************************************/

rb_red_blk_node * RBTreeInsert(rb_red_blk_tree* tree, void* key, void* info) {
  rb_red_blk_node * y;
  rb_red_blk_node * x;
  rb_red_blk_node * newNode;

  x=(rb_red_blk_node*) SafeMalloc(sizeof(rb_red_blk_node));
  x->key=key;
  x->info=info;

  TreeInsertHelp(tree,x);
  newNode=x;
  x->red=1;
  
  /******************************************************************
   * TODO: itt nem változnak a viszonyok (ha jól látom), minden
   * változtatás a *Rotate fv.-ekben történik, azokban a children
   * értékek frissítése
   ******************************************************************/
  while(x->parent->red) { /* use sentinel instead of checking for root */
    if (x->parent == x->parent->parent->left) {
      y=x->parent->parent->right;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->right) {
	  x=x->parent;
	  LeftRotate(tree,x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	RightRotate(tree,x->parent->parent);
      } 
    } else { /* case for x->parent == x->parent->parent->right */
      y=x->parent->parent->left;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->left) {
	  x=x->parent;
	  RightRotate(tree,x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	LeftRotate(tree,x->parent->parent);
      } 
    }
  }
  tree->root->left->red=0;
  return(newNode);

#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not red in RBTreeInsert");
  Assert(!tree->root->red,"root not red in RBTreeInsert");
#endif
}


/***********************************************************************/
/*  FUNCTION:  GetNodeRank  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the rank of. */
/**/
/*    OUTPUT:  This function returns the rank of x. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:   */
/***********************************************************************/
  
double GetNodeRank(rb_red_blk_tree* tree,rb_red_blk_node* x) {
     rb_red_blk_node* nil = tree->nil;
     rb_red_blk_node* root = tree->root;
     double ret = 0.0;
     
#ifdef DEBUG_ASSERT
     Assert((x!=nil),"x == nil in GetNodeRank!\n");
     Assert((x!=root),"x == root in GetNodeRank!\n");
#endif
     ret = x->left->children; //x is at least this
     rb_red_blk_node* w = x;
     while(w->parent != root) {
          if(w == w->parent->right) ret += w->parent->left->children + tree->DistFunc(w->parent->key,tree->dfparam);
          w = w->parent;
     }
     return ret;
}


/***********************************************************************/
/*  FUNCTION:  TreeSuccessor  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the successor of. */
/**/
/*    OUTPUT:  This function returns the successor of x or NULL if no */
/*             successor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/
  
rb_red_blk_node* TreeSuccessor(rb_red_blk_tree* tree,rb_red_blk_node* x) { 
  rb_red_blk_node* y;
  rb_red_blk_node* nil=tree->nil;
  rb_red_blk_node* root=tree->root;

  if (nil != (y = x->right)) { /* assignment to y is intentional */
    while(y->left != nil) { /* returns the minium of the right subtree of x */
      y=y->left;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->right) { /* sentinel used instead of checking for nil */
      x=y;
      y=y->parent;
    }
    if (y == root) return(nil);
    return(y);
  }
}

/***********************************************************************/
/*  FUNCTION:  TreePredecessor  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the predecessor of. */
/**/
/*    OUTPUT:  This function returns the predecessor of x or NULL if no */
/*             predecessor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/

rb_red_blk_node* TreePredecessor(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* y;
  rb_red_blk_node* nil=tree->nil;
  rb_red_blk_node* root=tree->root;

  if (nil != (y = x->left)) { /* assignment to y is intentional */
    while(y->right != nil) { /* returns the maximum of the left subtree of x */
      y=y->right;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->left) { 
      if (y == root) return(nil); 
      x=y;
      y=y->parent;
    }
    return(y);
  }
}


/***********************************************************************/
/*  FUNCTION:  TreeFirst  */
/**/
/*    INPUTS:  tree is the tree in question */
/**/
/*    OUTPUT:  The first node in the tree (i.e. a node for which is
 *        smaller than or equal to any other node, and can be used as
 *        a start for iterating over the whole tree in increasing
 *        oder with TreeSuccessor). */
/**/
/*    Modifies Input: none */
/***********************************************************************/

rb_red_blk_node* TreeFirst(rb_red_blk_tree* tree) {
     rb_red_blk_node* x = tree->root;
     rb_red_blk_node* nil = tree->nil;
     if(x->left == nil) return nil;
     while(x->left != nil) x = x->left;
     return x;
}


/***********************************************************************/
/*  FUNCTION:  TreeLast  */
/**/
/*    INPUTS:  tree is the tree in question */
/**/
/*    OUTPUT:  The last node in the tree (i.e. a node for which is
 *        greater than or equal to any other node, and can be used as
 *        a start for iterating over the whole tree in decreasing
 *        order with TreePrecedessor). */
/**/
/*    Modifies Input: none */
/***********************************************************************/

rb_red_blk_node* TreeLast(rb_red_blk_tree* tree) {
     rb_red_blk_node* x = tree->root;
     rb_red_blk_node* nil = tree->nil;
     if(x->left == nil) return nil;
     x = x->left;
     while(x->right != nil) x = x->right;
     return x;
}



/***********************************************************************/
/*  FUNCTION:  InorderTreePrint */
/**/
/*    INPUTS:  tree is the tree to print and x is the current inorder node */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively prints the nodes of the tree */
/*              inorder using the PrintKey and PrintInfo functions. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:    This function should only be called from RBTreePrint */
/***********************************************************************/

void InorderTreePrint(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* nil=tree->nil;
  rb_red_blk_node* root=tree->root;
  if (x != tree->nil) {
    InorderTreePrint(tree,x->left);
    printf("rank=%g  ",GetNodeRank(tree,x)); /** print the rank of node also **/
    printf("info=");
    tree->PrintInfo(x->info);
    printf("  key="); 
    tree->PrintKey(x->key);
    printf("  l->key=");
    if( x->left == nil) printf("NULL"); else tree->PrintKey(x->left->key);
    printf("  r->key=");
    if( x->right == nil) printf("NULL"); else tree->PrintKey(x->right->key);
    printf("  p->key=");
    if( x->parent == root) printf("NULL"); else tree->PrintKey(x->parent->key);
    printf("  red=%i\n",x->red);
    InorderTreePrint(tree,x->right);
  }
}


/***********************************************************************/
/*  FUNCTION:  TreeDestHelper */
/**/
/*    INPUTS:  tree is the tree to destroy and x is the current node */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively destroys the nodes of the tree */
/*              postorder using the DestroyKey and DestroyInfo functions. */
/**/
/*    Modifies Input: tree, x */
/**/
/*    Note:    This function should only be called by RBTreeDestroy */
/***********************************************************************/

void TreeDestHelper(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* nil=tree->nil;
  if (x != nil) {
    TreeDestHelper(tree,x->left);
    TreeDestHelper(tree,x->right);
    tree->DestroyKey(x->key);
    tree->DestroyInfo(x->info);
    free(x);
  }
}


/***********************************************************************/
/*  FUNCTION:  RBTreeDestroy */
/**/
/*    INPUTS:  tree is the tree to destroy */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Destroys the key and frees memory */
/**/
/*    Modifies Input: tree */
/**/
/***********************************************************************/

void RBTreeDestroy(rb_red_blk_tree* tree) {
  TreeDestHelper(tree,tree->root->left);
  free(tree->root);
  free(tree->nil);
  free(tree);
}


/***********************************************************************/
/*  FUNCTION:  RBTreePrint */
/**/
/*    INPUTS:  tree is the tree to print */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  This function recursively prints the nodes of the tree */
/*             inorder using the PrintKey and PrintInfo functions. */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/

void RBTreePrint(rb_red_blk_tree* tree) {
  InorderTreePrint(tree,tree->root->left);
}


/***********************************************************************/
/*  FUNCTION:  RBExactQuery */
/**/
/*    INPUTS:  tree is the tree to print and q is a pointer to the key */
/*             we are searching for */
/**/
/*    OUTPUT:  returns the a node with key equal to q.  If there are */
/*             multiple nodes with key equal to q this function returns */
/*             the one highest in the tree */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/
  
rb_red_blk_node* RBExactQuery(const rb_red_blk_tree* tree, const void* q) {
  rb_red_blk_node* x=tree->root->left;
  rb_red_blk_node* nil=tree->nil;
  int compVal;
  if (x == nil) return(0);
  compVal=tree->Compare(x->key,/*(int*)*/ q);
  while(0 != compVal) {/*assignemnt*/
    if (1 == compVal) { /* x->key > q */
      x=x->left;
    } else {
      x=x->right;
    }
    if ( x == nil) return(0);
    compVal=tree->Compare(x->key,/*(int*)*/ q);
  }
  return(x);
}


/***********************************************************************/
/*  FUNCTION:  RBDeleteFixUp */
/**/
/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
/*             out node in RBTreeDelete. */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
/**/
/*    Modifies Input: tree, x */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/

void RBDeleteFixUp(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* root=tree->root->left;
  rb_red_blk_node* w;

  while( (!x->red) && (root != x)) {
    if (x == x->parent->left) {
      w=x->parent->right;
      if (w->red) {
	w->red=0;
	x->parent->red=1;
	LeftRotate(tree,x->parent);
	w=x->parent->right;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red=1;
	x=x->parent;
      } else {
	if (!w->right->red) {
	  w->left->red=0;
	  w->red=1;
	  RightRotate(tree,w);
	  w=x->parent->right;
	}
	w->red=x->parent->red;
	x->parent->red=0;
	w->right->red=0;
	LeftRotate(tree,x->parent);
	x=root; /* this is to exit while loop */
      }
    } else { /* the code below is has left and right switched from above */
      w=x->parent->left;
      if (w->red) {
	w->red=0;
	x->parent->red=1;
	RightRotate(tree,x->parent);
	w=x->parent->left;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red=1;
	x=x->parent;
      } else {
	if (!w->left->red) {
	  w->right->red=0;
	  w->red=1;
	  LeftRotate(tree,w);
	  w=x->parent->left;
	}
	w->red=x->parent->red;
	x->parent->red=0;
	w->left->red=0;
	RightRotate(tree,x->parent);
	x=root; /* this is to exit while loop */
      }
    }
  }
  x->red=0;
  
/****************************************
 * TODO: itt nem változik a fa, csak a
 * *Rotate függvényeket hívja meg,
 * így itt nem kell semmit sem tenni
 ****************************************/

#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not black in RBDeleteFixUp");
#endif
}


/***********************************************************************/
/*  FUNCTION:  RBDelete */
/**/
/*    INPUTS:  tree is the tree to delete node z from */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Deletes z from tree and frees the key and info of z */
/*             using DestoryKey and DestoryInfo.  Then calls */
/*             RBDeleteFixUp to restore red-black properties */
/**/
/*    Modifies Input: tree, z */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/

void RBDelete(rb_red_blk_tree* tree, rb_red_blk_node* z){
  rb_red_blk_node* y;
  rb_red_blk_node* x;
  rb_red_blk_node* nil=tree->nil;
  rb_red_blk_node* root=tree->root;
  double ydval = 0.0; /* DistFunc(y) */
  double zdval = 0.0; /* DistFunc(x) */

  /*y= ((z->left == nil) || (z->right == nil)) ? z : TreeSuccessor(tree,z);*/
  if((z->left == nil) || (z->right == nil)) y = z; /** így átláthatóbb **/
  else y =  TreeSuccessor(tree,z);
  /*x= (y->left == nil) ? y->right : y->left;*/
  
  /*********************************
   * y-t töröljük, x-et betesszük y helyére
   * y-nak maximum egy gyereke lehet, ez x (lehet x == nil is)
   * két lehetőség: y == z, ekkor x bármelyik gyerek lehet
   * y != z, ekkor viszont x == y->right (y->left == nil, ha nem így lenne,
   *      akkor a Successor keresésnél arra mentünk volna tovább)
   *********************************/
  if(y->left == nil) x = y->right; /** így átláthatóbb **/
  else x = y->left;
  
  /*
   * In this case, updating the sums is a bit more complicated
   * 
   * First we delete the node y, and recursively
   * substract its value from the nodes above it
   * Next we replace y with x. The stored value of
   * x does not change, its the only child of y.
   */
  
  /** decrease the computed values of children for each node going upwards from y **/
  ydval = tree->DistFunc(y->key,tree->dfparam);
  {
       rb_red_blk_node* w = y->parent;
       
       while(w != root) {
            w->children -= ydval;
            w = w->parent;
       }
  }
  
  if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
    root->left=x;
    /*
     * megjegyzés: root nem az "igazi" root node, hanem egy "null" node, aminek a bal gyereke az igazi root,
     * tehát ebben az esetben y az "igazi" root node
     */
  } else {
    if (y == y->parent->left) {
      y->parent->left=x;
    } else {
      y->parent->right=x;
    }
  }
  /*********************************
   * TODO: 
   *      x-et betettük y helyére, x->children-t kell frissíteni
   * két lépésben lehet: először az eredeti x-ből indulva, felfelé haladva minden node
   * children értékét egyel csökkentjük (ebben benne lesz y is), majd x->children = y->children
   *********************************/
 /*  if(x != nil) x->children = y->children; //!! we put x in the place of y */
   
  if (y != z) { /* y should not be nil in this case */
     /********************************************
      * TODO:
      * ebben az esetben y-t tesszük be z helyére
      * a children értékeket már csökkentettük (x-ből indulva, z-t is beleértve),
      * y->children = z->children
      ********************************************/
      zdval = tree->DistFunc(z->key,tree->dfparam);
#ifdef DEBUG_ASSERT
    Assert( (y!=tree->nil),"y is nil in RBDelete\n");
#endif
    /* y is the node to splice out and x is its child */

     /***************
      * probléma (?): itt egy "félkész" fára futtatjuk a FixUp-ot
      * mégsem probléma: y-t már teljesen kivágtuk, x van a helyén, és a children értékek is stimmelnek
      ***************/
    if (!(y->red)) RBDeleteFixUp(tree,x);
  
    tree->DestroyKey(z->key);
    tree->DestroyInfo(z->info);
    y->left=z->left;
    y->right=z->right;
    y->parent=z->parent;
    y->red=z->red;
    y->children = y->left->children + y->right->children + ydval;
    z->left->parent=z->right->parent=y;
    if (z == z->parent->left) {
      z->parent->left=y; 
    } else {
      z->parent->right=y;
    }
    free(z);
    
    /** update the children values going upwards from y **/
    {
         rb_red_blk_node* w = y->parent;
         double diff = ydval - zdval;
         while(w != root) {
            w->children += diff;
            w = w->parent;
       }
    }
  } else {
       /***************************************
        * TODO
        * itt már semmit sem kell csinálni, 
        ***************************************/
    tree->DestroyKey(y->key);
    tree->DestroyInfo(y->info);
    if (!(y->red)) RBDeleteFixUp(tree,x);
    free(y);
  }
  
#ifdef DEBUG_ASSERT
  Assert(!tree->nil->red,"nil not black in RBDelete");
#endif
}


    
  
  



