#include <stdio.h>
#include <cstdlib>
#include "node.h"
#include "kdtree.h"
#include <fstream>
#include <list>
#include <iostream>
#include <time.h>
#include <sys/time.h>

using namespace std;

double read_timer();
void testkdtree();
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
    vector<int> nominal;
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
                nominal.push_back(1);
                value = hash(result.c_str());
                floatValues.push_back(value);
            }                //result is numeric
            else {
                nominal.push_back(0);
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
    vector<node> treeList;
    for (int i = 0; i < classDatasetList.size(); i++) {
        vector<float> classDataset = classDatasetList[i];
        cout << "class " << treeClassList[i] << " kd-tree\n";
        node temptree = kdTree(classDataset, m);
        treeList.push_back(temptree);

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
            
        //int sampleIndex = rand()%dataset.size();
        
        vector<float> observation;
        for(int j = 0; j < m+1; j++)
            observation.push_back(dataset[i*(m+1)+j]);
        

        if (observation.size() == m + 1) {

            int currClassTreeIndex = -1;
            for (int j = 0; j < treeClassList.size(); j++){
                //cout << "treeClassList " << j << " hash: " << hash(treeClassList[j].c_str()) << endl;
                if ((nominal[m] && hash(treeClassList[j].c_str()) == observation[m]) || (!nominal[m] && atof(treeClassList[j].c_str())==observation[m])) {
                    currClassTreeIndex = j;
                    break;
                }
            }
            
            //find k near hits
                    //timer -= read_timer();
            vector<float> nearHits = kNeighborSearch(&(treeList[currClassTreeIndex]), observation, k, nominal,m);
                    //timer += read_timer();            
            
            for (int j = 0; j < nearHits.size()/(m+1); j++) {
                for (int z = 0; z < m; z++) {
                    if (nominal[z] && observation[z] != nearHits[j*(m+1)+z]) {
                        attrWeight[z] -= 1;
                    } else {
                        float diff = abs(observation[z] - nearHits[j*(m+1)+z]);
                        attrWeight[z] -= diff;
                    }
                }
            }

            //find k near misses from each class
            for (int c = 0; c < treeList.size(); c++)
                if (c != currClassTreeIndex) {
                    vector<float> nearMisses = kNeighborSearch(&treeList[c], observation, k, nominal,m);
                    for (int j = 0; j < nearMisses.size()/(m+1); j++) {
                        for (int z = 0; z < m; z++) {
                            if (nominal[z] && observation[z] != nearMisses[j*(m+1)+z]) {
                                attrWeight[z] += (classProbabilityList[c] / (1 - classProbabilityList[currClassTreeIndex]));
                            } else {
                                float diff = abs(observation[z] - nearMisses[j*(m+1)+z]);
                                attrWeight[z] += (classProbabilityList[c] / (1 - classProbabilityList[currClassTreeIndex])) * diff;
                            }
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

void testkdtree() {
    node test(2, 3.5);

    //TEST TOSTRING
    printf("\nTEST TOTREE\n");
    string output = test.toString();
    printf("%s\n", output.c_str());

    //TEST VISITED    
    printf("\nTEST VISITED\n");
    test.visit();
    test.unvisit();
    if (test.isVisited())
        printf("node is visited\n");
    else
        printf("node is not visited\n");

    //TEST SETMEDIAN/SETATTRINDEX/GETMEDIAN/GETATTRINDEX
    printf("\nTEST SETMEDIAN/SETATTRINDEX/GETMEDIAN/GETATTRINDEX\n");
    test.setMedian(74.3);
    test.setAttrIndex(9);
    printf("(%d, %g) ", test.getAttrIndex(), test.getMedian());
    output = test.toString();
    printf("%s\n", output.c_str());

    //TEST GETLEFT/GETRIGHT/ISLEAF/GETPARENT/NEWNODE/DISTANCETO
    printf("\nTEST GETLEFT/GETRIGHT/ISLEAF/GETPARENT/NEWNODW/DISTANCETO\n");
    printf("Left = %d\n", test.getLeft());
    printf("Right = %d\n", test.getRight());
    printf("Leaf = %d\n", test.isLeaf());
    printf("Parent = %d\n", test.getParent());
    printf("ADDING CHILDREN:\n");
    test.addChildren();
    vector < float > leftvalues;
    leftvalues.push_back(3.2);
    leftvalues.push_back(5.9);
    leftvalues.push_back(8.4);
    vector < float > rightvalues;
    rightvalues.push_back(2.2);
    rightvalues.push_back(1.9);
    rightvalues.push_back(3.4);
    test.getLeft()->setValues(leftvalues);
    test.getRight()->setValues(rightvalues);
    printf("Left = %s, parent = %s\n", test.getLeft()->toString().c_str(), test.getLeft()->getParent()->toString().c_str());
    printf("Right = %s, parent = %s\n", test.getRight()->toString().c_str(), test.getRight()->getParent()->toString().c_str());
    printf("Leaf = %d\n", test.isLeaf());
    printf("Left values = ");
    for (int i = 0; i < test.getLeft()->getValues().size(); i++)
        printf("%f ", test.getLeft()->getValues()[i]);
    printf("\n");
    node test2(leftvalues);
    printf("new node: %s\n", test2.toString().c_str());
    printf("%s distance to %s = %f\n", test.getLeft()->toString().c_str(), test.getRight()->toString().c_str(), test.getLeft()->distanceTo(test.getRight()));

    //TEST UNVISITNODES
    printf("unvisit nodes test:\n");
    test.visit();
    test.getLeft()->visit();
    test.getRight()->visit();
    printf("before unvisit nodes: %d %d %d\n", test.isVisited(), test.getLeft()->isVisited(), test.getRight()->isVisited());
    unvisitNodes(&test);
    printf("after unvisit nodes: %d %d %d\n", test.isVisited(), test.getLeft()->isVisited(), test.getRight()->isVisited());
    printf("\n");

    //TEST TREETOSTRING
    printf("test kdtree to string test:\n%s", treeToString(test).c_str());


    //DEFINE TEST DATASET
    int n = 8, m = 3;
    vector<float> dataset;
    printf("dataset.size() = %d\n", dataset.size());
    cout << "number of records = " << dataset.size()/(m+1) << endl;
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < m+1; j++) {
            dataset.push_back((float) ((i * (j + 1)) % n));
        }
    dataset.push_back((float) 5);
    dataset.push_back((float) 2);
    dataset.push_back((float) 6);
    dataset.push_back((float) 4);
    for (int i = 0; i < n; i++) {
        printf("dataset[%d] = (", i);
        for (int j = 0; j < m+1; j++)
            printf("% g", dataset[i*(m+1)+j]);
        printf(" )\n");
    }
    printf("\n");
    /*//TEST SWAP
    printf("swap a=%g, b=%g: ", dataset[0][0], dataset[1][0]);
    swap(&dataset[0][0], &dataset[1][0]);
    printf("a=%g, b=%g\n", dataset[0][0], dataset[1][0]);*/

    //TEST FIND MEDIAN
    printf("findMedian test:\n");
    for (int i = 0; i < m; i++)
        printf("median[%d]=%g\n", i, findMedian(dataset, i, m));
    printf("\n");

    //TEST KDTREE
    printf("dataset.size() = %d\n", dataset.size());
    printf("build kdtree test:\n");
    node root;
    printf("number of attributes = %d\n", m);
    root = kdTree(dataset, m);
    printf("resulting kdtree:\n%s", treeToString(root).c_str());
    printf("\n");

    //TEST FIND k-neighbors
    printf("k-neighbors search test:\n");
    vector < float > observation;
    observation.push_back(5);
    observation.push_back(2);
    observation.push_back(6.5);
    observation.push_back(0);
    vector <int> nominal;
    nominal.push_back(0); nominal.push_back(0);
    int k = 2;
    vector<float> kneighbors = kNeighborSearch(&root, observation, k, nominal, m);
    printf("observation = (");
    for (int i = 0; i < m+1; i++)
        printf("% g", observation[i]);
    printf(" )\n");
    printf("%d nearest neighbors:\n", k);
    for (int i = 0; i < kneighbors.size()/(m+1); i++) {
        printf("(");
        for (int j = 0; j < m+1; j++)
            printf("% g", kneighbors[i*(m+1)+j]);
        printf(" )\n");
    }
}
