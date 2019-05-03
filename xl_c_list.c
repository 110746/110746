#include "xl_c_list.h"
#include <string.h>
#include  <assert.h>


pforward_list fs_create(size_t dsize)
{
	pforward_list fs = (pforward_list)malloc(sizeof(forward_list));

	fs->size = dsize;
	fs->n = 0;
	fs->head = I_END;
	fs->tail = I_END;

	//head前面的节点
	fs->headPrev = (fs_pnode)calloc(1,sizeof(fs_node));
	if (fs->headPrev == NULL)
	{
		free(fs);
		assert(0);
	}

	return fs;
}

void fs_destroy(pforward_list fs)
{
	fs_pnode iter;
	fs_pnode itmp;

	for (iter = fs->head; iter != I_END;)
	{
		itmp = iter;
		iter = iter->next;
		free(itmp->data);
		free(itmp);
		
	}
	free(fs->headPrev);
	free(fs);
	memset(fs, 0, sizeof(forward_list));

}

fs_pnode  fs_insert_after(pforward_list fs, fs_pnode node, const void *data,int iscpy)
{
	fs_pnode newNode;
//..................................
	newNode = (fs_pnode)calloc(1,sizeof(fs_node));

	if (iscpy)
	{
		newNode->data = malloc(fs->size);
		memcpy(newNode->data, data, fs->size);

	}else
		newNode->data = (void*)data;

//..................................

	if (fs->n == 0 || fs->headPrev == node)
		fs->head = newNode;	
	
	newNode->next = node->next;
	node->next = newNode;
	
   if( fs->tail == node)
	   fs->tail = newNode;

	++fs->n;

	return newNode;
}

fs_pnode  fs_push_front(pforward_list fs, const void *data, int iscpy)
{
	fs_pnode newNode;

	newNode = (fs_pnode)calloc(1, sizeof(fs_node));

	if (iscpy)
	{
		newNode->data = malloc(fs->size);
		memcpy(newNode->data, data, fs->size);
	}
	else
		newNode->data = (void*)data;

	if (fs->n == 0)
		fs->tail = newNode;
	
	//向左边增长
	newNode->next = fs->head;
	fs->head = newNode;
	fs->headPrev->next = newNode;
	++fs->n;

	return newNode;
}

fs_pnode  fs_push_back(pforward_list fs, const void *data, int iscpy)
{
	fs_pnode newNode;

	newNode =(fs_pnode)calloc(1, sizeof(fs_node));

	if (iscpy)
	{
		newNode->data = malloc(fs->size);
		memcpy(newNode->data, data, fs->size);
	}
	else
		newNode->data = (void*)data;

	if (fs->n == 0)
	{
		fs->head = newNode;
		fs->headPrev->next = newNode;
	}else 
		fs->tail->next = newNode;
		
	//向右边增长
	fs->tail = newNode;
	++fs->n;

	return newNode;
}

fs_pnode fs_erase_after(pforward_list fs, fs_pnode node,int spl)
{
	fs_pnode pnode;

	pnode = node->next;

	if (pnode == fs->head)
		fs->head = pnode->next;

	if (pnode == fs->tail)
		fs->tail = pnode->next;

	node->next = pnode->next;

	//如果ifs为0，则相当于把pnode从链表中分离出来
	if (!spl)
	{
		free(pnode->data);
		free(pnode);
    }

  
	--fs->n;
	return node->next;
}

//////////////////////////////////////////////////////////////
plist list_create()
{
	return (plist)calloc(1, sizeof(list));
}
void list_clear(plist list)
{
	if (NULL == list)
		return;

	list_destroy(list);
	memset(list, 0, sizeof(list));
}

void list_destroy(plist list)
{
	ls_pnode ndDel;
	ls_pnode ndIter;

	if (NULL == list)
		return;

	//删除所有节点
	ndIter = list->first;
	while (ndIter)
	{
		ndDel = ndIter;
		ndIter = ndIter->next;

		if (ndDel->isfree)
			free((ls_pdhead)ndDel->data - 1);
		free(ndDel);
	}
    
}

void list_insertp(plist list, ls_pnode ndPos, ls_pnode node, BOOL_ isFont)
{
	ls_pnode temp;

	if (NULL == list->first)
	{
		list->first = node;
		list->last = node;
		return;
	}

	//如果是在前面插入
	if (isFont)
	{
		temp = ndPos->back;

		//让当前节点成为上一个节点
		node->next = ndPos;
		ndPos->back = node;
		//如果上一个节点存在
		if (temp)
		{
			temp->next = node;
			node->back = temp;
		}
		else
			list->first = node;
	}
	else
	{
		temp = ndPos->next;

		//让当前节点成为下一个节点
		ndPos->next = node;
		node->back = ndPos;

		//如果原来的下一个节点存在
		if (temp)
		{
			node->next = temp;
			temp->back = node;
		}
		else
			list->last = node;
	}

}


ls_pnode list_insert(plist list, ls_pnode ndPos, void *obj, size_t size, BOOL_ isFree, BOOL_ isFont)
{
	ls_pnode node;

	if (NULL == list || NULL == obj)
		return NULL;

	//增加节点数目
	++list->number;
	node = (ls_pnode)calloc(1, sizeof(ls_node));

	if (isFree)
	{
		ls_pdhead pHead = (ls_pdhead)calloc(1, sizeof(ls_dhead) + size);

		//保存数据头
		pHead->thisNode = node;

		//复制对象数据
		memcpy(pHead + 1, obj, size);

		//指向数据头
		node->data = pHead + 1;
	}
	else
	{
		node->data = obj;
	}

	node->isfree = isFree;
	node->size = size;
	//插入节点
	list_insertp(list, ndPos, node, isFont);

	return node;
}


ls_pnode list_add_new(plist list, void *obj, size_t size)
{
	return list_insert(list, list->last, obj, size, 1, 0);
}

ls_pnode list_add_old(plist list, void *obj)
{
	return list_insert(list, list->last, obj, 0, 0, 0);
}

void list_erase(plist list, ls_pnode node, BOOL_ isDesy)
{
	ls_pnode back;
	ls_pnode next;

	if (NULL == list || NULL == node)
		return;

	back = node->back;
	next = node->next;
	//脱离
	node->back = NULL;
	node->next = NULL;

	//node的上一个节点指向下一个节点
	if (back)
		back->next = next;
	else
		list->first = next;

	//node的下一个节点指向上一个节点
	if (next)
		next->back = back;
	else
		list->last = back;

	//销毁对象
	if (isDesy)
	{
		if (node->isfree)
			free((ls_pdhead)node->data - 1);

		free(node);
	}

	--list->number;

}


void list_copy(plist ls1, plist ls2)
{
	ls_pnode ndIter;

	if (NULL == ls1 || NULL == ls2)
		return;

	list_clear(ls1);

	ndIter = ls2->first;

	while (ndIter)
	{
		list_add_new(ls1, ndIter->data, ndIter->size);
		ndIter = ndIter->next;
	}

}
