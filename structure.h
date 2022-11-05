#ifndef STRUCTURE_H
#define STRUCTURE_H

#include<stdint.h>


#define RECORD_LENGTH 100 //每条记录属性个数
#define RECORD_SIZE_BYTE (sizeof(int64_t)*(RECORD_LENGTH+1))//每条记录大小,包含一个主键
#define M 2
#define MAX_RESULT_NUM 10
//#define MAX_RECORD_NUM 1000000
#define MAX_RECORD_NUM 1000000

typedef struct Record {
    int64_t primary_key; //主键，即该记录的序号
    int64_t record_array[RECORD_LENGTH];//属性值
} Record;

//索引节点结构
typedef struct IndexNode {
    int64_t primary_key;//主键
    int64_t value;//属性值
} IndexNode;

//B+树节点结构
typedef struct BPlusTreeNode {
    IndexNode index_nodes[2 * M ];//关键字，即为索引节点  key
    struct BPlusTreeNode* childs[2 * M + 1]; //子节点数组       value
    int num;//子节点数
    bool is_leaf;
    struct BPlusTreeNode* parent;
    //struct BPlusTreeNode* prev;
    struct BPlusTreeNode* next;
} BPlusTreeNode;


#endif // STRUCT_H




