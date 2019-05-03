#ifndef XL_C_TREE_H
#define XL_C_TREE_H
#include <stdlib.h>
#include "xl_c_list.h"


//���ö������������
#define UT_SET_FUNC_FREE         1
//��������:ǰ��
#define UT_TRAVERSE_PREORDER     2
//��������:����
#define UT_TRAVERSE_POSORDER     4
//��������:���
#define UT_TRAVERSE_LAYER        8
//����ĳһ��,(������param1ָ��) 
#define UT_TRAVERSE_LAYER_N      16
//��������:������(˳��)
//����ϱ�������ʹ��
#define UT_TRAVERSE_LR           32 
//��������:���ҵ���(����)
//����ϱ�������ʹ��
#define UT_TRAVERSE_RL           64


///////////////�����ߺ�������,����������ú���������(�����ʹ��)//////////
#define UT_TYPEF_PREORDER       UT_TRAVERSE_PREORDER
#define UT_TYPEF_POSORDER       UT_TRAVERSE_POSORDER
#define UT_TYPEF_LAYER          UT_TRAVERSE_LAYER
#define UT_TYPEF_DESTROY        128

///////////////�ص���������ֵ,�������ʹ��//////////////////
//��������
#define UT_TRAVERSE_RET_CT   1
//�˳�����
#define UT_TRAVERSE_RET_EXIT 0
///////////////ǰ������ص���������ֵ,�������ʹ��//////////////////
//������ǰ�ڵ������,�����д���
#define UT_TRAVERSE_RET_CS 3     
///////////////��α����Ľ��,�������ʹ��//////////////////
//�ɹ������굱ǰ��
#define UT_TRAVERSE_LAYER_OK    1
//��ǰ�㲻����,����ʧ��
#define UT_TRAVERSE_LAYER_NULL   0
//�û��ж��˵�ǰ��ı���
#define UT_TRAVERSE_LAYER_INT    3


typedef int (*ut_cf)(struct _ut_cf_info*);  //�������������
typedef int (*ut_cfex)(struct _ut_cf_info_ex*);  //�������������

typedef struct _ut_node_info
{
	struct _list childs; // �����б�

	void *data;
	unsigned size;
	char isFree;  //�Ƿ��ͷŶ���
	unsigned z;  //��ǰ�ڵ��z��(�൱��X����)
	unsigned l; //��ǰ�ڵ�λ�ڵĲ�(�൱��y����)
}ut_node_info, *ut_node_pinfo;


typedef struct _ut_node
{
	////ʹ������Z�����/////
	struct _ut_node* parent;  //ʹ������z���ʱ��,����������
	ut_node_info *pni;
	
} ut_node,*ut_pnode;

/////////����ͷ,���ڴ������л�ȡ�ڵ�ָ��////////
typedef struct _ut_head
{
	ut_pnode  node;

} ut_head, *ut_phead;


typedef struct _ut_cf_info
{	 
	/////(���û������ֵ)////
	ut_cf  cfProc;         //�û��ص�����
  	unsigned long param1,param2;   //���ӵĲ���
	unsigned flag;      //ʹ���������ƺ�������Ϊ(����ʹ�û�����������)

	/////(���㷨�����ֵ)////
	ut_pnode node; //ָ��ǰ�ڵ��ָ��(�����û�������ȥ���ƽڵ�)
	
}ut_cf_info, *ut_cf_pinfo;

/////////��չ�û���Ϣ�ṹ(�㷨�ڲ�ʹ��)///////////
typedef struct _ut_cf_info_ex
{
	ut_cf_pinfo  pi;
	unsigned char offsetStart; //��ʼ�ĵ�����ƫ��
	unsigned char offsetNext;  //��һ����������ƫ��

}ut_cf_info_ex,*ut_cf_pinfo_ex;


typedef struct _tree
{
	struct _ut_node *root;
	unsigned n;
	unsigned l;  //����

}ut, *put;

////////////�ڲ�ʹ�õ��㷨/////////////
void ut_destroy_(ut_pnode root,ut_cf_pinfo pi);
////////////////////////////
void ut_traverse_set_offset(ut_cf_pinfo_ex piex, ut_cf_pinfo pi);
//ǰ�����
int ut_traverse_pre_order_(ut_pnode root, ut_cf_pinfo_ex pi);
//��������
int ut_traverse_pos_order_(ut_pnode root, ut_cf_pinfo_ex pi);
//��α���
int ut_traverse_layer_(ut_pnode root, int level, ut_cf_pinfo_ex pi);
//ǰ�����ǰ����һЩ����
void ut_traverse_pre_order(ut_pnode root, ut_cf_pinfo pi);
//�������ǰ����һЩ����
void ut_traverse_pos_order(ut_pnode root, ut_cf_pinfo pi);
//��α���ǰ����һЩ����
void ut_traverse_layer(ut_pnode root, ut_cf_pinfo pi);
//����Z��,����node���������е�λ��
void ut_z_sort_(ut_pnode *ns, unsigned n);
//////////////////////////////////

put  ut_create();
//ut_cf_info::flag���㷨����,����������
//ut_cf_info::cfProc ���㷨ʹ��,����������
//ut_cf_info::param1Ϊ��������,��ΪNULL
void ut_destroy(put ut, ut_cf_pinfo pi);
//��obj���븸�׵ĺ����б���
//parent ����
//brother �ֵ�
//obj,size ����ʹ�С
//isFree �����Ƿ�λ�ڶ��ϣ����Ϊ��,obj��û������ͷ��
//isFont ������뵽�ֵ�ǰ����
//����ֵ �����¼������Ľڵ�
ut_pnode ut_insert(put ut, ut_pnode parent, ut_pnode brother, void *obj, size_t size, BOOL_ isFree, BOOL_ isFont);
//ɾ��һ������
void ut_earse(put ut, ut_pnode root);
//��ȡroot�����Ľڵ���
unsigned ut_getcm(ut_pnode root);
//��ȡrootΪ���ĵ�n��Ľڵ���
unsigned ut_getcn(ut_pnode root, unsigned n);
//����node��Z����
int  ut_z_set(put ut, ut_pnode node, unsigned z);
//����node��z��Ϊz,��node���ڵĺ����б�����������
void  ut_z_sort(put ut, ut_pnode node, unsigned z);
//����node��z��Ϊz,��node���ڲ�����������
void ut_z_sortl(ut_pnode root, ut_pnode node);
//ʹ��node�Ͼ���ǰ�ĸ��ӹ�ϵ,��parent������
void ut_set_parent(ut_pnode parent, ut_pnode node);
void ut_traverse(put ut, ut_cf_pinfo pi);

/////////////////////////////////

//ͨ��flag��ȷ��ֵ������,��������
void ut_config_info(put ut,unsigned value, unsigned flag);
//�����ݻ�ȡ����ͷ
#define  ut_get_head(data) ((ut_phead)((char*)(data) - sizeof(ut_head)))
//�����ݻ�ȡ�ڵ�ָ��
#define ut_get_node(data) (((ut_phead)((char*)(data) - sizeof(ut_head)))->node)
#endif 
