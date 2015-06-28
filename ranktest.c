#include "red_black_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <time.h>


/*  test the CDF computation in a red-black tree: allocate an array,
 * 	fill it with random numbers, add the numbers to a tree, delete
 * 	some elements, sort the array, and check if the rank corresponds
 * 	to the position in the sorted array */

#define EPSILON 1.0e-15


static inline void swap(int64_t* s, unsigned int i, unsigned int j) {
	int64_t tmp = s[i];
	s[i] = s[j];
	s[j] = tmp;
}

static inline int cmp(int64_t* s, unsigned int i, unsigned int j) {
	if(s[i] < s[j]) return -1;
	if(s[i] > s[j]) return 1;
	return 0;
}

static void quicksort(int64_t* d, unsigned int s, unsigned int e) {
	if(s >= e) return;
	if(e-s == 1) return;
	if(e-s == 2) {
		if(cmp(d,s,s+1) > 0) swap(d,s,s+1);
		return;
	}
	
	unsigned int p = s + (e-s)/2;
	unsigned int i;
	swap(d,p,e-1);
	p = s;
	for(i=s;i<e-1;i++) {
		int a = cmp(d,e-1,i);
		if(a == 1 || (a == 0 && p<s+(e-s)/2)) {
			if(i>p) swap(d,i,p);
			p++;
		}
	}
	if(p < e-1) swap(d,p,e-1);
	quicksort(d,s,p);
	quicksort(d,p+1,e);
}



int main(int argc, char** argv) {
  int option=0;
  int64_t newKey,newKey2;
  rb_red_blk_node* newNode;
  rb_red_blk_tree* tree;
  int64_t* array = 0;
  int64_t* array2 = 0;
  unsigned int N = 65536; //total number of elements to insert
  unsigned int M = 16384; //number of elements to delete from the beginning
  unsigned int M2 = 16384; //number of elements to delete from the end
  int i;
  unsigned int j;
  time_t t1 = time(0);
  unsigned int seed = t1;
  double par = 2.5;
  
  for(i=1;i<argc;i++) if(argv[i][0] == '-') switch(argv[i][1]) {
	  case 'N':
	  	N = atoi(argv[i+1]);
	  	break;
	  case 'M':
	  	M = atoi(argv[i+1]);
	  	if(i+2 < argc) {
			if(isdigit(argv[i+2][0])) M2 = atoi(argv[i+2]);
			else M2 = M;
		}
		else M2 = M;
		break;
	  case 's':
	  	seed = atoi(argv[i+1]);
	  	break;
	  case 'p':
	  	par = atof(argv[i+1]);
		break;
	  default:
	  	fprintf(stderr,"unrecognized parameter: %s!\n",argv[i]);
	  	break;
  }
  
  if(M + M2 >= N) {
	  fprintf(stderr,"Error: number of elements to delete (%u + %u) is more than the total number of elements (%u)!\n",
	  	M,M2,N);
	  return 1;
  }
  
  tree=RBTreeCreate(CmpInt64,NullFunction,NullFunction,NullFunction,NullFunction,DFInt64,&par);
  array = SafeMalloc(sizeof(int64_t)*N);
  for(j=0;j<N;j++) {
	  array[j] = ((int64_t)rand())*((int64_t)rand())*((int64_t)rand());
	  RBTreeInsert(tree,(void*)(array[j]),0);
  }
  
  for(j=0;j<M;j++) {
	  newNode = RBExactQuery(tree,(void*)(array[j]));
	  if(!newNode) {
		  fprintf(stderr,"Error: node not found!\n");
		  goto rbt_end;
	  }
	  RBDelete(tree,newNode);
  }
  
  for(j=N-M2;j<N;j++) {
	  newNode = RBExactQuery(tree,(void*)(array[j]));
	  if(!newNode) {
		  fprintf(stderr,"Error: node not found!\n");
		  goto rbt_end;
	  }
	  RBDelete(tree,newNode);
  }
  
  N = N-M2-M;
  array2 = array+M;
  quicksort(array2,0,N);
  j=0;
  newNode = TreeFirst(tree);
  double cdf = 0.0;
  do {
	  int64_t v1 = (int64_t)(newNode->key);
	  if(v1 != array2[j]) {
		  fprintf(stderr,"error: %d != %d!\n",v1,array2[j]);
		  break;
	  }
	  double cdf2 = GetNodeRank(tree,newNode);
	  double diff = fabs(cdf2-cdf);
	  if(diff > EPSILON) {
		  fprintf(stderr,"wrong cdf value: %g != %g (diff: %g)!\n",cdf,cdf2,diff);
		  break;
	  }
	  j++;
	  cdf += DFInt64((void*)array2[j],&par);
	  newNode = TreeSuccessor(tree,newNode);
  } while(newNode != tree->nil && j<N);
  
  if( !(newNode == tree->nil && j == N) ) {
	  fprintf(stderr,"error: tree or array too short / long!\n");
  }

rbt_end:
  
  RBTreeDestroy(tree);
  free(array);
  
  time_t t2 = time(0);
  fprintf(stderr,"runtime: %u\n",(unsigned int)(t2-t1));
  
  return 0;
}




