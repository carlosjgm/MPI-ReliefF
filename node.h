#ifndef NODE_H
#define	NODE_H

#include <vector>
#include <cfloat>
#include <cmath>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

class node {
public:

    node(vector < float > values);
    node(int attrIndex, float median);
    node(node * parent);
    node();
    float distanceTo(node * target);    
    node * getParent();
    void addChildren();
    vector < float > getValues();     
    void setValues(vector < float > values);    
    node * getLeft();
    node * getRight();    
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

    node * parent;
    node * left;
    node * right;
    vector < float > values;

    float median;
    int attrIndex;

    bool visited;
    bool leaf;
};

#endif	/* NODE_H */

