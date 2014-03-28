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
        while (rightData.size() == 0 && searchCount < m) {
            leftData.clear();
            rightData.clear();
            attrIndex++;
            if(attrIndex == m) attrIndex=0;
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
            
            attrIndex++;
            if(attrIndex == m) attrIndex=0;
            
            aux_kdtree(root->getLeft(), leftData, attrIndex, m);
            aux_kdtree(root->getRight(), rightData, attrIndex, m);
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

void unvisitNodes(node * root) {
    queue<node *> nodeQueue;
    node * currNode;
    nodeQueue.push(root);
    while (!nodeQueue.empty()) {
        currNode = nodeQueue.front();
        nodeQueue.pop();
        currNode->unvisit();
        if (currNode->getLeft() != NULL)
            nodeQueue.push(currNode->getLeft());
        if (currNode->getRight() != NULL)
            nodeQueue.push(currNode->getRight());
    }
}

vector< vector<float> > kNeighborSearch(node * root, vector<float> observation, int k) {
    vector< vector<float> > knnlist;
    vector<float> distances;
    float maxDistance = -1;
    int maxIndex = -1;

    //find nearest neighbor
    int attrIndex;
    float compareValue;
    node * currNode = root;
    while (!currNode->isLeaf()) {
        attrIndex = currNode->getAttrIndex();
        compareValue = currNode->getMedian();
        if (observation[attrIndex] <= compareValue)
            currNode = currNode->getLeft();
        else
            currNode = currNode->getRight();
    }
    float currDistanceToObs = 0;
    vector<float> currValues = currNode->getValues();
    for (int i = 0; i < currValues.size(); i++) {
        currDistanceToObs += abs(observation[i] - currValues[i]);
    }
    if (currDistanceToObs != 0) {
        knnlist.push_back(currValues);
        distances.push_back(currDistanceToObs);
        maxDistance = currDistanceToObs;
        maxIndex = 0;
    }

    currNode->visit();
    
    //find other k neighbors    
    stack<node *> nodeStack;   
    nodeStack.
    nodeStack.push(currNode->getParent());
    while (!nodeStack.empty()) {
        currNode = nodeStack.top();
        nodeStack.pop();

        //reached a possible nearest neighbor
        if (currNode->isLeaf()) {
            //determine distance of current leaf
            currDistanceToObs = 0;
            currValues = currNode->getValues();
            for (int i = 0; i < currValues.size(); i++) {
                currDistanceToObs += abs(observation[i] - currValues[i]);
            }
            //add to knnlist if the list is not full or if it is a new nearest neighbor
            if (knnlist.size() != k) {
                knnlist.push_back(currValues);
                distances.push_back(currDistanceToObs);
            } else {
                if (currDistanceToObs < maxDistance) {
                    knnlist.at(maxIndex) = currValues;
                    distances[maxIndex] = currDistanceToObs;
                    //update maxDistance
                    maxDistance = -1;
                    maxIndex = -1;
                    for (int i = 0; i < distances.size(); i++)
                        if (distances[i] > maxDistance) {
                            maxDistance = distances[i];
                            maxIndex = i;
                        }
                }
            }
            //mark node as visited
            currNode->visit();
        }

            //check if subtree contains possible nearest neighbors
        else {
            
            //move up the tree
            currNode->visit();
            if (currNode->getParent() != NULL && !currNode->getParent()->isVisited())
                nodeStack.push(currNode->getParent());
            
            bool checkLeft = true;
            bool checkRight = true;
            if (knnlist.size() == k) {
                float subTreeMinDistToObs = abs(observation[currNode->getAttrIndex()] - currNode->getMedian());
                if (subTreeMinDistToObs >= maxDistance) {
                    checkRight = false;
                    checkLeft = false;
                }
            }

            //check left
            node * nextSubtree = currNode->getLeft();
            if (checkLeft && nextSubtree != NULL && !nextSubtree->isVisited())
                nodeStack.push(nextSubtree);

            //check right
            nextSubtree = currNode->getRight();
            if (checkRight && nextSubtree != NULL && !nextSubtree->isVisited())
                nodeStack.push(nextSubtree);
            
        }
    }

    unvisitNodes(root);
    
    return knnlist;

}

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
        result += "\n";
    }
    return result;
}
