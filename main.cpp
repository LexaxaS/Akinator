#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tree.hpp"
#include "akinator.hpp"

int main()
    {
    Tree* tree1 = TreeRead("testtree2.txt").tree;

    // mainakin("data1.txt");

    TreeGraphicDump(tree1);
    TreePrintInFile(tree1, "data1.txt");
    return 0;
    }