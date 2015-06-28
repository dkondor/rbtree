#ifndef RBTREE_H
#define RBTREE_H

#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "misc.h"
#include "stack.h"

/**************************************************
 * changes: 2013-06-25 by Dániel Kondor, kondor.dani@gmail.com
 * add the possibility to quickly (O(log(n))) compute the
 * rank of each node
 * a node's children is the size of the subtree starting from that node
 * (including the given node)
 * 
 * further changes: instead of giving the node rank, compute the CDF
 * (cumulative distribution function) of a function depending on the key
 * values, e.g.: f(x) = x^b
 * CDF(x) =  \frac{ \sum_{y < x} f(y) }{ \sum_y f(y) }
 **************************************************/


/*  CONVENTIONS:  All data structures for red-black trees have the prefix */
/*                "rb_" to prevent name conflicts. */
/*                                                                      */
/*                Function names: Each word in a function name begins with */
/*                a capital letter.  An example funcntion name is  */
/*                CreateRedTree(a,b,c). Furthermore, each function name */
/*                should begin with a capital letter to easily distinguish */
/*                them from variables. */
/*                                                                     */
/*                Variable names: Each word in a variable name begins with */
/*                a capital letter EXCEPT the first letter of the variable */
/*                name.  For example, int newLongInt.  Global variables have */
/*                names beginning with "g".  An example of a global */
/*                variable name is gNewtonsConstant. */

/* comment out the line below to remove all the debugging assertion */
/* checks from the compiled code.  */
/* #define DEBUG_ASSERT 1 */

/********************************
 * functions for int64_t keys
 * works only on 64-bit machines, where sizeof(int64_t) == sizeof(void*)
 ********************************/
#include <stdint.h>
#include <math.h>
static int CmpInt64(const void* a, const void* b) {
     int64_t i = (int64_t)a;
     int64_t j = (int64_t)b;
     if(i > j) return 1;
     if(i < j) return -1;
     return 0;
}

static double DFInt64(const void* a, const void* b) {
     double a1 = *(double*)b;
     int64_t val = (int64_t)a;
     double v2 = (double)val;
     return pow(v2,a1);
}


/*******************
 * node definition *
 *******************/
typedef struct rb_red_blk_node {
  void* key;
  void* info;
  int red; /* if red=0 then the node is black */
  struct rb_red_blk_node* left;
  struct rb_red_blk_node* right;
  struct rb_red_blk_node* parent;
  double children; /** sum of DistFunc(key) from this subtree, including this node -- 0.0 for nil and root **/
} rb_red_blk_node;


/* Compare(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
typedef struct rb_red_blk_tree {
  int (*Compare)(const void* a, const void* b); 
  void (*DestroyKey)(void* a);
  void (*DestroyInfo)(void* a);
  void (*PrintKey)(const void* a);
  void (*PrintInfo)(void* a);
  double (*DistFunc)(const void* a, const void* par);
  void* dfparam; /* this is passed to the DistFunc function */
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  rb_red_blk_node* root;             
  rb_red_blk_node* nil; 
} rb_red_blk_tree;

rb_red_blk_tree* RBTreeCreate(int  (*CompFunc)(const void*, const void*),
			     void (*DestFunc)(void*), 
			     void (*InfoDestFunc)(void*), 
			     void (*PrintFunc)(const void*),
			     void (*PrintInfo)(void*),
			     double (*DistFunc)(const void*, const void*),
			     void* dfparam);
rb_red_blk_node * RBTreeInsert(rb_red_blk_tree*, void* key, void* info);
void RBTreePrint(rb_red_blk_tree*);
void RBDelete(rb_red_blk_tree* , rb_red_blk_node* );
void RBTreeDestroy(rb_red_blk_tree*);
rb_red_blk_node* TreePredecessor(rb_red_blk_tree*,rb_red_blk_node*);
rb_red_blk_node* TreeSuccessor(rb_red_blk_tree*,rb_red_blk_node*);
rb_red_blk_node* TreeFirst(rb_red_blk_tree*); //!! get the first node (can be used to start an iteration over the tree nodes)
rb_red_blk_node* TreeLast(rb_red_blk_tree*); //!! get the last node
rb_red_blk_node* RBExactQuery(const rb_red_blk_tree*, const void*);
stk_stack * RBEnumerate(rb_red_blk_tree* tree,void* low, void* high);
void NullFunction(const void*);
double GetNodeRank(rb_red_blk_tree*,rb_red_blk_node*); //!! get the rank of the node

#endif

