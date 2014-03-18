#include <stdio.h>
#include <cstdlib>
#include "node.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    printf("build kdtree\n");
    
    node test (2, 3.5);
    
    string output = test.toString();
    
    printf("%s\n",output.c_str());

    return (EXIT_SUCCESS);
}

