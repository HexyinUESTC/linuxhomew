#ifndef CLEXEFUNCINSERT_H
#define CLEXEFUNCINSERT_H
#include "CLExeFuncProvider.h"
#include "table.h"

class CLExecutiveFunctionInsert: public CLExecutiveFunctionProvider {
public:
    CLExecutiveFunctionInsert() {}

    virtual ~CLExecutiveFunctionInsert() {}

    virtual void RunExecutiveFunction() {
        table* m_table = table::GetTable();
        m_table->InsertRecord();
    }
};


#endif // CLEXECUTIVEFUNCTIONINSERT_H
