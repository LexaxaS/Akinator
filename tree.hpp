#ifndef TREE_HPP
#define TREE_HPP 

#include "tools.hpp"

// typedef int treeElem_t;
// #define treeElemPr "%d"
// const treeElem_t POISON_TREE = 123456789;

typedef char* treeElem_t;
#define treeElemPr "%s"

const treeElem_t POISON_TREE = "123456789";
const size_t ZEROID = 0;

const size_t MAX_TREE_LEN = 128;
const size_t PATH_MAX_LEN = 128;
const size_t CMD_MAX_LEN = 256;

#define TREE_DOT_FOLDER "TREEDOT"
#define TREE_PNG_FOLDER "TREEIMG"

#define TREE_SEPARATOR ';'
#define TREE_TERMINATOR ' '

struct TreeNode
    {
    treeElem_t value;

    size_t id;
    size_t subtreeLen;

    TreeNode* parent;
    TreeNode* right;
    TreeNode* left;
    };

struct Tree
    {
    TreeNode* root;
    size_t* size;
    };

struct TreeNodewErr
    {
    TreeNode* node;
    error_t error;
    };

struct TreewErr
    {
    Tree* tree;
    error_t error;
    };
    
enum TREE_ERRORS
    {
    ERR_TREE_LOOP = 1,
    ERR_TREE_SYNTAX = 2,
    };

error_t TreeInit(Tree* tree, TreeNode* root);

TreeNodewErr TreeNodeNew(treeElem_t value, TreeNode* left, TreeNode* right);
TreewErr TreeRead(char* filename);
error_t TreePrintInFile(Tree* tree, char* filename);

error_t TreeAddLeft(TreeNode* dest, TreeNode* node);
error_t TreeAddRight(TreeNode* dest, TreeNode* node);

error_t TreeVerify(Tree* tree);
error_t NodeDestruct(TreeNode* node);
error_t TreeDestruct(Tree* tree);

error_t RecountNotes(Tree* tree);
error_t TreeGraphicDump(Tree* tree);

#endif