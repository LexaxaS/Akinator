#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "tree.hpp"

static size_t NODE_ID = 1;
static size_t GRAPH_ID = 1;

static error_t _updateSubtreeLen(TreeNode* node, ssize_t delta);
static TreeNodewErr _recTreeRead(Text* text, size_t* elnum);
static TreeNodewErr _recOpenBracket(Text* text, size_t* elnum);
static error_t _TreeBufReader(Text* text);
static error_t _recTreePrint(TreeNode* node, FILE* filedest);


TreewErr TreeRead(char* filename)
    {
    Tree* tree = (Tree*) calloc(1, sizeof(Tree));
    Text text = setbuf(filename);
    _TreeBufReader(&text);
    for (size_t i = 0; i < text.nLines; i++)
        printf("%s\n", text.lines[i].linePtr);
    printf("huy");
    size_t elnum = 0;
    TreeNodewErr newTree = _recTreeRead(&text, &elnum);
    if (newTree.error != NO_ERROR)
        {
        return {nullptr, newTree.error};
        }
    tree->root = newTree.node;
    RecountNotes(tree);
    tree->size = &newTree.node->subtreeLen;
    return {tree, NO_ERROR};
    }

static TreeNodewErr _recTreeRead(Text* text, size_t* elnum)
    {
    if (*elnum > text->nLines)
        {
        printf("maxb\n");
        return {nullptr, ERR_TOO_BIG_IND};
        }

    char* string = text->lines[(*elnum)++].linePtr;

    const char* openBracket = strchr(string, '(');
    if (openBracket)
        {
        return _recOpenBracket(text, elnum);   
        }

    const char* nil = strstr(string, "nil");
    if (nil)
        {
        return {nullptr, NO_ERROR};
        }

    return {nullptr, ERR_TREE_SYNTAX};
    }

static TreeNodewErr _recOpenBracket(Text* text, size_t* elnum)
    {
    if (*elnum > text->nLines)
        {
        ERR_PRINT();
        return {nullptr, ERR_TOO_BIG_IND};
        }

    const char* string = text->lines[(*elnum)++].linePtr;
    treeElem_t value = {};
    if (treeElemPr == "%s")
        {
        value = (treeElem_t) calloc(text->lines[*elnum - 1].length + 1, sizeof(*value));
        size_t scanlen = 0;
        if (sscanf(string, treeElemPr "%n", value, &scanlen) != 1)
            {
            ERR_PRINT();
            return {nullptr, ERR_TREE_SYNTAX};
            }
        }
    else
        {
        value = POISON_TREE;
        size_t scanlen = 0;
        if (sscanf(string, treeElemPr "%n", &value, &scanlen) != 1)
            {
            ERR_PRINT();
            return {nullptr, ERR_TREE_SYNTAX};
            }
        }
        

    TreeNodewErr leftNodeE = _recTreeRead(text, elnum);
    if (leftNodeE.error != NO_ERROR)
        {
        printf("Err %d in line %d in file %s\n", leftNodeE.error, __LINE__, __FILE__);
        return {nullptr, leftNodeE.error};
        }

    TreeNodewErr rightNodeE = _recTreeRead(text, elnum);
    if (rightNodeE.error != NO_ERROR)
        {
        printf("Err in line %d in file %s\n", __LINE__, __FILE__);
        return {nullptr, rightNodeE.error};
        }

    TreeNodewErr nodeE = TreeNodeNew(value, leftNodeE.node, rightNodeE.node);

    if (nodeE.error != NO_ERROR)
        {
        printf("Err in line %d in file %s\n", __LINE__, __FILE__);
        return {nullptr, nodeE.error};
        }

    const char* closeBracket = strchr(text->lines[(*elnum)++].linePtr, ')');
    if (!closeBracket)
        return {nullptr, ERR_TREE_SYNTAX};

    return {nodeE.node, NO_ERROR};
    }

static error_t _TreeBufReader(Text* text)
    {
    char* buf = text->bufPtr;
    size_t flen = text->size;
    size_t buf_i = 0;
    size_t ptr_j = 1;

    String* pointers = (String*) calloc(flen + 1, sizeof(*pointers));

    size_t lineLen = 0;

    pointers[0].linePtr = &buf[0];
    printf("hren = %s\n", buf);
    bool isstring = false;

    while (buf_i < flen)
        {
        if (buf[buf_i] == TREE_TERMINATOR && !isstring)
            {
            if (lineLen == 0)
                {
                buf[buf_i++] = '\0';
                pointers[ptr_j - 1].linePtr = &(buf[buf_i]);
                }

            else
                {
                buf[buf_i++] = '\0';
                pointers[ptr_j - 1].length = lineLen;
                printf("poi = %s\n", pointers[ptr_j - 1]);
                lineLen = 0;
                pointers[ptr_j++].linePtr = &(buf[buf_i]);
                }
            }
        else if (buf[buf_i] == ';' && !isstring)
            {
            printf("true\n");
            isstring = true;
            buf_i++;
            lineLen++;
            }
        else if (buf[buf_i] == ';' && isstring)
            {
            printf("false\n");
            isstring = false;
            buf_i++;
            lineLen++;
            }
        else
            {
            buf_i++;
            lineLen++;
            }
        }

    pointers[ptr_j].linePtr = 0;
    text->lines = pointers;
    text->nLines = ptr_j;
    return NO_ERROR;
    }
    
error_t TreeVerify(Tree* tree)
    {
    printf("huy");
    return NO_ERROR;
    }

error_t TreePrintInFile(Tree* tree, char* filename)
    {
    MY_ASSERT_SOFT(tree, ERR_NULL_PTR);
    MY_ASSERT_SOFT(filename, ERR_NULL_PTR);

    FILE* filedest = fileopenerW(filename);
    _recTreePrint(tree->root, filedest);
    fclose(filedest);

    return NO_ERROR;
    }

static error_t _recTreePrint(TreeNode* node, FILE* filedest)
    {
    if (!node)
        {
        fprintf(filedest, "nil%c", TREE_TERMINATOR);
        return NO_ERROR;
        }

    fprintf(filedest, "(%c"treeElemPr"%c", TREE_TERMINATOR, node->value, TREE_TERMINATOR);
    _recTreePrint(node->left, filedest);
    _recTreePrint(node->right, filedest);
    fprintf(filedest, ")%c", TREE_TERMINATOR);

    return NO_ERROR;    
    }

error_t TreeInit(Tree* tree, TreeNode* root)
    {
    MY_ASSERT_SOFT(root, ERR_NULL_PTR);

    tree->root = root;
    tree->size = &root->subtreeLen;

    return NO_ERROR;
    }

TreeNodewErr TreeNodeNew(treeElem_t value, TreeNode* left, TreeNode* right)
    {
    TreeNode* node = (TreeNode*) calloc(1, sizeof(TreeNode));

    if (!node)
        return {nullptr, ERR_ALLOC_ERROR};

    node->id = NODE_ID++;

    if (left)
        left->parent = node;
    node->left = left;

    if (right)
        right->parent = node;
    node->right = right;

    node->parent = nullptr;

    node->value = value;

    // printf("val = %s\n", value);

    return {node, NO_ERROR};
    }

error_t TreeAddLeft(TreeNode* dest, TreeNode* node)
    {
    MY_ASSERT_HARD(dest);
    MY_ASSERT_HARD(node);

    dest->left = node;
    node->parent = dest;

    return _updateSubtreeLen(dest, node->subtreeLen);
    }

error_t TreeAddRight(TreeNode* dest, TreeNode* node)
    {
    MY_ASSERT_HARD(dest);
    MY_ASSERT_HARD(node);

    dest->right = node;
    node->parent = dest;

    return _updateSubtreeLen(dest, node->subtreeLen);
    }

static error_t _recNodeDestruct(TreeNode* node)
    {
    MY_ASSERT_HARD(node);

    if (node->id == ZEROID)
        RETURN_PRINT_ERROR(ERR_TREE_LOOP);
    node->id = 0;

    if (node->left)
        {
        if (node->left->parent != node)
            RETURN_PRINT_ERROR(ERR_TREE_LOOP);
        _recNodeDestruct(node->left);
        }

    if (node->right)
        {
        if (node->right->parent != node)
            RETURN_PRINT_ERROR(ERR_TREE_LOOP);
        _recNodeDestruct(node->right);
        }
    
    node->value = POISON_TREE;
    node->subtreeLen = 0;

    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;

    free(node);
    return NO_ERROR;
    }

error_t NodeDestruct(TreeNode* node)
    {
    MY_ASSERT_HARD(node);

    if (node->parent)
        {
        if (node->parent->left == node)
            {
            if (node->parent->right == node)
                RETURN_PRINT_ERROR(ERR_TREE_LOOP);
            node->parent->left = nullptr;
            }
        else if (node->parent->right == node)
            node->parent->right = nullptr;
        else
            RETURN_PRINT_ERROR(ERR_TREE_LOOP);
        _updateSubtreeLen(node->parent, - (ssize_t) node->subtreeLen);
        }

    _recNodeDestruct(node);
    }

error_t TreeDestruct(Tree* tree)
    {
    MY_ASSERT_HARD(tree);

    _recNodeDestruct(tree->root);

    return NO_ERROR;
    }

static error_t _updateSubtreeLen(TreeNode* node, ssize_t delta)
    {
    MY_ASSERT_HARD(node);

    if (node->id == ZEROID)
        RETURN_PRINT_ERROR(ERR_TREE_LOOP);
    
    size_t nodeid = node->id;
    node->id = ZEROID;

    node->subtreeLen += delta;

    if (node->parent)
        {
        if (node->parent->right != node && node->parent->left != node)
            RETURN_PRINT_ERROR(ERR_TREE_LOOP);
        _updateSubtreeLen(node->parent, delta);
        }
    
    node->id = nodeid;

    return NO_ERROR;
    }

static error_t _recRecountNotes(TreeNode* node)
    {
    MY_ASSERT_SOFT(node, ERR_NULL_PTR);

    if (node->id == ZEROID)
        RETURN_PRINT_ERROR(ERR_TREE_LOOP);
    

    node->subtreeLen = 1;

    if (!node->left && !node->right)
        return NO_ERROR;
        
    size_t nodeid = node->id;
    node->id = ZEROID;
    
    if (node->left)
        {
        RETURN_ERROR(_recRecountNotes(node->left));
        node->subtreeLen += node->left->subtreeLen;
        }
    
    if (node->right)
        {
        RETURN_ERROR(_recRecountNotes(node->right));
        node->subtreeLen += node->right->subtreeLen;
        }

    node->id = nodeid;

    return NO_ERROR;
    }

error_t RecountNotes(Tree* tree)
    {
    MY_ASSERT_SOFT(tree, ERR_NULL_PTR);

    return _recRecountNotes(tree->root);
    }
    
#define FONT_SIZE "10"
#define FONT_NAME "\"Sans Bold Not-Rotated\""
#define BACK_GROUND_COLOR "\"#5e67d4\""
#define NODE_COLOR "\"#faa116\""
#define NODE_FRAME_COLOR "\"#000000\""
#define ROOT_COLOR "\"#c25a90\""
#define FREE_COLOR "\"#b9e793\""
#define TREE_COLOR "\"#007be9\""

static error_t _TreeGraphBuild(TreeNode* node, FILE* outDotFile, size_t curNum, size_t maxNum)
    {
    MY_ASSERT_HARD(outDotFile);

    if (!node)
        return NO_ERROR;

    if (curNum > maxNum)
        {
        printf("cur > max build\n");
        return NO_ERROR;
        }

    fprintf(outDotFile, "NODE_%zu[style = \"filled\", fillcolor = " NODE_COLOR ", ", node->id);
    if (node->value == POISON_TREE)
        fprintf(outDotFile, "label = \"{Value:\\nPOISON|{<left>Left|<right>Right}}\"];\n");
    else
        fprintf(outDotFile, "label = \"{Value:\\n" treeElemPr "|{<left>Left|<right>Right}}\"];\n", node->value);
    
    _TreeGraphBuild(node->left, outDotFile, curNum++, maxNum);
    _TreeGraphBuild(node->right, outDotFile, curNum++, maxNum);

    return NO_ERROR;
    }

static error_t _TreeGraphDraw(TreeNode* node, FILE* outDotFile, size_t curNum, size_t maxNum)
    {
    MY_ASSERT_HARD(outDotFile);

    if (curNum > maxNum)
        {
        printf("cur = %d > max draw = %d, line %d\n", curNum, maxNum, __LINE__);
        return NO_ERROR;
        }

    if (!node)
        return NO_ERROR;

    if (node->left)
        fprintf(outDotFile, "NODE_%zu:left->NODE_%zu;\n", node->id, node->left->id);
    if (node->right)
        fprintf(outDotFile, "NODE_%zu:right->NODE_%zu;\n", node->id, node->right->id);

    _TreeGraphDraw(node->left, outDotFile, curNum++, maxNum);
    _TreeGraphDraw(node->right, outDotFile, curNum++, maxNum);

    return NO_ERROR;
    }

error_t TreeGraphicDump(Tree* tree)
    {
    MY_ASSERT_HARD(tree);

    RecountNotes(tree);
    printf("size = %d\n", *tree->size);

    char outFilePath[PATH_MAX_LEN] = "";
    sprintf(outFilePath, "%s/treegraph%zu.dot", TREE_DOT_FOLDER, GRAPH_ID);

    FILE* outDotFile = fileopenerW(outFilePath);

    fprintf(outDotFile,
        "digraph\n"
        "{\n"
        "rankdir = TB;\n"
        "node[shape = record, color = " NODE_FRAME_COLOR ", fontname = " FONT_NAME ", fontsize = " FONT_SIZE "];\n"
        "bgcolor = " BACK_GROUND_COLOR ";\n");
    fprintf(outDotFile, 
        "TREE[rank = \"min\", style = \"filled\", fillcolor = " TREE_COLOR ", "
        "label = \"{Tree|Error: %d|Size: %zu|<root>Root}\"];\n",
        TreeVerify(tree), *(tree->size));

    fprintf(outDotFile, "NODE_%zu[style = \"filled\", fillcolor = " NODE_COLOR ", ", tree->root->id);

    if (tree->root->value == POISON_TREE)
        fprintf(outDotFile, "label = \"{Value:\\nPOISON|{<left>Left|<right>Right}}\"];\n");
    else
        fprintf(outDotFile, "label = \"{Value:\\n" treeElemPr "|{<left>Left|<right>Right}}\"];\n", tree->root->value);

    size_t maxNum = min(*tree->size, MAX_TREE_LEN);
    _TreeGraphBuild(tree->root->left, outDotFile, 1, maxNum);
    _TreeGraphBuild(tree->root->right, outDotFile, 1, maxNum);

    _TreeGraphDraw(tree->root, outDotFile, 1, maxNum);

    fprintf(outDotFile, "\n");
    fprintf(outDotFile, "TREE:root->NODE_%zu\n", tree->root->id);

    fprintf(outDotFile, "}\n");
    char command[CMD_MAX_LEN] = "";
    sprintf(command, "dot %s -Tpng -o %s/TreeGraph%zu.png", outFilePath, TREE_PNG_FOLDER, GRAPH_ID);
    printf("\n%s\n", command);

    fclose(outDotFile);

    system(command);

    GRAPH_ID++;

    return NO_ERROR;
    }

#undef FONT_SIZE "10"
#undef FONT_NAME "\"Sans Bold Not-Rotated\""
#undef BACK_GROUND_COLOR "\"#5e67d4\""
#undef NODE_COLOR "\"#faa116\""
#undef NODE_FRAME_COLOR "\"#000000\""
#undef ROOT_COLOR "\"#c25a90\""
#undef FREE_COLOR "\"#b9e793\""