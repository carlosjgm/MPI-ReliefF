#include <stdio.h>
#include <cstdlib>
#include "node.h"

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
    leftvalues.push_back(3.2); leftvalues.push_back(5.9); leftvalues.push_back(8.4);    
    vector < float > rightvalues;
    rightvalues.push_back(2.2); rightvalues.push_back(1.9); rightvalues.push_back(3.4);  
    test.getLeft()->setValues(leftvalues);
    test.getRight()->setValues(rightvalues);
    printf("Left = %s, parent = %s\n", test.getLeft()->toString().c_str(), test.getLeft()->getParent()->toString().c_str());
    printf("Right = %s, parent = %s\n", test.getRight()->toString().c_str(), test.getRight()->getParent()->toString().c_str());
    printf("Leaf = %d\n", test.isLeaf());
    printf("Left values = ");
    for(int i=0; i < test.getLeft()->getValues().size(); i++)
        printf("%f ",test.getLeft()->getValues()[i]);
    printf("\n");
    node test2(leftvalues);
    printf("new node: %s\n", test2.toString().c_str());
    printf("%s distance to %s = %f",test.getLeft()->toString().c_str(),test.getRight()->toString().c_str(),test.getLeft()->distanceTo(test.getRight()));

    return (EXIT_SUCCESS);
}

