#ifndef KNNLIST_H
#define	KNNLIST_H

#include <vector>
#include <cmath>

using namespace std;

class knnList {    
public:
    
    knnList();
    knnList(int k, vector<float> obervation, vector<bool> nominal);
    void add(vector<float> candidate);
    vector< vector<float> > getNeighbors();
    bool isFull();
    float getMaxDistance();
    
private:   
    
    float distanceTo(vector<float> candidate);
    vector< vector<float> > values;
    vector<float> distances;
    int k;
    int m;
    vector<float> observation;
    vector<bool> nominal;
};

#endif	/* KNNLIST_H */

