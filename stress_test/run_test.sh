#!/bin/bash

# 1. 临时开启 TCP 端口快速回收 (解决 connect fail 的核心)
echo "优化系统网络参数..."
sudo sysctl -w net.ipv4.tcp_tw_reuse=1
sudo sysctl -w net.ipv4.ip_local_port_range="1024 65535"

# 2. 临时优化 MySQL 刷盘策略 (解决写入慢的核心)
echo "优化 MySQL 写入性能..."
mysql -u root -p123456 -e "SET GLOBAL innodb_flush_log_at_trx_commit = 2;"

# 3. 编译
echo "正在编译..."
g++ -o press_test stress_test.cpp \
    ../src/CommonConnectionPool.cpp \
    ../src/Connection.cpp \
    -I../include \
    -lmysqlclient -lpthread -std=c++11

if [ $? -ne 0 ]; then
    echo "编译失败！"
    exit 1
fi

# 4. 运行
echo "开始压力测试..."
rm -f test_results.csv
./press_test 10000
