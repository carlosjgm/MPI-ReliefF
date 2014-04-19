#include "kdtree.h"
#include <iostream>

kdTree::kdTree(vector<float> dataset, int m, vector<bool> nominal) {

    this->m = m;
    this->nominal = nominal;
    this->nodeList.push_back(node(0));
    aux_kdtree(0, dataset, 0);

}

void kdTree::aux_kdtree(int subroot, vector<float> dataset, int attrIndex) {

    //only one element in dataset
    if (dataset.size() == this->m + 1)
        this->nodeList[subroot].setValues(dataset);
    
    //separate dataset by median
    else {
        float median = findMedian(dataset, attrIndex);
        vector<float> leftData;
        vector<float> rightData;
        for (int i = 0; i < dataset.size() / (this->m + 1); i++) {
            if (dataset[i * (this->m + 1) + attrIndex] <= median)
                for (int j = 0; j < this->m + 1; j++)
                    leftData.push_back(dataset[i * (this->m + 1) + j]);
            else
                for (int j = 0; j < this->m + 1; j++)
                    rightData.push_back(dataset[i * (this->m + 1) + j]);
        }

        //find an attrIndex that separates data if the curr attrIndex does not 
        //distinguish between elements in dataset
        int searchCount = 0;
        while (rightData.size() == 0 && searchCount < this->m) {
            leftData.clear();
            rightData.clear();
            attrIndex++;
            if (attrIndex == this->m) attrIndex = 0;
            median = findMedian(dataset, attrIndex);
            for (int i = 0; i < dataset.size() / (this->m + 1); i++) {
                if (dataset[i * (this->m + 1) + attrIndex] <= median)
                    for (int j = 0; j < this->m + 1; j++)
                        leftData.push_back(dataset[i * (this->m + 1) + j]);
                else
                    for (int j = 0; j < this->m + 1; j++)
                        rightData.push_back(dataset[i * (this->m + 1) + j]);
            }
            searchCount++;
        }

        //dataset contains repeated elements
        if (rightData.size() == 0){
            vector<float> temp;
            for(int i=0; i<this->m+1;i++)
                temp.push_back(dataset[i]);
            this->nodeList[subroot].setValues(temp);
        }
        
        else {
            
            this->nodeList[subroot].setAttrIndex(attrIndex);
            this->nodeList[subroot].setMedian(median);

            //add left child
            int leftIndex = this->nodeList.size();
            this->nodeList[subroot].addLeft(leftIndex);            
            this->nodeList.push_back(node(leftIndex, subroot));
            
            //add right child
            int rightIndex = this->nodeList.size();
            this->nodeList[subroot].addRight(rightIndex);            
            this->nodeList.push_back(node(rightIndex, subroot));
            
            attrIndex++;
            if (attrIndex == m) attrIndex = 0;

            //expand left
            aux_kdtree(leftIndex, leftData, attrIndex);
            
            //expand right
            aux_kdtree(rightIndex, rightData, attrIndex);
        }
    }

}

kdTree::kdTree(vector<node> nodeList, int m, vector<bool> nominal) {

    this->nodeList = nodeList;
    this->m = m;
    this->nominal = nominal;

}

float kdTree::findMedian(vector<float> dataset, int attrIndex) {
    
    int m = this->m;
    
    int n = dataset.size()/(m+1);
    int low = 0, high = n - 1, median = (low + high) / 2, middle, ll, hh;

    for (;;) {
        if (high <= low)
            return dataset[median*(m+1)+attrIndex];

        if (high == low + 1) {
            if (dataset[low*(m+1)+attrIndex] > dataset[high*(m+1)+attrIndex])
                swap(&dataset[low*(m+1)+attrIndex], &dataset[high*(m+1)+attrIndex]);
            return dataset[median*(m+1)+attrIndex];
        }

        middle = (low + high) / 2;
        if (dataset[middle*(m+1)+attrIndex] > dataset[high*(m+1)+attrIndex]) swap(&dataset[middle*(m+1)+attrIndex], &dataset[high*(m+1)+attrIndex]);
        if (dataset[low*(m+1)+attrIndex] > dataset[high*(m+1)+attrIndex]) swap(&dataset[low*(m+1)+attrIndex], &dataset[high*(m+1)+attrIndex]);
        if (dataset[middle*(m+1)+attrIndex] > dataset[low*(m+1)+attrIndex]) swap(&dataset[middle*(m+1)+attrIndex], &dataset[low*(m+1)+attrIndex]);

        /* Swap low item (now in position middle) into position (low+1) */
        swap(&dataset[middle*(m+1)+attrIndex], &dataset[(low + 1)*(m+1)+attrIndex]);

        /* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (dataset[low*(m+1)+attrIndex] > dataset[ll*(m+1)+attrIndex]);
            do hh--; while (dataset[hh*(m+1)+attrIndex] > dataset[low*(m+1)+attrIndex]);

            if (hh < ll)
                break;

            swap(&dataset[ll*(m+1)+attrIndex], &dataset[hh*(m+1)+attrIndex]);
        }

        /* Swap middle item (in position low) back into correct position */
        swap(&dataset[low*(m+1)+attrIndex], &dataset[hh*(m+1)+attrIndex]);

        /* Re-set active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;

    }
}

void kdTree::swap(float * a, float * b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

vector< vector<float> > kdTree::kNeighborSearch(vector<float> observation, int k) {
    
    this->knnlist = knnList(k, observation, this->nominal);
    
    //find nearest neighbor
    int attrIndex;
    float compareValue;
    node currNode = this->nodeList[0];
    while (!currNode.isLeaf()) {
        attrIndex = currNode.getAttrIndex();
        compareValue = currNode.getMedian();
        if (observation[attrIndex] <= compareValue)
            currNode = this->nodeList[currNode.getLeft()];
        else
            currNode = this->nodeList[currNode.getRight()];
    }
    this->knnlist.add(currNode.getValues());
    this->nodeList[currNode.getIndex()].visit();
    
    aux_kNeighborSearch(currNode.getParent(), observation);
    
    unvisitNodes();
    
    return this->knnlist.getNeighbors();
}

void kdTree::aux_kNeighborSearch(int root, vector<float> observation) {
        
    node currNode = this->nodeList[root];

    //reached a possible nearest neighbor
    if (currNode.isLeaf()) {
        this->knnlist.add(currNode.getValues());
        
        //mark node as visited
        this->nodeList[root].visit();
        
        root = currNode.getParent();
        currNode = this->nodeList[root];

    }

    if (!currNode.isVisited()) {        
        
        //check if subtree contains possible nearest neighbors
        bool checkLeft = true;
        bool checkRight = true;
        if (this->knnlist.isFull()) {
            float subTreeMinDistToObs;
            if(this->nominal[currNode.getAttrIndex()])
                subTreeMinDistToObs = 1;
            else
                subTreeMinDistToObs = abs(observation[currNode.getAttrIndex()] - currNode.getMedian());
            
            if (subTreeMinDistToObs >= this->knnlist.getMaxDistance()) {
                checkRight = false;
                checkLeft = false;
            }
        }
        
        //mark node as visited
        this->nodeList[root].visit();
        
        //check left
        int tempIndex = currNode.getLeft();
        if (checkLeft && tempIndex != -1 && !this->nodeList[tempIndex].isVisited())
            aux_kNeighborSearch(tempIndex, observation);

        //check right
        tempIndex = currNode.getRight();
        if (checkRight && tempIndex != -1 && !this->nodeList[tempIndex].isVisited())
            aux_kNeighborSearch(tempIndex, observation);

        //move up the tree
        if (currNode.getParent() != -1 && !this->nodeList[currNode.getParent()].isVisited())
            aux_kNeighborSearch(currNode.getParent(), observation);
    }
    
}

void kdTree::unvisitNodes() {
    for(int i=0; i < this->nodeList.size(); i++)
        this->nodeList[i].unvisit();
}

vector<node> kdTree::getNodeList() {
    return this->nodeList;
}

string kdTree::toString() {
    
    string result;
    node currNode;
    queue<node> nodeQueue;
    nodeQueue.push(this->nodeList[0]);
    while (!nodeQueue.empty()) {
        currNode = nodeQueue.front();
        nodeQueue.pop();
        result += currNode.toString();
        if (currNode.getLeft() != -1)
            nodeQueue.push(this->nodeList[currNode.getLeft()]);
        else
            result += " nL";
        if (currNode.getRight() != -1)
            nodeQueue.push(this->nodeList[currNode.getRight()]);
        else
            result += " nR";
        result += "\n";
    }
    return result;
    
}
