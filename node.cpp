#include "node.h"

node::node(int nodeIndex, vector < float > values) {
    this->index = nodeIndex;
    this->values = values;
    this->visited = false;
    this->left = -1;
    this->right = -1;
    this->parent = -1;
}

node::node(int nodeIndex, int attrIndex, float median) {
    this->index = nodeIndex;
    this->median = median;
    this->attrIndex = attrIndex;
    this->visited = false;
    this->left = -1;
    this->right = -1;
    this->parent = -1;
}

node::node(int nodeIndex, int parent) {
    this->index = nodeIndex;
    this->parent = parent;
    this->visited = false;
    this->left = -1;
    this->right = -1;
}

node::node(int nodeIndex) {
    this->index = nodeIndex;
    this->visited = false;
    this->left = -1;
    this->right = -1;
    this->parent = -1;
}

node::node() {
    this->index = -1;
    this->visited = false;
    this->left = -1;
    this->right = -1;
    this->parent = -1;
}

int node::getParent() {
    return this->parent;
}

void node::addLeft(int childIndex) {
    if(this->left != -1)
        abort();
    else
        this->left = childIndex;
}

void node::addRight(int childIndex) {
    if(this->right != -1)
        abort();
    else
        this->right = childIndex;
}

vector < float > node::getValues() {
    if(this->isLeaf())
        return this->values;
    else
        abort();
}

void node::setValues(vector < float > values) {
    this->values = values;
}

int node::getIndex() {
    return this->index;
}

int node::getLeft() {
    return this->left;
}

int node::getRight() {
    return this->right;
}

bool node::isLeaf() {
    return this->left == -1 && this->right == -1;
}

float node::getMedian() {
    return median;
}

int node::getAttrIndex() {
    return this->attrIndex;
}

void node::setMedian(float median) {
    this->median = median;
}

void node::setAttrIndex(int attrIndex) {
    this->attrIndex = attrIndex;
}

void node::visit() {
    this->visited = true;
}

void node::unvisit() {
    this->visited = false;
}

bool node::isVisited() {
    return this->visited;
}

string node::toString() {
    string result;
    ostringstream temp;
    if (this->isLeaf() && this->values.size() != 0) {
        result = "[";
        for (int i = 0; i < this->values.size() - 1; i++)
            temp << this->values[i] << ", ";
        temp << this->values[this->values.size() - 1];
        result += temp.str() + "]";
    }
    else {
        temp << this->attrIndex << ", " << this->median;
        result = "(" + temp.str() + ")";
    }

    return result;
}
