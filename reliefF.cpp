#include <stdio.h>
#include <cstdlib>
#include "node.h"
#include "kdtree.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

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
    printf("before unvisit nodes: %d %d %d\n",test.isVisited(),test.getLeft()->isVisited(),test.getRight()->isVisited());
    unvisitNodes(&test);
    printf("after unvisit nodes: %d %d %d\n",test.isVisited(),test.getLeft()->isVisited(),test.getRight()->isVisited());
    printf("\n");

    //TEST TREETOSTRING
    printf("test kdtree to string test:\n%s", treeToString(test).c_str());


    //DEFINE TEST DATASET
    int n = 8;
    vector< vector<float> > dataset(n);
    printf("dataset.size() = %d\n", dataset.size());
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < 3; j++) {
            dataset[i].push_back((float) ((i * (j + 1)) % n));
        }
    dataset[n - 1].push_back((float) 5);
    dataset[n - 1].push_back((float) 2);
    dataset[n - 1].push_back((float) 6);
    for (int i = 0; i < n; i++) {
        printf("dataset[%d] = (", i);
        for (int j = 0; j < 3; j++)
            printf("% g", dataset[i][j]);
        printf(" )\n");
    }
    printf("\n");
    /*//TEST SWAP
    printf("swap a=%g, b=%g: ", dataset[0][0], dataset[1][0]);
    swap(&dataset[0][0], &dataset[1][0]);
    printf("a=%g, b=%g\n", dataset[0][0], dataset[1][0]);*/

    //TEST FIND MEDIAN
    printf("findMedian test:\n");
    for (int i = 0; i < 3; i++)
        printf("median[%d]=%g\n", i, findMedian(dataset, i));
    printf("\n");

    //TEST KDTREE
    printf("dataset.size() = %d\n", dataset.size());
    printf("build kdtree test:\n");
    node root;
    int m = dataset[0].size();
    printf("number of attributes = %d\n", m);
    root = kdTree(dataset, m);
    printf("resulting kdtree:\n%s", treeToString(root).c_str());
    printf("\n");

    //TEST FIND k-neighbors
    printf("k-neighbors search test:\n");
    vector < float > observation;
    observation.push_back(5);
    observation.push_back(2);
    observation.push_back(8);
    int k = 2;
    vector< vector<float> > kneighbors = kNeighborSearch(&root, observation, k);
    printf("observation = (");
    for (int i = 0; i < 3; i++)
        printf("% g", observation[i]);
    printf(" )\n");
    printf("%d nearest neighbors:\n", k);
    for (int i = 0; i < kneighbors.size(); i++) {
        printf("(");
        for (int j = 0; j < 3; j++)
            printf("% g", kneighbors[i][j]);
        printf(" )\n");
    }


    return (EXIT_SUCCESS);
}
