#ifndef XL_C_TREE_H
#define XL_C_TREE_H
#include <stdlib.h>
#include "xl_c_list.h"


//设置对象的析构函数
#define UT_SET_FUNC_FREE         1
//遍历类型:前序
#define UT_TRAVERSE_PREORDER     2
//遍历类型:后序
#define UT_TRAVERSE_POSORDER     4
//遍历类型:层次
#define UT_TRAVERSE_LAYER        8
//遍历某一层,(层数由param1指定) 
#define UT_TRAVERSE_LAYER_N      16
//遍历方向:从左到右(顺序)
//可组合遍历类型使用
#define UT_TRAVERSE_LR           32 
//遍历方向:从右到左(逆序)
//可组合遍历类型使用
#define UT_TRAVERSE_RL           64


///////////////调用者函数类型,用于区别调用函数的类型(可组合使用)//////////
#define UT_TYPEF_PREORDER       UT_TRAVERSE_PREORDER
#define UT_TYPEF_POSORDER       UT_TRAVERSE_POSORDER
#define UT_TYPEF_LAYER          UT_TRAVERSE_LAYER
#define UT_TYPEF_DESTROY        128

///////////////回调函数返回值,不可组合使用//////////////////
//继续处理
#define UT_TRAVERSE_RET_CT   1
//退出处理
#define UT_TRAVERSE_RET_EXIT 0
///////////////前序遍历回调函数返回值,不可组合使用//////////////////
//跳过当前节点的子树,不进行处理
#define UT_TRAVERSE_RET_CS 3     
///////////////层次遍历的结果,不可组合使用//////////////////
//成功遍历完当前层
#define UT_TRAVERSE_LAYER_OK    1
//当前层不存在,遍历失败
#define UT_TRAVERSE_LAYER_NULL   0
//用户中断了当前层的遍历
#define UT_TRAVERSE_LAYER_INT    3


typedef int (*ut_cf)(struct _ut_cf_info*);  //层遍历函数类型
typedef int (*ut_cfex)(struct _ut_cf_info_ex*);  //层遍历函数类型

typedef struct _ut_node_info
{
	struct _list childs; // 孩子列表

	void *data;
	unsigned size;
	char isFree;  //是否释放对象
	unsigned z;  //当前节点的z序(相当于X坐标)
	unsigned l; //当前节点位于的层(相当于y坐标)
}ut_node_info, *ut_node_pinfo;


typedef struct _ut_node
{
	////使得排序Z序更简单/////
	struct _ut_node* parent;  //使得排序z序的时候,不交换父亲
	ut_node_info *pni;
	
} ut_node,*ut_pnode;

/////////数据头,用于从数据中获取节点指针////////
typedef struct _ut_head
{
	ut_pnode  node;

} ut_head, *ut_phead;


typedef struct _ut_cf_info
{	 
	/////(由用户传入的值)////
	ut_cf  cfProc;         //用户回调函数
  	unsigned long param1,param2;   //附加的参数
	unsigned flag;      //使用它来控制函数的行为(可以使用或运算符来组合)

	/////(由算法传入的值)////
	ut_pnode node; //指向当前节点的指针(相信用户有能力去控制节点)
	
}ut_cf_info, *ut_cf_pinfo;

/////////扩展用户信息结构(算法内部使用)///////////
typedef struct _ut_cf_info_ex
{
	ut_cf_pinfo  pi;
	unsigned char offsetStart; //起始的迭代器偏移
	unsigned char offsetNext;  //下一个迭代器的偏移

}ut_cf_info_ex,*ut_cf_pinfo_ex;


typedef struct _tree
{
	struct _ut_node *root;
	unsigned n;
	unsigned l;  //层数

}ut, *put;

////////////内部使用的算法/////////////
void ut_destroy_(ut_pnode root,ut_cf_pinfo pi);
////////////////////////////
void ut_traverse_set_offset(ut_cf_pinfo_ex piex, ut_cf_pinfo pi);
//前序遍历
int ut_traverse_pre_order_(ut_pnode root, ut_cf_pinfo_ex pi);
//后续遍历
int ut_traverse_pos_order_(ut_pnode root, ut_cf_pinfo_ex pi);
//层次遍历
int ut_traverse_layer_(ut_pnode root, int level, ut_cf_pinfo_ex pi);
//前序遍历前做的一些处理
void ut_traverse_pre_order(ut_pnode root, ut_cf_pinfo pi);
//后序遍历前做的一些处理
void ut_traverse_pos_order(ut_pnode root, ut_cf_pinfo pi);
//层次遍历前做的一些处理
void ut_traverse_layer(ut_pnode root, ut_cf_pinfo pi);
//根据Z序,调整node在所属层中的位置
void ut_z_sort_(ut_pnode *ns, unsigned n);
//////////////////////////////////

put  ut_create();
//ut_cf_info::flag由算法设置,你无需设置
//ut_cf_info::cfProc 由算法使用,你无需设置
//ut_cf_info::param1为析构函数,可为NULL
void ut_destroy(put ut, ut_cf_pinfo pi);
//把obj加入父亲的孩子列表中
//parent 父亲
//brother 兄弟
//obj,size 对象和大小
//isFree 对象是否位于堆上？如果为假,obj是没有数据头的
//isFont 对象加入到兄弟前面吗？
//返回值 返回新加入对象的节点
ut_pnode ut_insert(put ut, ut_pnode parent, ut_pnode brother, void *obj, size_t size, BOOL_ isFree, BOOL_ isFont);
//删除一颗子树
void ut_earse(put ut, ut_pnode root);
//获取root子树的节点数
unsigned ut_getcm(ut_pnode root);
//获取root为根的第n层的节点数
unsigned ut_getcn(ut_pnode root, unsigned n);
//设置node的Z序列
int  ut_z_set(put ut, ut_pnode node, unsigned z);
//设置node的z序为z,以node所在的孩子列表来重新排序
void  ut_z_sort(put ut, ut_pnode node, unsigned z);
//设置node的z序为z,以node所在层来重新排序
void ut_z_sortl(ut_pnode root, ut_pnode node);
//使得node断绝先前的父子关系,认parent做父亲
void ut_set_parent(ut_pnode parent, ut_pnode node);
void ut_traverse(put ut, ut_cf_pinfo pi);

/////////////////////////////////

//通过flag来确定值得类型,来配置树
void ut_config_info(put ut,unsigned value, unsigned flag);
//从数据获取数据头
#define  ut_get_head(data) ((ut_phead)((char*)(data) - sizeof(ut_head)))
//从数据获取节点指针
#define ut_get_node(data) (((ut_phead)((char*)(data) - sizeof(ut_head)))->node)
#endif 
