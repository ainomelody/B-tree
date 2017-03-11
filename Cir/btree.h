#ifndef BTREE_H
#define BTREE_H

#include<vector>

template<typename Item>
class B_tree
{
private:
    struct BTNode {
        int keynum;
        BTNode *parent;
        Item *key;
        BTNode **ptr;
    };
    struct Result{
        BTNode *pt;
        int i;
        bool tag;
    };

    BTNode *root;
    int n;

    Result searchBTree(const Item & toSearch);
    int indexInParent(BTNode *node);
    BTNode *leftSibling(BTNode *node);
    BTNode *rightSibling(BTNode *node);
    BTNode *newNode();
    void deleteNode(BTNode *node);

public:
    B_tree(const int order = 4);
    //virtual ~B_tree();

    bool insertItem(const Item &toInsert);
    bool deleteItem(const Item &toDelete);
    //Result search(const Item &k);
    std::vector<Item> RangeSerach(const Item& LBound, const Item& UBound);
};
#endif
