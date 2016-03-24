#ifndef BTREE_H
#define BTREE_H

#include <set>
#include <cstring>
#include <queue>
#include "thread.h"
#include <QFile>
#include <QMessageBox>
#include <QTime>
#include <stack>

class paintWidget;
template<typename Item>
class B_tree
{
private:
	struct BTNode {
		int keynum;
		BTNode *parent;
		Item *key;
		BTNode **ptr;
        int X;              //结点左边缘横坐标，绘图用
	};
	struct Result{
		BTNode *pt;
		int i;
		bool tag;
	};
    WorkThread *work;
	BTNode *root;
	int n;
    int levelNum;           //层数，绘图用
	
	Result searchBTree(const Item & toSearch);
	int indexInParent(BTNode *node);
	BTNode *leftSibling(BTNode *node);
	BTNode *rightSibling(BTNode *node);
	BTNode *newNode();
	void deleteNode(BTNode *node);
    int nodeLevel(BTNode *node);
    void readNode(BTNode *node, QFile *file);
    long time;
    friend class paintWidget;

public:
	B_tree(const int order = 4);
	~B_tree();
    WorkThread* getThread();
	bool insertItem(const Item &toInsert);
	bool deleteItem(const Item &toDelete);
    bool search(const Item &toSearch);
    std::set<Item> *rangeSerach(const Item& LBound, const Item& UBound);
    int getLevelNum();
    void saveToFile(QFile *file);
    bool isEmpty();
    void loadFromFile(QFile *file);
    long getTime();
};

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
WorkThread* B_tree<Item>::getThread()
{
    return work;
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
	BTNode *moveptr;
    QTime count;
    count.start();
    Result delLoc = searchBTree(toDelete);

	if (!delLoc.tag)
		return false;
	
    time = 0;

    /*将要删除元素右边子树的最小值移到此位置,改为删除此最小元素*/
    moveptr = delLoc.pt->ptr[delLoc.i];
    while (moveptr != NULL && moveptr->ptr[0] != NULL)
        moveptr = moveptr->ptr[0];

    if (moveptr != NULL)
    {
        delLoc.pt->key[delLoc.i] = moveptr->key[1];
        delLoc.pt = moveptr;
        delLoc.i = 1;
    }
	
	bool finished = false;
	int minKeyNum = (n + 1) / 2 - 1;
	
	while (!finished)
    {
		std::memmove(delLoc.pt->key + delLoc.i, delLoc.pt->key + delLoc.i + 1, sizeof(Item) * (delLoc.pt->keynum - delLoc.i));	//修改关键字的顺序表
        delLoc.pt->keynum--;

        time += count.elapsed();
        work->run();
        count.restart();

        if (delLoc.pt == root && (delLoc.pt->keynum > 0 || delLoc.pt->ptr[0] == NULL)
            || delLoc.pt->keynum >= minKeyNum)					//情况1，删除完成
				finished = true;
		else
		{
			BTNode *lsib = leftSibling(delLoc.pt);
			BTNode *rsib = rightSibling(delLoc.pt);
			int index = indexInParent(delLoc.pt);
			
			if (lsib != NULL && lsib->keynum > minKeyNum)	//情况2，移动左兄弟中最大关键字
			{
				Item &keyInParent = delLoc.pt->parent->key[index];
				
                delLoc.pt->keynum++;
				delLoc.pt->key[delLoc.i] = keyInParent;
				keyInParent = lsib->key[lsib->keynum];

                BTNode * &ptrToInsert = delLoc.pt->ptr[0];
                std::memmove(delLoc.pt->ptr + 1, delLoc.pt->ptr, sizeof(BTNode *) * (delLoc.pt->keynum));
                ptrToInsert = lsib->ptr[lsib->keynum];
                if (ptrToInsert)
                    ptrToInsert->parent = delLoc.pt;
                lsib->ptr[lsib->keynum] = NULL;

                delLoc.pt = lsib;
                delLoc.i = lsib->keynum;
			}
			else if (rsib != NULL && rsib->keynum > minKeyNum)	//情况2，移动右兄弟中最小关键字
			{
				Item &keyInParent = delLoc.pt->parent->key[index + 1];
				
                delLoc.pt->keynum++;
				delLoc.pt->key[delLoc.i] = keyInParent;
				keyInParent = rsib->key[1];

                BTNode * &ptrToInsert = delLoc.pt->ptr[delLoc.pt->keynum];
                ptrToInsert = rsib->ptr[0];
                if (ptrToInsert)
                    ptrToInsert->parent = delLoc.pt;

                std::memmove(rsib->ptr, rsib->ptr + 1, sizeof(BTNode *) * rsib->keynum);
                rsib->ptr[rsib->keynum] = NULL;

                delLoc.pt = rsib;
                delLoc.i = 1;
			}
			else							//情况3：合并结点
			{				
				if (lsib != NULL)			//合并进左兄弟
				{
					lsib->key[++lsib->keynum] = lsib->parent->key[index];
					for (int i = 1; i <= delLoc.pt->keynum; i++)			//复制剩余关键字
						lsib->key[lsib->keynum + i] = delLoc.pt->key[i];
					
					for (int i = 0; i <= delLoc.pt->keynum; i++)			//复制剩余子树指针
						if (lsib->ptr[lsib->keynum + i] = delLoc.pt->ptr[i])
							lsib->ptr[lsib->keynum + i]->parent = lsib;
					
					std::memmove(lsib->parent->ptr + index, lsib->parent->ptr + index + 1, sizeof(BTNode *) *(lsib->parent->keynum - index));
					lsib->parent->ptr[lsib->parent->keynum] = NULL;
                    lsib->keynum += delLoc.pt->keynum;
                    deleteNode(delLoc.pt);
					
					delLoc.pt = lsib->parent;								//对父结点中的关键字执行删除操作
					delLoc.i = index;
				}
				else if (rsib != NULL)
				{
                    std::memmove(rsib->key + 2 + delLoc.pt->keynum, rsib->key + 1, sizeof(Item) * rsib->keynum);
					rsib->keynum += delLoc.pt->keynum + 1;
                    std::memmove(rsib->ptr + 1 + delLoc.pt->keynum, rsib->ptr, sizeof(BTNode *) * (rsib->keynum + 1));
					
					for (int i = 1; i <= delLoc.pt->keynum; i++)	//复制剩余关键字
						rsib->key[i] = delLoc.pt->key[i];
					rsib->key[delLoc.pt->keynum + 1] = rsib->parent->key[index + 1];	//复制父结点关键字
					
					for (int i = 0; i <= delLoc.pt->keynum; i++)
						if (rsib->ptr[i] = delLoc.pt->ptr[i])
							rsib->ptr[i]->parent = rsib;
					
					std::memmove(rsib->parent->ptr + index, rsib->parent->ptr + index + 1, sizeof(BTNode *) *(rsib->parent->keynum - index));
					rsib->parent->ptr[rsib->parent->keynum] = NULL;
                    deleteNode(delLoc.pt);
					
					delLoc.pt = rsib->parent;
					delLoc.i = index + 1;
				}
				else						//剩下空根结点+一个子树，更新根结点
				{
					BTNode *node = root->ptr[0];
					deleteNode(root);
					root = node;
					root->parent = NULL;
					finished = true;
                    levelNum--;
                    time += count.elapsed();
                    work->run();
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
	std::memset(node->ptr, 0, sizeof(BTNode *) * (n + 1));	//将全部指针设置为NULL
	
	return node;
}

template<typename Item>
B_tree<Item>::B_tree(const int order)
{
	n = order;
    work = new WorkThread(5,0);
    QObject::connect(work, SIGNAL(finished()), work, SLOT(deleteLater()));
	root = newNode();				//一个key数为0的结点表示空树
    levelNum = 1;
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
        if (i <= p->keynum && p->key[i] == toSearch)
			found = true;
		else
		{
			q = p;
			p = p->ptr[i - 1];
		}
	}
    if (found)
        return Result{p, i, true};

    return Result{q, i, false};
}

template<typename Item>
bool B_tree<Item>::insertItem(const Item &toInsert)
{
	Item x = toInsert;
	BTNode *ap = NULL;
	bool finished = false;
    QTime count;
    count.start();
	Result insLoc = searchBTree(toInsert);
	
	if (insLoc.tag)
		return false;
    time = 0;
	while (!finished && insLoc.pt != NULL)
	{
		/*移动顺序表，插入关键字和指针*/
        std::memmove(insLoc.pt->key + insLoc.i + 1, insLoc.pt->key + insLoc.i, sizeof(Item) * (insLoc.pt->keynum + 1 - insLoc.i));
        std::memmove(insLoc.pt->ptr + insLoc.i + 1, insLoc.pt->ptr + insLoc.i, sizeof(BTNode *) * (insLoc.pt->keynum + 1 - insLoc.i));
		insLoc.pt->key[insLoc.i] = x;
		insLoc.pt->ptr[insLoc.i] = ap;
		insLoc.pt->keynum++;

        time += count.elapsed();
        work->run();
        count.restart();

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
                if (ap->ptr[i - mid] != NULL)           //fix bug:移动指针后改变其parent
                    ap->ptr[i - mid]->parent = ap;
			}
			ap->ptr[0] = insLoc.pt->ptr[mid];
            if (ap->ptr[0] != NULL)                     //fix bug:移动指针后改变其parent
                ap->ptr[0]->parent = ap;
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
        newRoot->ptr[0] = root;         //fixBug:原为insLoc.pt(NULL)
		newRoot->ptr[1] = ap;
		root = newRoot;
        levelNum++;         //层数加1
        root->parent = NULL;
        time += count.elapsed();
        work->run();
	}
	return true;
}

template <typename Item>
void B_tree<Item>::deleteNode(BTNode *node)
{
	if (node == NULL)
		return;
	
	delete []node->key;
	delete []node->ptr;
	delete node;
}

template <typename Item>
B_tree<Item>::~B_tree()
{
	std::queue<BTNode *> que;
	BTNode *node;
	
	que.push(root);
	
	while(!que.empty())
	{
		node = que.front();
		que.pop();
		for (int i = 0; i <= node->keynum; i++)
			if (node->ptr[i] != NULL)
				que.push(node->ptr[i]);
			else
				break;
			
		deleteNode(node);
	}
    work->change();
    work->terminate();
    delete work;
}

template <typename Item>
int B_tree<Item>::getLevelNum()
{
    return levelNum;
}

template <typename Item>
int B_tree<Item>::nodeLevel(BTNode *node)
{
    int i = 1;

    while (node->parent != NULL)
    {
        node = node->parent;
        i++;
    }

    return i;
}

template <typename Item>
void B_tree<Item>::saveToFile(QFile *file)
{
    QTime count;

    count.start();

    file->write((const char *)&n, sizeof(int));
    file->write((const char *)&levelNum, sizeof(int));
    std::queue<BTNode *> que;
    que.push(root);
    BTNode *node;

    while (!que.empty())
    {
        node = que.front();
        que.pop();
        file->write((const char *)&(node->keynum), sizeof(int));
        for (int i = 1; i <= node->keynum; i++)
            file->write((const char *)(node->key + i), sizeof(Item));
        for (int i = 0; i <= node->keynum; i++)
        {
            file->write((const char *)(node->ptr + i), sizeof(BTNode *));
            if (node->ptr[i] != NULL)
                que.push(node->ptr[i]);
        }
    }

    time = count.elapsed();
}

template <typename Item>
bool B_tree<Item>::isEmpty()
{
    return (root == NULL || root->keynum == 0);
}

template <typename Item>
void B_tree<Item>::loadFromFile(QFile *file)
{
    QTime count;

    count.start();
    std::queue<BTNode *> que;
    BTNode *node;
    file->read((char *)&levelNum, sizeof(int));
    readNode(root, file);
    que.push(root);

    while (!que.empty())
    {
        node = que.front();
        que.pop();
        for (int i = 0; i <= node->keynum; i++)
            if (node->ptr[i] != NULL)
            {
                node->ptr[i] = newNode();
                node->ptr[i]->parent = node;
                readNode(node->ptr[i], file);
                que.push(node->ptr[i]);
            }
            else
                break;
    }

    time = count.elapsed();
}

template <typename Item>
void B_tree<Item>::readNode(BTNode *node, QFile *file)
{
    file->read((char *)&(node->keynum), sizeof(int));
    for (int i = 1; i <= node->keynum; i++)
        file->read((char *)(node->key + i), sizeof(Item));
    for (int i = 0; i <= node->keynum; i++)
        file->read((char *)(node->ptr + i), sizeof(BTNode *));
}

template <typename Item>
long B_tree<Item>::getTime()
{
    return time;
}

template <typename Item>
bool B_tree<Item>::search(const Item &toSearch)
{
    Result sres = searchBTree(toSearch);

    return sres.tag;
}

template <typename Item>
std::set<Item> *B_tree<Item>::rangeSerach(const Item &LBound, const Item &UBound)
{
    BTNode *node;
    QTime count;

    std::stack<BTNode *> nodestk;
    std::stack<int> numstk;
    int i;
    Result max, min;

    count.start();

    std::set<Item> *result = new std::set<Item>;
    max = searchBTree(UBound);
    min = searchBTree(LBound);

	/*中序遍历*/
    node = root;
    i = 0;
    while ((node && i <= node->keynum) || !nodestk.empty())
    {
        if (node)                           //访问子树
        {
            if (i < node->keynum)           //该结点后续仍需访问，将结点和索引压栈
            {
                if (node == max.pt && i == max.i)   //遇到最大值，停止执行
                    break;
                if (node == min.pt && i == 0)       //遇到最小值，当前结点不为叶结点
                {
                    result->insert(node->key[min.i]);   //将最小值插入set
                    i = min.i - 1;                      //继续访问最小值右边的子树
                }

                nodestk.push(node);
                numstk.push(i + 1);
            }
            node = node->ptr[i];            //访问node的第i个子树
            i = 0;
        }
        else                                //访问关键字
        {
            node = nodestk.top();
            nodestk.pop();
            i = numstk.top();
            numstk.pop();
            if (node->ptr[0] == NULL)       //最底层结点，直接访问所有关键字
            {
                for (int num = i; num <= node->keynum; num++)
                    if (node->key[num] >= LBound && node->key[num] <= UBound)
                        result->insert(node->key[num]);
                if (node->key[node->keynum] >= UBound)
                    break;
                node = NULL;                //node设为NULL，则下次循环继续从栈中弹出结点
            }
            else if (node->key[i] >= LBound && node->key[i] <= UBound)
                result->insert(node->key[i]);
        }
    }
    time = count.elapsed();
    return result;
}

#endif
