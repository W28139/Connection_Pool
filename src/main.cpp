#include<iostream>
#include <mysql/mysql.h> 
#include"Connection.hpp"
#include<string>
#include"CommonConnectionPool.hpp"
using namespace std;

int main()
{
    ConnectionPool *cp = ConnectionPool::getConnectionPool();

    return 0;
}