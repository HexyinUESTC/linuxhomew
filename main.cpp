#include <iostream>
#include<string>
#include<stdlib.h>
#include "table.h"
#include "CLThread.h"
#include "CLExeFuncInsert.h"
#include "CLExeFuncSearch.h"

int main()
{
    //测试
    table* m_table=table::GetTable();
    //m_table->InsertRecord();
    //m_table->SearchRecord(1,1000,1);
    m_table->SearchRecord(1,1000,1);
    //CLExecutiveFunctionProvider* inserter=new CLExecutiveFunctionInsert();
    //CLExecutiveFunctionProvider* Search=new CLExecutiveFunctionSearch(10,1000,8);
    //CLExecutive *pThread=new CLThread(Search);
    //pThread->Run();
    //pThread->WaitForDeath();
}
