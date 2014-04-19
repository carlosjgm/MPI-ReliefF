#include "knnList.h"
#include <iostream>

knnList::knnList() {
}

knnList::knnList(int k, vector<float> obervation, vector<bool> nominal) {
    this->k = k;
    this->observation = obervation;
    this->nominal = nominal;
    this->m = observation.size();
}

void knnList::add(vector<float> candidate) {

    float distance = this->distanceTo(candidate);

    if (distance != 0) {

        if (this->values.size() == 0) {

            this->values.push_back(candidate);
            this->distances.push_back(distance);

        } else if (!this->isFull()) {
            
            int index;
            for (index = 0; index < this->values.size(); index++)
                if (distance < this->distances[index])
                    break;
            this->values.insert(this->values.begin() + index, candidate);
            this->distances.insert(this->distances.begin() + index, distance);


        } else if (distance < this->distances[k - 1]) {

            this->values.erase(this->values.end() - 1);
            this->distances.erase(this->distances.end() - 1);
            int index;
            for (index = 0; index < this->values.size(); index++)
                if (distance < this->distances[index])
                    break;
            this->values.insert(this->values.begin() + index, candidate);
            this->distances.insert(this->distances.begin() + index, distance);

        }
    }
}

vector< vector<float> > knnList::getNeighbors() {
    return values;
}

bool knnList::isFull() {
    return this->k == this->values.size();
}

float knnList::getMaxDistance() {
    return distances.back();
}


float knnList::distanceTo(vector<float> candidate) {
    float temp = 0;
    for (int i = 0; i < m; i++)
        if (nominal[i]) {
            if (observation[i] != candidate[i])
                temp++;
        } else
            temp += abs(candidate[i] - this->observation[i]);
    return temp;
}
