#include <stdio.h>
#include <cstdlib>
#include "kdtree.h"
#include <fstream>
#include <list>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

using namespace std;

void test();
double read_timer();
float hash(const char *str);

int main(int argc, char** argv) {
    
    if (argc != 4) {
        cout << "arguments: <input csv file location> <number of neighbors> <number of iterations>\n";
        return(EXIT_FAILURE);
    }

    double startTime = read_timer();

    int k, iternum;
    char* inputPath = argv[1];
    istringstream(argv[2]) >> k;
    istringstream(argv[3]) >> iternum;

    vector<float> dataset;
    vector<string> recordClassList;
    vector<float> floatValues;
    vector<bool> nominal;
    int m;

    //
    //load input
    //
    cout << "loading input\n";
    ifstream infile(inputPath);
    string line;
    //
    //initialize some variables
    //
    bool init = false;
    while (!init) {
        getline(infile, line);
        istringstream currLine(line);
        string result;
        bool skip = false;
        floatValues.clear();
        nominal.clear();
        while (getline(currLine, result, ',')) {
            //check for missing values
            if (result.find_first_not_of(' ') == std::string::npos || result.find("?") != std::string::npos) {
                skip = true;
                break;
            }

            //parse result
            float value = atof(result.c_str());

            //result is nominal
            if (value == 0 && result[0] != '0') {
                nominal.push_back(true);
                value = hash(result.c_str());
                floatValues.push_back(value);
            }                //result is numeric
            else {
                nominal.push_back(false);
                floatValues.push_back(value);
            }
        }
        if (!skip) {
            m = floatValues.size() - 1;
            init = true;
            dataset.insert(dataset.end(),floatValues.begin(),floatValues.end());
            recordClassList.push_back(result);
        }
    }
    
    //
    //initialize max and min
    //
    vector<float> max;
    vector<float> min;
    for (int i = 0; i < m; i++) {
        max.push_back(dataset[i]);
        min.push_back(dataset[i]);
    }

    //iterate over file
    while (getline(infile, line)) {
        istringstream currLine(line);
        string result;
        bool skip = false;
        floatValues.clear();
        while (getline(currLine, result, ',')) {
            //check for missing values
            if (result.find_first_not_of(' ') == std::string::npos || result.find("?") != std::string::npos) {
                skip = true;
                break;
            }

            //parse result
            float value = atof(result.c_str());

            //result is nominal
            if (value == 0 && result[0] != '0') {
                value = hash(result.c_str());
                floatValues.push_back(value);
            }                //result is numeric
            else
                floatValues.push_back(value);
        }

        if (!skip && floatValues.size() == m + 1) {
            dataset.insert(dataset.end(),floatValues.begin(),floatValues.end());
            recordClassList.push_back(result);
            for (int i = 0; i < m; i++) {
                float value = floatValues[i];
                if (value < min[i])
                    min[i] = value;
                if (value > max[i])
                    max[i] = value;
            }
        }
    }
    
    
    cout << "dataset size = " << dataset.size()/(m+1) << " lines\n";
    cout << m << " attributes detected (excluding class)\n";

    //
    //calculate range of values
    //
    vector<float> range(m);
    for (int i = 0; i < m; i++) {
        range[i] = max[i] - min[i];
        if (range[i] == 0)
            range[i] = 1;
        //printf("range[%d]=%g\n",i,range[i]);
    }

    //
    //normalize dataset to [0,1]
    //
    for (int i = 0; i < dataset.size() / (m + 1); i++)
        for (int j = 0; j < m; j++) {
            dataset[i * (m + 1) + j] = (dataset[i * (m + 1) + j] - min[j]) / range[j];
        }

    
   /* for (int i = 0; i < 10; i++) {
        cout << "recordClassList[" << i << "]=" << recordClassList[i] << "\n";
        for (int j = 0; j < m + 1; j++)
            cout << dataset[i*(m+1)+j] << " ";
        cout << "\n";
    }*/

    //
    //build class list
    //
    vector<float> classProbabilityList;
    vector<string> treeClassList;
    vector< vector<float> > classDatasetList;
    //determine to which class kd-tree each observation belongs to
    int treeClassIndex;
    for (int i = 0; i < dataset.size()/(m+1) ;i++) {
        //determine if obs i belongs to a previously found class
        treeClassIndex = -1;
        for (int c = 0; c < treeClassList.size(); c++) {
            if (!recordClassList[i].compare(treeClassList[c])) {
                treeClassIndex = c;
                break;
            }
        }

        //new class
        if (treeClassIndex == -1) {
            vector<float> newClassDataset;
            newClassDataset.insert(newClassDataset.end(),dataset.begin()+i*(m+1),dataset.begin()+(i+1)*(m+1));
            classDatasetList.push_back(newClassDataset);
            treeClassList.push_back(recordClassList[i]);
        }            //previously added class
        else
            classDatasetList[treeClassIndex].insert(classDatasetList[treeClassIndex].end(),dataset.begin()+i*(m+1),dataset.begin()+(i+1)*(m+1));
    }


    /*cout << "treeClassList.size()=" << treeClassList.size() << "\n" << "classDatasetList.size()=" << classDatasetList.size() << "\n";
    cout << "dataset.size()=" << dataset.size()/(m+1) << "\n";
    int total = 0;
    for (int c = 0; c < treeClassList.size(); c++) {
        cout << "treeClassList[" << c << "].size()=" << classDatasetList[c].size()/(m+1) << ", " << treeClassList[c] << "(" << hash(treeClassList[c].c_str()) << "): \n";
        total += classDatasetList[c].size()/(m+1);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < m+1; j++)
                cout << classDatasetList[c][i*(m+1)+j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
    cout << "total=" << total << "\n";*/



    //
    //build kd-trees for each class
    //
    vector<kdTree> treeList;
    for (int i = 0; i < classDatasetList.size(); i++) {
        vector<float> classDataset = classDatasetList[i];
        cout << "class " << treeClassList[i] << " kd-tree\n";
        treeList.push_back(kdTree(classDataset, m, nominal));

        //calculate probability of class
        classProbabilityList.push_back((float) classDatasetList[i].size() / dataset.size());
        cout << "class probability = " << classProbabilityList[i] << "\n";
    }
    
    //attr Weights
    vector<float> attrWeight(m);
    
    if (iternum < 1 || iternum >= dataset.size()/(m+1))
        iternum = dataset.size()/(m+1);    
    
    //Update attributes
    cout << "Updating attributes\n";
    //for each iteration
    double timer = 0;
    for (int i = 0; i < iternum; i++) {

        if (i % (iternum / 25) == 0)
            cout << (int) ((float) i / iternum * 100) << "% " << flush;

        //select observation          
        int sampleIndex = i;
        //int sampleIndex = rand()%dataset.size();

        vector<float> observation;
        for (int j = 0; j < m; j++)
            observation.push_back(dataset[sampleIndex * (m + 1) + j]);

        int currClassTreeIndex = -1;
        for (int j = 0; j < treeClassList.size(); j++) {
            //cout << "treeClassList " << j << " hash: " << hash(treeClassList[j].c_str()) << endl;
            if (!treeClassList[j].compare(recordClassList[sampleIndex])) {
                currClassTreeIndex = j;
                break;
            }
        }

        //find k near hits
        //timer -= read_timer();
        vector< vector<float> > nearHits = treeList[currClassTreeIndex].kNeighborSearch(observation, k);
        //timer += read_timer();            

        for (int j = 0; j < nearHits.size(); j++) {
            vector<float> currNearHit = nearHits[j];
            for (int z = 0; z < m; z++) {
                if (nominal[z]) {
                    if(observation[z] != currNearHit[z])
                        attrWeight[z] -= 1;
                } else {
                    float diff = abs(observation[z] - currNearHit[z]);
                    attrWeight[z] -= diff;
                }
            }
        }

        //find k near misses from each class
        for (int c = 0; c < treeList.size(); c++)
            if (c != currClassTreeIndex) {
                vector< vector<float> > nearMisses = treeList[c].kNeighborSearch(observation, k);
                for (int j = 0; j < nearMisses.size(); j++) {
                    vector<float> currNearMiss = nearMisses[j];
                    for (int z = 0; z < m; z++) {
                        if (nominal[z]) {
                            if(observation[z] != currNearMiss[z])
                                attrWeight[z] += (classProbabilityList[c] / (1 - classProbabilityList[currClassTreeIndex]));
                        } else {
                            float diff = abs(observation[z] - currNearMiss[z]);
                            attrWeight[z] += (classProbabilityList[c] / (1 - classProbabilityList[currClassTreeIndex])) * diff;
                        }
                    }
                }
            }
    }

    //cout << "\n" << timer << "s\n";

    vector< vector<float> > result(m);

    for (int i = 0; i < m; i++) {
        result[i].push_back(i + 1);
        result[i].push_back(attrWeight[i]);
    }

    //sort result
    for (int i = 0; i < m - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < m; j++) {
            if (result[j][1] > result[maxIndex][1])
                maxIndex = j;
        }
        float temp = result[i][1];
        int tempIndex = (int) result[i][0];
        result[i][1] = result[maxIndex][1];
        result[i][0] = result[maxIndex][0];
        result[maxIndex][1] = temp;
        result[maxIndex][0] = tempIndex;
    }

    cout << "\nweight, attribute\n";
    for (int i = 0; i < m; i++)
        cout << result[i][1] / (iternum * k) << " " << (int) result[i][0] << "\n";

    double elapsedTime = read_timer() - startTime;
    cout << "\nElapsed time: " << ((int) (elapsedTime / 3600)) % 60 << "hr " << ((int) (elapsedTime / 60)) % 60 << "min " << elapsedTime - ((int) (elapsedTime/60))*60 << "s\n";
     
    
    return (EXIT_SUCCESS);

}

float hash(const char *str) {
    int h = 0;
    while (*str)
        h = h << 1 ^ *str++;
    return h;
}

double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void test() {

    //
    //DEFINE TEST DATASET
    //
    printf("\n");
    int n = 9, m = 3;
    vector<float> dataset;
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < m + 1; j++) {
            dataset.push_back((float) ((i * (j + 1)) % n));
        }
    dataset.push_back((float) 5);
    dataset.push_back((float) 2);
    dataset.push_back((float) 6);
    dataset.push_back((float) 4);
    for (int i = 0; i < n; i++) {
        printf("dataset[%d] = (", i);
        for (int j = 0; j < m + 1; j++)
            printf("% g", dataset[i * (m + 1) + j]);
        printf(" )\n");
    }
    printf("dataset.size() = %d\n", dataset.size());
    cout << "number of records = " << dataset.size() / (m + 1) << endl;
    printf("\n");

    //
    //TEST KNNLIST
    //
    vector < float > observation;
    observation.push_back(5);
    observation.push_back(2);
    observation.push_back(6.5);
    vector <bool> nominal;
    nominal.push_back(false);
    nominal.push_back(false);
    nominal.push_back(true);
    knnList neighborList(2, observation, nominal);
    for (int i = 0; i < dataset.size() / (m + 1); i++) {
        vector<float> candidate;
        for (int j = 0; j < m; j++)
            candidate.push_back(dataset[i * (m + 1) + j]);
        neighborList.add(candidate);
        if (i < 1)
            assert(!neighborList.isFull());
        else
            assert(neighborList.isFull());
    }
    assert(neighborList.getMaxDistance() == 2);
    vector< vector<float> > neighbors = neighborList.getNeighbors();
    for (int j = 0; j < m; j++)
        assert(neighbors[0][j] == dataset[8 * (m + 1) + j]);
    for (int j = 0; j < m; j++)
        assert(neighbors[1][j] == dataset[5 * (m + 1) + j]);


    //
    //TEST NODE
    //
    node valueNode(1, observation);
    assert(valueNode.getIndex() == 1);
    assert(valueNode.getLeft() == -1);
    assert(valueNode.getRight() == -1);
    assert(valueNode.getParent() == -1);
    assert(valueNode.isLeaf());
    for (int i = 0; i < m; i++)
        assert(valueNode.getValues()[i] == observation[i]);
    node attrNode(2, 1, 3.5);
    assert(attrNode.getIndex() == 2);
    assert(attrNode.getLeft() == -1);
    assert(attrNode.getRight() == -1);
    assert(attrNode.getParent() == -1);
    assert(attrNode.getAttrIndex() == 1);
    assert(attrNode.getMedian() == 3.5);
    assert(attrNode.isLeaf());
    attrNode.addLeft(3);
    assert(!attrNode.isLeaf());
    attrNode.setMedian(5.5);
    assert(attrNode.getMedian() == 5.5);
    attrNode.setAttrIndex(5);
    assert(attrNode.getAttrIndex() == 5);
    node parentNode(3, 2);
    assert(parentNode.getIndex() == 3);
    assert(parentNode.getLeft() == -1);
    assert(parentNode.getRight() == -1);
    assert(parentNode.getParent() == 2);
    assert(parentNode.isLeaf());
    node genNode(4);
    assert(genNode.getIndex() == 4);
    assert(genNode.getLeft() == -1);
    assert(genNode.getRight() == -1);
    assert(genNode.getParent() == -1);
    assert(genNode.isLeaf());
    assert(!genNode.isVisited());
    genNode.visit();
    assert(genNode.isVisited());
    genNode.unvisit();
    assert(!genNode.isVisited());


    //
    //TEST KDTREE
    //
    kdTree testTree(dataset, m, nominal);
    vector<node> nodeList = testTree.getNodeList();
    int map[] = {2,0,1,3,4,5,8,6,7};
    int j=0;
    for(int i=0; i < nodeList.size(); i++)
        if(nodeList[i].isLeaf()){
            vector<float> tempvalues = nodeList[i].getValues();
            for(int k=0; k < tempvalues.size(); k++)
                assert(dataset[map[j]*(m+1)+k]==tempvalues[k]);
            j++;
        }
    neighbors = testTree.kNeighborSearch(observation, 2);
    assert(neighbors.size()==2);
    for (int j = 0; j < m; j++)
        assert(neighbors[0][j] == dataset[8 * (m + 1) + j]);
    for (int j = 0; j < m; j++)
        assert(neighbors[1][j] == dataset[5 * (m + 1) + j]);
    neighbors = testTree.kNeighborSearch(observation, 1);
    assert(neighbors.size()==1);
    for (int j = 0; j < m; j++)
        assert(neighbors[0][j] == dataset[8 * (m + 1) + j]);
}
