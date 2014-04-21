#ifndef NODE_H
#define	NODE_H

#include <vector>
#include <cfloat>
#include <cmath>
#include <string>
#include <sstream>
#include <stdio.h>
#include <cstdlib>

using namespace std;

class node {
public:

    node(int nodeIndex, vector < float > values);
    node(int nodeIndex, int attrIndex, float median);
    node(int nodeIndex, int parent);
    node(int nodeIndex);
    node(int nodeIndex, int parent, int left, int right, int attrIndex, float median, float values[], int n);
    node();
    int getParent();
    void addLeft(int childIndex);
    void addRight(int childIndex);
    vector < float > getValues();
    void setValues(vector < float > values);
    int getIndex();
    int getLeft();
    int getRight();
    bool isLeaf();
    float getMedian();
    int getAttrIndex();
    void setMedian(float median);
    void setAttrIndex(int attrIndex);
    void visit();
    void unvisit();
    bool isVisited();
    string toString();

private:

    int index;
    int parent;
    int left;
    int right;
    int attrIndex;
    int visited;
    float median;
    
    vector < float > values;

};

#endif	/* NODE_H */

