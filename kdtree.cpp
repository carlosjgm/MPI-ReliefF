#include "kdtree.h"

vector<node> kdTree(vector< vector<float> > dataset, int m);
void aux_kdtree(int rootId, vector< vector<float> > dataset, int attrIndex, int m);

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

string toString(vector<node> nodes);
