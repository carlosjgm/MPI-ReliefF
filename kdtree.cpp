#include "kdtree.h"

node kdTree(vector<float> dataset, int m) {
    node root;
    aux_kdtree(&root, dataset, 0, m);
    return root;
}

void aux_kdtree(node * root, vector<float> dataset, int attrIndex, int m) {

    //base case: create leaf node with values
    if (dataset.size() == m+1)
        root->setValues(dataset);

    else {
        float median = findMedian(dataset, attrIndex, m);
        vector<float> leftData;
        vector<float> rightData;
        for (int i = 0; i < dataset.size()/(m+1); i++) {
            if (dataset[i * (m + 1) + attrIndex] <= median)
                for (int j = 0; j < m + 1; j++)
                    leftData.push_back(dataset[i * (m + 1) + j]);
            else
                for (int j = 0; j < m + 1; j++)
                    rightData.push_back(dataset[i * (m + 1) + j]);
        }

        int searchCount = 0;
        while (rightData.size() == 0 && searchCount < m) {
            leftData.clear();
            rightData.clear();
            attrIndex++;
            if (attrIndex == m) attrIndex = 0;
            median = findMedian(dataset, attrIndex, m);
            for (int i = 0; i < dataset.size() / (m + 1); i++) {
                if (dataset[i * (m + 1) + attrIndex] <= median)
                    for (int j = 0; j < m + 1; j++)
                        leftData.push_back(dataset[i * (m + 1) + j]);
                else
                    for (int j = 0; j < m + 1; j++)
                        rightData.push_back(dataset[i * (m + 1) + j]);
            }
            searchCount++;
        }

        if (rightData.size() == 0)
            root->setValues(dataset);

        else {
            root->setMedian(median);
            root->setAttrIndex(attrIndex);
            root->addChildren();

            attrIndex++;
            if (attrIndex == m) attrIndex = 0;

            aux_kdtree(root->getLeft(), leftData, attrIndex, m);
            aux_kdtree(root->getRight(), rightData, attrIndex, m);
        }
    }
}

float findMedian(vector<float> dataset, int attrIndex, int m) {
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

vector<float> kNeighborSearch(node * root, vector<float> observation, int k, vector<int> nominal, int m) {
    vector<float> knnlist;
    knnlist.reserve(k*(m+1)*sizeof(float));
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
        if(nominal[i] && observation[i]!=currValues[i])
                currDistanceToObs++;
        else
            currDistanceToObs += abs(observation[i] - currValues[i]);
    }
    if (currDistanceToObs != 0) {
        knnlist.insert(knnlist.end(),currValues.begin(),currValues.end());
        distances.push_back(currDistanceToObs);
        maxDistance = currDistanceToObs;
        maxIndex = 0;
    }

    currNode->visit();
    
    aux_kNeighborSearch(currNode->getParent(), observation, k, nominal, &knnlist, &distances, &maxDistance, &maxIndex,m);
    
    unvisitNodes(root);
    
    return knnlist;
}

void aux_kNeighborSearch(node * root, vector<float> observation, int k, vector<int> nominal, vector<float> * knnlist, vector<float> * distances, float * maxDistance, int * maxIndex, int m) {
    
    node * currNode = root;   
    
    //reached a possible nearest neighbor
    if (currNode->isLeaf()) {
        //determine distance of current leaf
        float currDistanceToObs = 0;
        vector<float> currValues = currNode->getValues();
        for (int i = 0; i < currValues.size(); i++) {
            if(nominal[i] && observation[i]!=currValues[i])
                currDistanceToObs++;
            else
                currDistanceToObs += abs(observation[i] - currValues[i]);
        }
        //add to knnlist if the list is not full or if it is a new nearest neighbor
        
        if (knnlist->size()/(m+1) != k) {
            knnlist->insert(knnlist->end(),currValues.begin(),currValues.end());
            distances->push_back(currDistanceToObs);
            //update maxDistance
            *maxDistance = -1;
            *maxIndex = -1;
            for (int i = 0; i < distances->size(); i++)
                if (distances->at(i) > *maxDistance) {
                    *maxDistance = distances->at(i);
                    *maxIndex = i;
                }
        } else if (currDistanceToObs < *maxDistance) {
            
            for(int i=0; i<m+1; i++)
                knnlist->at((*maxIndex) * (m+1)+i) = currValues[i];
            distances->at(*maxIndex) = currDistanceToObs;
            //update maxDistance
            *maxDistance = -1;
            *maxIndex = -1;
            for (int i = 0; i < distances->size(); i++)
                if (distances->at(i) > *maxDistance) {
                    *maxDistance = distances->at(i);
                    *maxIndex = i;
                }
        }        
        
        currNode->visit();
        currNode = currNode->getParent();
        
    }

    if (!currNode->isVisited()) {
        
        //mark node as visited
        currNode->visit();
        
        //check if subtree contains possible nearest neighbors
        bool checkLeft = true;
        bool checkRight = true;
        if (knnlist->size()/(m+1) == k) {
            float subTreeMinDistToObs;
            if(nominal[currNode->getAttrIndex()])
                subTreeMinDistToObs = 1;
            else
                subTreeMinDistToObs = abs(observation[currNode->getAttrIndex()] - currNode->getMedian());
            
            if (subTreeMinDistToObs >= *maxDistance) {
                checkRight = false;
                checkLeft = false;
            }
        }

        //check left
        node * nextSubtree = currNode->getLeft();
        if (checkLeft && nextSubtree != NULL && !nextSubtree->isVisited())
            aux_kNeighborSearch(nextSubtree, observation, k, nominal, knnlist, distances, maxDistance, maxIndex,m);

        //check right
        nextSubtree = currNode->getRight();
        if (checkRight && nextSubtree != NULL && !nextSubtree->isVisited())
            aux_kNeighborSearch(nextSubtree, observation, k, nominal, knnlist, distances, maxDistance, maxIndex,m);

        //move up the tree
        if (currNode->getParent() != NULL && !currNode->getParent()->isVisited())
            aux_kNeighborSearch(currNode->getParent(), observation, k, nominal, knnlist, distances, maxDistance, maxIndex,m);
    }
    
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
