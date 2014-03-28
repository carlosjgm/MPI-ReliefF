#ifndef KDTREE_H
#define	KDTREE_H

#include "node.h"
#include <queue>
#include <stack>

using namespace std;

//
// functions used for building kdTree
//
node kdTree(vector< vector<float> > dataset, int m);
void aux_kdtree(node * root, vector< vector<float> > dataset, int attrIndex, int m);
float findMedian(vector< vector<float> > dataset, int attrIndex);
void swap(float * a, float * b);
void unvisitNodes(node * root);

//
// function used for searching in the kdtree
//
vector< vector<float> > kNeighborSearch(node * root, vector<float> observation, int k);

string treeToString(node root);


#endif	/* KDTREE_H */

