#include "node.h"

node::node(vector < float > values) {
    this->values = values;
    this->visited = false;
}

node::node(int attrIndex, float median) {
    this->median = median;
    this->attrIndex = attrIndex;
    this->visited = false;
}

node::node(node * parent) {
    this->parent = parent;
    this->visited = false;
}

float node::distanceTo(node * target) {

    if (target == NULL || target->getValues().empty()) {
        return FLT_MAX;
    } else {
        vector < float > targetValues = target->getValues();
        float distance = 0;
        for (int i = 0; i < targetValues.size(); i++)
            distance += abs(this->values[i] - targetValues[i]);
        return distance;
    }
}

node * node::getParent() {
    return this->parent;
}

void node::addChildren() {
    node left(this);
    this->left = &left;
    node right(this);
    this->right = &right;
}

vector < float > node::getValues() {
    return this->values;
}

void node::setValues(vector < float > values) {
    vector< float >().swap(this->values);
    this->values = values;
}

node * node::getLeft() {
    return this->left;
}

node * node::getRight() {
    return this->right;
}

bool node::isLeaf() {
    return this->left == NULL && this->right == NULL;
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
    if (this->isLeaf()) {
        result = "({";
        for (int i = 0; i < this->values.size() - 1; i++) {
            temp << this->values[i];
            result += temp.str() + ", ";
        }
        temp << this->values[this->values.size() - 1];
        result += temp.str() + "})";
    } else
        temp << this->attrIndex << ", " << this->median;
    result = "(" + temp.str() + ")";

    return result;
}