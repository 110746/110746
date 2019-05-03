#include "xl_c_tree.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define get_member(type,p_strt,size)   (*(type*)((char*)(p_strt) + (size)))

static int ut_destroy_call(ut_cf_pinfo pi)
{
	ut_node_pinfo pni = pi->node->pni;

	//�ͷŽڵ��б�
	if (list_size(&pni->childs))
		list_destroy(&pni->childs);

	//���û��ͷ�����
   if(pi->param1)
	  if (!((ut_cf)pi->param1)(pi))
		  return 0;

   //�ͷŵ�ǰ�ڵ�
    if(pni->isFree)
	   free(ut_get_head(pni->data));

	   free(pni);
	
	return UT_TRAVERSE_RET_CT;
}
void ut_destroy_(ut_pnode root,ut_cf_pinfo pi)
{
	if (root == NULL)
		return;

	ut_pnode parent = root->parent;

	//��������ĺ���
	pi->cfProc =  ut_destroy_call;
	pi->flag = UT_TRAVERSE_POSORDER | UT_TRAVERSE_LR | UT_TYPEF_DESTROY;
	ut_traverse_pos_order(root, pi);

	//�����һ������,�������׺����б���ɾ��
	if (parent)
	{
		list_erase(&parent->pni->childs, 
			list_get_node(root), 1);

	}
	//���������,��ֱ��ɾ��
	else
	{
		free(root);
	}
}
//////////////////////////////
void ut_traverse_set_offset(ut_cf_pinfo_ex piex, ut_cf_pinfo pi)
{
	//����Ǵ����ұ���
	if (pi->flag & UT_TRAVERSE_LR)
		piex->offsetStart = list_offset_first;
	//����Ǵ��ҵ��ұ���
	else if (pi->flag & UT_TRAVERSE_RL)
		piex->offsetStart = list_offset_last;
	else //Ĭ��Ϊ˳�����
		piex->offsetStart = list_offset_first;

	if (pi->flag & UT_TRAVERSE_LR)
		piex->offsetNext = list_offset_next;
	//����Ǵ��ҵ��ұ���
	else if (pi->flag & UT_TRAVERSE_RL)
		piex->offsetNext = list_offset_back;
	else //Ĭ��Ϊ˳�����
		piex->offsetNext = list_offset_next;
}

int ut_traverse_pre_order_(ut_pnode root, ut_cf_pinfo_ex piex)
{
	if (root == NULL)
		return 0;

	piex->pi->node = root;

	switch (piex->pi->cfProc(piex->pi))
	{
	case UT_TRAVERSE_RET_CS:
		return UT_TRAVERSE_RET_CT;
	case UT_TRAVERSE_RET_EXIT:
		return UT_TRAVERSE_RET_EXIT;
	}
	
	ls_pnode iter = get_member(ls_pnode, &root->pni->childs, piex->offsetStart);
	while(iter)
	{
		//����û�����UT_TRAVERSE_RET_EXIT���˳�����
		if (ut_traverse_pre_order_((ut_pnode)iter->data,piex)==
			UT_TRAVERSE_RET_EXIT)
			  return UT_TRAVERSE_RET_EXIT;
	
		iter = get_member(ls_pnode, iter, piex->offsetNext);
	}

	return UT_TRAVERSE_RET_CT;

}

int ut_traverse_pos_order_(ut_pnode root, ut_cf_pinfo_ex piex)
{	
	if (root == NULL)
		return UT_TRAVERSE_RET_CT;

	ls_pnode iter = get_member(ls_pnode, &root->pni->childs, piex->offsetStart);
	while (iter)
	{
		//����û�����UT_TRAVERSE_RET_EXIT���˳�����
		if (ut_traverse_pos_order_((ut_pnode)iter->data, piex) ==
			UT_TRAVERSE_RET_EXIT)
			return UT_TRAVERSE_RET_EXIT;

		iter = get_member(ls_pnode, iter, piex->offsetNext);
	}

	piex->pi->node = root;
	//����û�����0���˳�����
	return piex->pi->cfProc(piex->pi);
	
}

int ut_traverse_layer_(ut_pnode root, int level, ut_cf_pinfo_ex piex)
{
	if (root == NULL || level < 0)
		return UT_TRAVERSE_LAYER_NULL;

	int ret = UT_TRAVERSE_LAYER_NULL;

	if (level == 0)
	{		 
		piex->pi->node = root;
		//����û�����0���˳�����
		if (!piex->pi->cfProc(piex->pi))
			return UT_TRAVERSE_LAYER_INT;

		return  UT_TRAVERSE_LAYER_OK;
	}
	
	ls_pnode iter = get_member(ls_pnode, &root->pni->childs, piex->offsetStart);
	
	while (iter)
	{		
		ret |= ut_traverse_layer_((ut_pnode)iter->data, level - 1, piex);

		//����û�����0���˳�����	
		if (ret == UT_TRAVERSE_LAYER_INT)
			return UT_TRAVERSE_LAYER_INT;

		iter = get_member(ls_pnode, iter, piex->offsetNext);
	 
	}
	
	return ret ;
}

void ut_traverse_pre_order(ut_pnode root, ut_cf_pinfo pi)
{
	if (root == NULL)
		return;

	ut_cf_info_ex piex;
	piex.pi = pi;
	ut_traverse_set_offset(&piex, pi);
	ut_traverse_pre_order_(root, &piex);
}

void ut_traverse_pos_order(ut_pnode root, ut_cf_pinfo pi)
{
	if (root == NULL)
		return;

	ut_cf_info_ex piex;
	piex.pi = pi;
	ut_traverse_set_offset(&piex, pi);
	ut_traverse_pos_order_(root, &piex);
}

void ut_traverse_layer(ut_pnode root, ut_cf_pinfo pi)
{
	if (root == NULL)
		return;

	ut_cf_info_ex piex;
	piex.pi = pi;
	ut_traverse_set_offset(&piex, pi);

	if (pi->flag & UT_TRAVERSE_LAYER_N)
	{
		ut_traverse_layer_(root, pi->param1, &piex);
	}
	else if (pi->flag & UT_TRAVERSE_LAYER)
	{
		int level = 0;
		while (ut_traverse_layer_(root, level, &piex) ==
			UT_TRAVERSE_LAYER_OK)
			++level;
	}
     
}
//////////////////////////////////

void ut_z_sort_(ut_pnode *ns,unsigned n)
{
	static ut_node_pinfo pniTmp;
	//ð������z��(ע��:�������ʹ���ȶ��������㷨)
	for (unsigned i = 0; i < n - 1; ++i)
	{
		for (unsigned j = 0; j < n - 1 - i; ++j)
		{
			if (ns[j]->pni->z > ns[j + 1]->pni->z)
			{
				pniTmp = ns[j]->pni;
				ns[j]->pni = ns[j + 1]->pni;
				ns[j + 1]->pni = pniTmp;
			}
		}
	}
}

void  ut_z_sort(put ut, ut_pnode node, unsigned z)
{
	list *ls =  &node->parent->pni->childs;
	ls_pnode iter;
	unsigned n = list_size(ls);
	ut_pnode *ns = (ut_pnode *)malloc(sizeof(ut_pnode) * n);
	ut_pnode *pns = ns;

	for (iter = list_first(ls); iter; iter = iter->next)
		*pns++ = (ut_pnode)iter->data;

	ut_z_sort_(ns,n);
	free(ns);
 
}

static int ut_z_sortl_call(ut_cf_pinfo pi)
{
	//int v =  *(int*)pi->node->pni->data;
	ut_pnode *ns = (ut_pnode *)pi->param2;
	unsigned *i = (unsigned*)(ns - 1);
	//����ڵ㵽������
	ns[(*i)++] = pi->node;

	return UT_TRAVERSE_RET_CT;
}

void ut_z_sortl(ut_pnode root, ut_pnode node)
{
	if (root == NULL)
		return;

	ut_node_pinfo pni = node->pni;
	//��ȡnode���ڲ�Ľڵ���
	unsigned nl = ut_getcn(root, pni->l);
	ut_pnode *ns = (ut_pnode *)malloc(sizeof(ut_pnode) * ( nl + 1));
	ut_cf_info ci = { 0 };
	ci.cfProc = ut_z_sortl_call;
	ci.flag = UT_TRAVERSE_LAYER_N | UT_TRAVERSE_LR;
	ci.param1 = pni->l;
	//param2�洢�ڴ����������
	*ns++ = 0;
	ci.param2 = (unsigned long)ns;
	ut_traverse_layer(root, &ci);

	//ð������z��(ע��:�������ʹ���ȶ��������㷨)
	ut_z_sort_(ns, nl);

	free(ns - 1);

}

////////////////////////////////////////////////////////////
put  ut_create()
{
	return (put)calloc(1, sizeof(ut));
}

void ut_destroy(put ut,ut_cf_pinfo pi)
{
	static struct _tree tmp = { 0 };
	ut_destroy_(ut->root, pi);	
	*ut = tmp;
}

ut_pnode ut_insert(put ut,ut_pnode parent,ut_pnode brother,void *obj,size_t size,BOOL_ isCopy,BOOL_ isFont)  
{
	ut_node node = { 0 }, *pnode;
	ut_node_pinfo piParent = parent ? parent->pni : NULL;
	ls_pnode lsNode;
	ut_phead head;

	ut_node_pinfo pinfo = (ut_node_pinfo)calloc(1, sizeof(ut_node_info));	
	
	if (isCopy)
	{
		head = (ut_phead)calloc(1, sizeof(ut_head) + size);
		pinfo->data = head + 1;
		memcpy(pinfo->data, obj, size);
		
	}
	else
	{
		pinfo->data = obj;
	}
	//����node��������Ϣ
	pinfo->size = size;
	//pinfo->parent = parent;
	pinfo->isFree = isCopy;
	pinfo->z = 0;
	pinfo->l = piParent ? piParent->l + 1 : 0;

	++ut->n;
	if (pinfo->l > ut->l)
		ut->l = pinfo->l;

	if (ut->root == NULL)
	{
		ut->root = (ut_pnode)calloc(1,sizeof(ut_node));
		ut->root->pni = pinfo;
		head->node = ut->root;
		return ut->root;
	}
	
	if (list_size(&piParent->childs) == 0)
	{
		lsNode = list_insert(&piParent->childs, list_last(&piParent->childs),
			&node, sizeof(ut_node), 1, 0);
	}
	else
	{
		//�ѽڵ�ָ����뵽ָ�����ֵ�λ����
		lsNode = list_insert(&piParent->childs, list_get_node(brother),
			&node, sizeof(ut_node), 1, isFont);
	}

	//ָ��������Ϣ
	pnode = (ut_pnode)lsNode->data;
	pnode->parent = parent;
	pnode->pni = pinfo;
	head->node = pnode;

	return  (ut_pnode)lsNode->data;
}
////////////////////////////////////////
static int ut_earse_call(ut_cf_pinfo pi)
{
	unsigned long *ls = (unsigned long*)pi->param1;
	if (pi->flag & 0x1000)
	{		
		//ȷ��root�Ƿ������߽ڵ�
		if (ls[0] == pi->node->pni->l)
			pi->param2 = 1;
		
		//�ۼƽڵ���
		++ls[1];
			
	}
	else if (pi->flag & 0x2000)
	{
		unsigned l = pi->node->pni->l;

		//�����Ƚϼ���Ҫɾ��������
		if (ls[2] == (unsigned)pi->node)
			return UT_TRAVERSE_RET_CS;

		//��¼��߲�������Ŀ
		if (ls[0] == l)
			++ls[1];
		//��¼�ڶ��ߵĲ���
		if (pi->param2 < l && l != ls[0])
			pi->param2 = l;
	}
			 
	return UT_TRAVERSE_RET_CT;
}

void ut_earse(put ut, ut_pnode root)
{	
	if (ut->root == NULL || root == NULL)
		return;
	else if (ut->root == root)
	{
		ut_cf_info ci = { 0 };
		ut_destroy(ut, &ci);
		ut->n = -1;
		return;
	}
	static ut_cf_info temp = { 0 };
	static const unsigned FLAGS[2] = { 0x1000,0x2000 };
	unsigned l = root->pni->l;
	unsigned n = 0;
	ut_cf_info ci = { 0 };
	unsigned ls[3];

	ci.cfProc = ut_earse_call;
	//ls[0]�洢���ǵ�ǰ��߲��
	ls[0] = ut->l;

	//�����Ҷ�ӽڵ�,����ȥ����
	if (!list_size(&root->pni->childs))
	{
		//�Ƿ��������߲�
		ci.param2 = root->pni->l == ut->l;
		n = 1;
	}
	else
	{
		//root���ӽڵ���
		ls[1] = 0;
		//���������ַ
		ci.param1 = (unsigned long)ls;
		//ci.param2ȷ���Ƿ����
		ci.param2 = 0;
		//���root�����ӽڵ��Ƿ��������߲�
		ci.flag = UT_TRAVERSE_PREORDER | UT_TRAVERSE_LR | FLAGS[0];
		ut_traverse_pre_order(root, &ci);
		n = ls[1];
	}
	
	//���������߲�
	if (ci.param2)
	{
		//ls[0]Ϊ��߲���,
		//ls[1]Ϊ��߲�����Ŀ
		ls[1] = 0;
		//ls[2]Ϊroot
		ls[2] = (unsigned long)root;

		ci.param1 = (unsigned long)ls;
		//c.param2��¼�ڶ��ߵĲ���
		ci.param2 = 0;
		
		ci.flag = UT_TRAVERSE_PREORDER | UT_TRAVERSE_LR | FLAGS[1];
		ut_traverse_pre_order(ut->root, &ci);

		//root���������Ľڵ�
		//����߲��Ψһһ���ڵ�
		//���һ��
		if (ls[1] == 0)
			ut->l = ci.param2;
	}
	
	//������ȥroot�ڵ���
	ut->n -= n;
	ci = temp;
	//�ͷŵ���ǰ�ڵ�
	ut_destroy_(root, &ci);

}

////////////////////////////////////////
static int ut_getcm_call(ut_cf_pinfo pi)
{
	++pi->param1;
	return UT_TRAVERSE_RET_CT;
}
unsigned ut_getcm(ut_pnode root)
{
	static ut_cf_info ci = { ut_getcm_call, 0 };
	//root�Ľڵ���(����root)
	ci.param1 = 0;
	ut_traverse_pre_order(root, &ci);

	return ci.param1;
}
static int ut_getcn_call(ut_cf_pinfo pi)
{
	++pi->param2;
	return UT_TRAVERSE_RET_CT;
}
unsigned ut_getcn(ut_pnode root, unsigned n)
{
	static ut_cf_info ci = { ut_getcn_call, 0,0, UT_TRAVERSE_LAYER_N | UT_TRAVERSE_LR };
	//param1�洢���� 
	ci.param1 = n;
	//param2�洢�ڵ���
	ci.param2 = 0;
	ut_traverse_layer(root, &ci);

	return ci.param2;
}

int  ut_z_set(put ut, ut_pnode node, unsigned z)
{
	if (node->pni->z == z)
		return 0;

	node->pni->z = z;
	ut_z_sort(ut, node,z);
	return 1;
}

void ut_set_parent(ut_pnode parent, ut_pnode node)
{

}

void ut_traverse(put ut, ut_cf_pinfo pi)
{
	if (ut->root == NULL)
		return;

	if (pi->flag & UT_TRAVERSE_PREORDER)
		ut_traverse_pre_order(ut->root, pi);

	else if (pi->flag & UT_TRAVERSE_POSORDER)
		ut_traverse_pos_order(ut->root, pi);

	else  if (pi->flag & UT_TRAVERSE_LAYER ||
		pi->flag & UT_TRAVERSE_LAYER_N)
		  ut_traverse_layer(ut->root, pi);
		       

}

////////////////////////////

void ut_config_info(put ut, unsigned value, unsigned flag)
{
	switch (flag)
	{
	case UT_SET_FUNC_FREE:
		break;
	}
}