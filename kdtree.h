#ifndef KDTREE_H
#define	KDTREE_H

#include "node.h"
#include <queue>
#include <stack>

using namespace std;

//
// functions used for building kdTree
//
node kdTree(vector<float> dataset, int m);
void aux_kdtree(node * root, vector<float> dataset, int attrIndex, int m);
float findMedian(vector<float> dataset, int attrIndex, int m);
void swap(float * a, float * b);
void unvisitNodes(node * root);

//
// function used for searching in the kdtree
//
vector<float> kNeighborSearch(node * root, vector<float> observation, int k, vector<int> nominal, int m);
void aux_kNeighborSearch(node * root, vector<float> observation, int k, vector<int> nominal, vector<float> * knnlist, vector<float> * distances, float * maxDistance, int * maxIndex, int m);

string treeToString(node root);

#endif	/* KDTREE_H */

