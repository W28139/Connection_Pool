#include<iostream>
#include <mysql/mysql.h> 
#include"Connection.hpp"
#include<string>
using namespace std;

int main()
{
    Connection conn;
    char sql[1024]={0};
    string s1;
    string s2;
    int a;
    sprintf(sql,"insert into User values('%s',%d,'%s')",
        s1,a,s2);        
    conn.connect("127.0.0.1",3306,"root","123456","chat");
    conn.update(sql);

    return 0;
}