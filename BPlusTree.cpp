#include "BPlusTree.h"

//创建一个B+树节点
BPlusTreeNode *BPlusTree::BPlusTreeNode_new()
{
    BPlusTreeNode *node = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    if (node == NULL)
        return NULL;
    for (int i = 0; i < 2 * M; i++)
        node->index_nodes[i].value = -1; // key: 0 ~ 2M-1
    for (int i = 0; i < 2 * M + 1; i++)
        node->childs[i] = NULL; // value: 0 ~ 2M
    node->num = 0;
    node->is_leaf = true;
    node->parent = NULL;
    //node->prev = NULL;
    node->next = NULL;
    return node;
}

// B+树的创建
BPlusTreeNode *BPlusTree::BPlusTree_create()
{
    BPlusTreeNode *node = BPlusTreeNode_new();
    if (NULL == node)
        return NULL;
    //node->prev = node;
    node->next = node;
    return node;
}

//创建一个索引节点，即B+树节点中的关键码
IndexNode BPlusTree::IndexNode_new(const Record &record, int col)
{
    IndexNode indexNode;
    indexNode.primary_key = record.primary_key;
    indexNode.value = record.record_array[col - 1];
    return indexNode;
}

//叶子节点需要分裂
BPlusTreeNode *BPlusTree::BPlusTree_Split_leaf(BPlusTreeNode *node)
{
    BPlusTreeNode *new_node = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    //分裂节点，创建新节点，对其赋值
    new_node->next = node->next;
    node->next = new_node;
    new_node->is_leaf = node->is_leaf;
    node->num = M;
    new_node->num = M;
    for (int i = 0; i < M; i++)
    {
        new_node->index_nodes[i] = node->index_nodes[M + i];
        new_node->childs[i] = node->childs[M + i];
    }
    int insert_num = node->index_nodes[M].value;
    BPlusTreeNode *parent = node->parent;
    //如果是根节点，分裂时需要产生一个新的父节点
    if (parent == NULL)
    {
        parent = BPlusTreeNode_new();
        parent->is_leaf = false;
        parent->num = 1;
        parent->index_nodes[0] = node->index_nodes[M];
        parent->childs[0] = node;
        parent->childs[1] = new_node;
        new_node->parent = parent;
        node->parent = parent;
    }
    else
    {
        new_node->parent = parent;
        int loc = 0;
        //找到在父节点中的插入位置
        while (loc < parent->num && insert_num > parent->index_nodes[loc].value)
            loc++;
        //插入key
        for (int i = parent->num - 1; i >= loc; i--)
            parent->index_nodes[i + 1] = parent->index_nodes[i];
        parent->index_nodes[loc] = node->index_nodes[M];
        loc++;
        //插入value
        for (int i = parent->num; i >= loc; i--)
            parent->childs[i + 1] = parent->childs[i];
        parent->childs[loc] = new_node;
        parent->num++; //在父节点中也需要添加一个节点
    }
    BPlusTreeNode *root = node; //返回的B+树根节点
    //从分裂节点向上遍历到根节点
    while (root->parent)
    {
        root = root->parent;
        //如果内部节点需要分裂
        if (root->num == 2 * M)
            root = BPlusTree_Split_inner(root);
    }
    return root;
}
//内部节点需要分裂
BPlusTreeNode *BPlusTree::BPlusTree_Split_inner(BPlusTreeNode *node)
{
    BPlusTreeNode *new_node = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    new_node->is_leaf = node->is_leaf;
    int i;
    for (i = 0; i < M - 1; i++)
    {
        new_node->index_nodes[i] = node->index_nodes[M + 1 + i];
        new_node->childs[i] = node->childs[M + 1 + i];
    }
    new_node->childs[i] = node->childs[2 * M];
    new_node->num = M - 1;
    node->num = M;
    BPlusTreeNode *parent = node->parent;
    //父节点为空，则需要新创建一个父节点
    if (!parent)
    {
        parent = BPlusTreeNode_new();
        parent->num = 1;
        parent->is_leaf = false;
        parent->index_nodes[0] = node->index_nodes[M];
        parent->childs[0] = node;
        parent->childs[1] = new_node;
        new_node->parent = parent;
        node->parent = parent;
    }
    else
    {
        new_node->parent = parent;
        int insert_num = node->index_nodes[M].value;
        int loc = 0;
        //找到在父节点中的插入位置
        while (loc < parent->num && insert_num > parent->index_nodes[loc].value)
            loc++;
        //插入key
        for (int i = parent->num - 1; i >= loc; i--)
            parent->index_nodes[i + 1] = parent->index_nodes[i];
        parent->index_nodes[loc] = node->index_nodes[M];
        loc++;
        //插入value
        for (int i = parent->num; i >= loc; i--)
            parent->childs[i + 1] = parent->childs[i];
        parent->childs[loc] = new_node;
        parent->num++;
    }
    return node;
}


//插入节点
BPlusTreeNode *BPlusTree::BPlusTree_Insert(BPlusTreeNode *root, const Record &record, int col)
{
    IndexNode indexNode = IndexNode_new(record, col);
    if (!root)
    {
        return NULL;
    }
    BPlusTreeNode *probe = root;
    while (!probe->is_leaf)
    {
        int loc = 0;
        //找到在内部节点中需要探索的子节点
        while (loc < probe->num && indexNode.value >= probe->index_nodes[loc].value)
            loc++;
        probe = probe->childs[loc];
    }
    int loc = 0;
    while (loc < probe->num && indexNode.value > probe->index_nodes[loc].value)
        loc++;
    if (probe->num == 0)//如果为空，直接插入
    {
        probe->index_nodes[0] = indexNode;
    }
    else//如果非空，需要移动
    {
        //插入key
        for (int i = probe->num - 1; i >= loc; i--)
            probe->index_nodes[i + 1] = probe->index_nodes[i];
        probe->index_nodes[loc] = indexNode;
    }
    probe->num++;
    if (probe->num == 2 * M)
    {
        root = BPlusTree_Split_leaf(probe);
    }
    return root;
}

/** 搜索特定属性值
 * root B+树根节点
 * value 属性值
 * result 结果数组 存放记录的主键
 * num 结果数
 */
void BPlusTree::SearchValueEqual(BPlusTreeNode *root, int value, int64_t *result, int &num)
{
    num = 0;
    if (!root)
        return;
    BPlusTreeNode *node = root;
    while (!node->is_leaf)
    { //不是叶节点，向下搜索
        int loc = 0;
        while (loc < node->num && value > node->index_nodes[loc].value)
            loc++;
        node = node->childs[loc];
    }
    //到达叶节点，顺着next指针往前搜索
    while (node)
    {
        for (int i = 0; i < node->num && num < MAX_RESULT_NUM; i++)
        {
            if (node->index_nodes[i].value > value)
                return;
            if (node->index_nodes[i].value == value)
                result[num++] = node->index_nodes[i].primary_key;
        }
        if (num == MAX_RESULT_NUM)
            return;
        node = node->next;
    }
}

/**搜索属性值在[left,right]范围内的记录
 */
void BPlusTree::SearchValueRange(BPlusTreeNode *root, int left, int right, int64_t *result, int &num)
{
    num = 0;

    BPlusTreeNode *node_left = root;
    BPlusTreeNode *node_right = root;

    //往下搜索，到达两个端点所在的叶节点
    while (!node_left->is_leaf)
    {
        int pos = 0;
        while (pos < node_left->num && left > node_left->index_nodes[pos].value)
            pos++;
        node_left = node_left->childs[pos];
    }
    while (!node_right->is_leaf)
    {
        int pos = node_right->num;
        while (pos > 0 && right < node_right->index_nodes[pos - 1].value)
            pos--;
        node_right = node_right->childs[pos];
    }

    //移动node_left指针直到node_right
    while (node_left != node_right)
    {
        if (node_left == NULL)
            std::cout << "error";
        for (int i = 0; i < node_left->num && num < MAX_RESULT_NUM; i++)
        {
            if (node_left->index_nodes[i].value >= left)
                result[num++] = node_left->index_nodes[i].primary_key;
        }
        if (num == MAX_RESULT_NUM)
            return;
        node_left = node_left->next;
    }

    // node_left和node_right相遇
    for (int i = 0; i < node_left->num && num < MAX_RESULT_NUM; i++)
    {
        if (left <= node_left->index_nodes[i].value && node_left->index_nodes[i].value <= right)
            result[num++] = node_left->index_nodes[i].primary_key;
    }
}

//保存为第col列属性创建的B+树，创建或更新索引时调用此函数
void BPlusTree::WriteBPlusTree(BPlusTreeNode *root, int col)
{
    char index_file_path[20];
    sprintf(index_file_path, "%s%d", INDEX_FILE_PATH, col);
    //删除原来的索引文件
    if (access(index_file_path, F_OK) == 0)
    {
        if (remove(index_file_path) == -1)
        {
            std::cout << "删除失败";
            throw "In BPlusTree::WriteBPlusTree(),remove error";
        }
    }
    int fd = open(index_file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw "In BPlusTree::WriteBPlusTree(),open error";
    WriteBPlusTreeNode(fd, root);//从根节点开始写入文件
    close(fd);
}

//将B+树节点写入文件
void BPlusTree::WriteBPlusTreeNode(int &fd, BPlusTreeNode *node)
{
    if (node == NULL || fd == -1)
    {
        return;
    }
    if (write(fd, node, sizeof(BPlusTreeNode)) == -1)
        throw "In BPlusTree::WriteBPlusTreeNode(),write error";
    for (int i = 0; i <= node->num; i++)
        WriteBPlusTreeNode(fd, node->childs[i]);
}

//读取B+树
BPlusTreeNode *BPlusTree::ReadBPlusTree(int col)
{
    BPlusTreeNode *leaf_node_pre = NULL; //保存前一个叶节点
    char index_file_path[20];
    sprintf(index_file_path, "%s%d", INDEX_FILE_PATH, col);
    if (access(index_file_path, F_OK) == -1)
        return NULL;
    int fd = open(index_file_path, O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return NULL;
    BPlusTreeNode *node = ReadBPlusTreeNode(fd, leaf_node_pre);
    close(fd);
    return node;
}

//读取B+树节点
BPlusTreeNode *BPlusTree::ReadBPlusTreeNode(int &fd, BPlusTreeNode *&leaf_node_pre)
{

    BPlusTreeNode *node = BPlusTreeNode_new();
    if (node == NULL)
        return NULL;
    if (read(fd, node, sizeof(BPlusTreeNode)) == -1)
        return NULL;
    if (node->is_leaf)//如果是叶子节点
    {
        if (leaf_node_pre == NULL)
            leaf_node_pre = node;
        else
        {
            leaf_node_pre->next = node;
            leaf_node_pre = node;
        }
        return node;
    }
    for (int i = 0; i <= node->num; i++)
        node->childs[i] = ReadBPlusTreeNode(fd, leaf_node_pre);
    return node;
}