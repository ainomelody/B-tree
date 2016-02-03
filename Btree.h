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

#include "Btree.h"
#include <cstring>

using namespace std;

template<typename Item>
int B_tree<Item>::indexInParent(BTNode *node)
{
	BTNode *parent = node->parent;
	if (parent == NULL)
		return -1;					//根节点
	
	for (int i = 0; i <= parent->keynum; i++)
		if (parent->ptr[i] == node)
			return i;
	
	return -2;						//错误，可能parent设置产生问题
}

template<typename Item>
typename B_tree<Item>::BTNode * B_tree<Item>::leftSibling(BTNode *node)
{
	int i = indexInParent(node);
	if (i < 1)
		return NULL;
	
	return node->parent->ptr[i - 1];
}

template<typename Item>
typename B_tree<Item>::BTNode * B_tree<Item>::rightSibling(BTNode *node)
{
	int i = indexInParent(node);
	
	if (i < 0 || i == node->parent->keynum)
		return NULL;
	
	return node->parent->ptr[i + 1];
}

template<typename Item>
bool B_tree<Item>::deleteItem(const Item &toDelete)
{
	Result delLoc = searchBTree(toDelete);
	BTNode *moveptr;
	
	if (!delLoc.tag)
		return false;
	
	/*将要删除元素右边子树的最小值移到此位置,改为删除此最小元素*/
	moveptr = delLoc.pt->ptr[delLoc.i];
	while (moveptr != NULL && moveptr->ptr[0] != NULL)
		moveptr = moveptr->ptr[0];
	
	if (moveptr != NULL)
	{
		delLoc.pt->key[delLoc.i] = moveptr.key[1];
		delLoc.pt = moveptr;
		delLoc.i = 1;
	}
	
	bool finished = false;
	int minKeyNum = (n + 1) / 2 - 1;
	
	while (!finished)
	{
		memmove(delLoc.pt->key + delLoc.i, delLoc.pt->key + delLoc.i + 1, sizeof(Item) * (delLoc.pt->keynum - delLoc.i));	//修改关键字的顺序表
		
		if (delLoc.pt == root && delLoc.pt->keynum > 2 ||
			delLoc.pt->keynum > minKeyNum)					//情况1，删除完成	
			{
				delLoc.pt->keynum--;
				finished = true;
			}
		else
		{
			BTNode *lsib = leftSibling(delLoc.pt);
			BTNode *rsib = rightSibling(delLoc.pt);
			int index = indexInParent(delLoc.pt);
			
			if (lsib != NULL && lsib->keynum > minKeyNum)	//情况2，移动左兄弟中最大关键字
			{
				Item &keyInParent = delLoc.pt->parent->key[index];
				
				delLoc.pt->key[delLoc.i] = keyInParent;
				keyInParent = lsib->key[lsib->keynum];
				delLoc.pt = lsib;
				delLoc.i = lsib->keynum;
			}
			else if (rsib != NULL && rsib->keynum > minKeyNum)	//情况2，移动右兄弟中最小关键字
			{
				Item &keyInParent = delLoc.pt->parent->key[index + 1];
				
				delLoc.pt->key[delLoc.i] = keyInParent;
				keyInParent = rsib->key[1];
				delLoc.pt = rsib;
				delLoc.i = 1;
			}
			else							//情况3：合并结点
			{
				delLoc.pt->keynum--;
				
				if (lsib != NULL)			//合并进左兄弟
				{
					lsib->key[++lsib->keynum] = lsib->parent->key[index];
					for (int i = 1; i <= delLoc.pt->keynum; i++)			//复制剩余关键字
						lsib->key[lsib->keynum + i] = delLoc.pt->key[i];
					lsib->keynum += delLoc.pt->keynum;
					
					for (int i = 0; i <= delLoc.pt->keynum; i++)			//复制剩余子树指针
						lsib->ptr[lsib->keynum + i] = delLoc.pt->ptr[i];
					
					deleteNode(delLoc.pt);
					memmove(lsib->parent->ptr + index, lsib->parent->ptr + index + 1, sizeof(BTNode *) *(lsib->parent->keynum - index));
					lsib->parent->ptr[lsib->parent->keynum] = NULL;
					
					delLoc.pt = lsib->parent;								//对父结点中的关键字执行删除操作
					delLoc.i = index;
				}
				else if (rsib != NULL)
				{
					memmove(rsib->key + 1 + delLoc.pt->keynum, rsib->key, sizeof(Item) * (delLoc.pt + 1));
					rsib->keynum += delLoc.pt->keynum + 1;
					memmove(rsib->ptr + 1 + delLoc.pt, rsib->ptr, sizeof(BTNode *) * (delLoc.pt + 1));
					
					for (int i = 1; i <= delLoc.pt->keynum; i++)	//复制剩余关键字
						rsib->key[i] = delLoc.pt->key[i];
					rsib->key[delLoc.pt->keynum + 1] = rsib->parent->key[index + 1];	//复制父结点关键字
					
					for (int i = 0; i <= delLoc.pt->keynum; i++)
						rsib->ptr[i] = delLoc.pt->ptr[i];
					
					deleteNode(delLoc.pt);
					memmove(rsib->parent->ptr + index, rsib->parent->ptr + index + 1, sizeof(BTNode *) *(rsib->parent->keynum - index));
					rsib->parent->ptr[rsib->parent->keynum] = NULL;
					
					delLoc.pt = rsib->parent;
					delLoc.i = index + 1;
				}
				else						//剩下空根结点+一个子树，更新根结点
				{
					BTNode *node = root->ptr[0];
					deleteNode(root);
					root = node;
					finished = true;
				}
			}
		}
	}
	
	return true;
}

template<typename Item>
typename B_tree<Item>::BTNode * B_tree<Item>::newNode()
{
	BTNode *node = new BTNode;
	
	node->parent = NULL;
	node->keynum = 0;
	node->key = new Item[n + 1];
	node->ptr = new BTNode * [n + 1];
	memset(node->ptr, 0, sizeof(BTNode *) * (n + 1));	//将全部指针设置为NULL
	
	return node;
}

template<typename Item>
B_tree<Item>::B_tree(const int order)
{
	n = order;
	root = newNode();				//一个key数为0的结点表示空树
}

template<typename Item>
typename B_tree<Item>::Result B_tree<Item>::searchBTree(const Item & toSearch)
{
	BTNode *q = NULL, *p = root;
	bool found = false;
	int i;
	
	while (p != NULL && !found)
	{
		i = 1;
		while (i <= p->keynum && p->key[i] < toSearch)
			i++;
		if (i <= p->keynum && p->keynum[i] == toSearch)
			found = true;
		else
		{
			q = p;
			p = p->ptr[i - 1];
		}
		
		if (found)
			return Result(p, i, true);
		
		return Result(q, i, false);
	}
}

template<typename Item>
bool B_tree<Item>::insertItem(const Item &toInsert)
{
	Item x = toInsert;
	BTNode *ap = NULL;
	bool finished = false;
	Result insLoc = searchBTree(toInsert);
	
	if (insLoc.tag)
		return false;
	
	while (!finished && insLoc.pt != NULL)
	{
		/*移动顺序表，插入关键字和指针*/
		memmove(insLoc.pt->key + insLoc.i, insLoc.pt->key + insLoc.i + 1, sizeof(Item) * (insLoc.pt->keynum + 1 - insLoc.i));
		memmove(insLoc.pt->ptr + insLoc.i, insLoc.pt->ptr + insLoc.i + 1, sizeof(BTNode *) * (insLoc.pt->keynum + 1 - insLoc.i));
		insLoc.pt->key[insLoc.i] = x;
		insLoc.pt->ptr[insLoc.i] = ap;
		insLoc.pt->keynum++;
		
		if (insLoc.pt->keynum < n)
			finished = true;
		else
		{
			int mid = (n + 1) / 2;
			
			/*形成新结点*/
			ap = newNode();
			for (int i = mid + 1; i <= insLoc.pt->keynum; i++)
			{
				ap->key[i - mid] = insLoc.pt->key[i];
				ap->ptr[i - mid] = insLoc.pt->ptr[i];
			}
			ap->ptr[0] = insLoc.pt->ptr[mid];
			ap->parent = insLoc.pt->parent;
			ap->keynum = insLoc.pt->keynum - mid;
			
			x = insLoc.pt->key[mid];
			insLoc.pt->keynum = mid - 1;
			insLoc.pt = insLoc.pt->parent;
			insLoc.i = 1;
			if (insLoc.pt != NULL)
				while (insLoc.i <= insLoc.pt->keynum && insLoc.pt->key[insLoc.i] < x)
					insLoc.i++;
		}
	}
	if (!finished)
	{
		BTNode *newRoot = newNode();
		root->parent = newRoot;
		ap->parent = newRoot;
		newRoot->keynum = 1;
		newRoot->key[1] = x;
		newRoot->ptr[0] = insLoc.pt;
		newRoot->ptr[1] = ap;
		root = newRoot;
	}
	return true;
}

template <typename Item>
void B_tree<Item>::deleteNode(BTNode *node)
{
	delete []node->key;
	delete []node->ptr;
	delete node;
}

#endif