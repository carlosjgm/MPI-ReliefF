#ifndef KDTREE_H
#define	KDTREE_H

#include "node.h"
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

//
// functions used for building kdTree
//
vector<node> kdTree(vector< vector<float> > dataset, int m);
void aux_kdtree(int rootId, vector< vector<float> > dataset, int attrIndex, int m);
float findMedian(vector< vector<float> > dataset, int attrIndex);
void swap(float * a, float * b);
void unvisitNodes(vector<node> * nodes);

//
// function used for searching in the kdtree
//
vector< node > kNeighborSearch(vector<node> nodes, vector<float> observation, int k);

string toString(vector<node> nodes);


#endif	/* KDTREE_H */

