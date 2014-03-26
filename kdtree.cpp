#include "kdtree.h"

node kdTree(vector< vector<float> > dataset, int m) {
    node root;
    aux_kdtree(&root, dataset, 0, m);
    return root;
}

void aux_kdtree(node * root, vector< vector<float> > dataset, int attrIndex, int m) {

    //base case: create leaf node with values
    if (dataset.size() == 1)
        root->setValues(dataset[0]);

    else {
        float median = findMedian(dataset, attrIndex);
        vector< vector<float> > leftData;
        vector< vector<float> > rightData;
        for (int i = 0; i < dataset.size(); i++) {
            if (dataset[i][attrIndex] <= median)
                leftData.push_back(dataset[i]);
            else
                rightData.push_back(dataset[i]);
        }

        int searchCount = 0;
        while (leftData.size() == 0 && searchCount < m) {
            attrIndex = (attrIndex + 1) % m;
            median = findMedian(dataset, attrIndex);
            for (int i = 0; i < dataset.size(); i++) {
                if (dataset[i][attrIndex] <= median)
                    leftData.push_back(dataset[i]);
                else
                    rightData.push_back(dataset[i]);
            }
            searchCount++;
        }

        if (rightData.size() == 0)
            root->setValues(dataset[0]);

        else {
            root->setMedian(median);
            root->setAttrIndex(attrIndex);
            root->addChildren();

            aux_kdtree(root->getLeft(), leftData, (attrIndex + 1) % m, m);
            aux_kdtree(root->getRight(), rightData, (attrIndex + 1) % m, m);
        }
    }
}

float findMedian(vector< vector<float> > dataset, int attrIndex) {
    int n = dataset.size();
    int low = 0, high = n - 1, median = (low + high) / 2, middle, ll, hh;

    for (;;) {
        if (high <= low)
            return dataset[median][attrIndex];
        
        if (high == low + 1) {
            if (dataset[low][attrIndex] > dataset[high][attrIndex])
                swap(&dataset[low][attrIndex], &dataset[high][attrIndex]);
            return dataset[median][attrIndex];
        }

        middle = (low + high) / 2;
        if (dataset[middle][attrIndex] > dataset[high][attrIndex]) swap(&dataset[middle][attrIndex], &dataset[high][attrIndex]);
        if (dataset[low][attrIndex] > dataset[high][attrIndex]) swap(&dataset[low][attrIndex], &dataset[high][attrIndex]);
        if (dataset[middle][attrIndex] > dataset[low][attrIndex]) swap(&dataset[middle][attrIndex], &dataset[low][attrIndex]);

        /* Swap low item (now in position middle) into position (low+1) */
        swap(&dataset[middle][attrIndex], &dataset[low + 1][attrIndex]);

        /* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (dataset[low][attrIndex] > dataset[ll][attrIndex]);
            do hh--; while (dataset[hh][attrIndex] > dataset[low][attrIndex]);

            if (hh < ll)
                break;

            swap(&dataset[ll][attrIndex], &dataset[hh][attrIndex]);
        }

        /* Swap middle item (in position low) back into correct position */
        swap(&dataset[low][attrIndex], &dataset[hh][attrIndex]);

        /* Re-set active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;

    }
}

void swap(float * a, float * b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

void unvisitNodes(vector<node> * nodes) {
    for (int i = 0; i < nodes->size(); i++)
        nodes->at(i).unvisit();
}

vector< node > kNeighborSearch(vector<node> nodes, vector<float> observation, int k);

string treeToString(node root) {
    string result;
    node currNode;
    queue<node> nodeQueue;
    nodeQueue.push(root);
    while (!nodeQueue.empty()) {
        currNode = nodeQueue.front();
        nodeQueue.pop();
        result += currNode.toString();
        if (currNode.getLeft() != NULL)
            nodeQueue.push(*currNode.getLeft());
        else
            result += " nL";
        if (currNode.getRight() != NULL)
            nodeQueue.push(*currNode.getRight());
        else
            result += " nR";
        result  += "\n";
    }
    return result;
}
