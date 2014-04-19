#ifndef KDTREE_H
#define	KDTREE_H

#include "node.h"
#include "knnList.h"
#include <queue>
#include <stack>

using namespace std;

class kdTree {
public:

    kdTree(vector<float> dataset, int m, vector<bool> nominal);
    kdTree(vector<node> nodeList, int m, vector<bool> nominal);
    vector< vector<float> > kNeighborSearch(vector<float> observation, int k);
    vector<node> getNodeList();
    string toString();
    
private:

    void aux_kdtree(int subroot, vector<float> dataset, int attrIndex);
    void aux_kNeighborSearch(int root, vector<float> observation);
    float findMedian(vector<float> dataset, int attrIndex);
    void swap(float * a, float * b);
    void unvisitNodes();

    int m;
    vector<bool> nominal;
    knnList knnlist;
    vector<node> nodeList;
    
};

#endif	/* KDTREE_H */

