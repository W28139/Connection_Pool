#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <fstream>
#include <atomic>   // 增加原子变量头文件
#include "CommonConnectionPool.hpp"
#include "Connection.hpp"

using namespace std;
using namespace std::chrono;

// 测试配置
const int TOTAL_DATA = 50000; 
const string DB_IP = "127.0.0.1";
const int DB_PORT = 3306;
const string DB_USER = "root";
const string DB_PWD = "123456"; 
const string DB_NAME = "db_proxy";

// 保存结果到CSV
void saveToCSV(int thread_num, string type, double time_ms) {
    ofstream ofs("test_results.csv", ios::app);
    if (ofs.is_open()) {
        ofs << thread_num << "," << type << "," << time_ms << endl;
        ofs.close();
    }
}

// 1. 真正的“短连接”测试：每次发送 SQL 都重新创建并销毁连接
void testWithoutPool(int count, int thread_num) {
    atomic<int> success_cnt(0);
    auto start = high_resolution_clock::now();
    vector<thread> threads;

    for (int i = 0; i < thread_num; ++i) {
        threads.emplace_back([&, count, thread_num]() {
            for (int j = 0; j < count / thread_num; ++j) {
                // 【核心改进】：连接的创建和销毁放在循环内部
                Connection conn; 
                if (conn.connect(DB_IP, DB_PORT, DB_USER, DB_PWD, DB_NAME)) {
                    string sql = "insert into user(name, age) values('worker', 20)";
                    if(conn.update(sql)) {
                        success_cnt++;
                    }
                }
                // conn 析构时会自动断开连接
            }
        });
    }
    for (auto &t : threads) t.join();

    auto end = high_resolution_clock::now();
    duration<double, milli> ms = end - start;
    cout << thread_num << " 线程 | 不使用连接池(短连接) | 耗时: " << ms.count() << " ms | 成功插入: " << success_cnt << endl;
    
    saveToCSV(thread_num, "WithoutPool", ms.count());
}

// 2. 使用连接池的测试 (保持不变，体现复用优势)
void testWithPool(int count, int thread_num) {
    atomic<int> success_cnt(0);
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    auto start = high_resolution_clock::now();
    vector<thread> threads;

    for (int i = 0; i < thread_num; ++i) {
        threads.emplace_back([&, count, thread_num]() {
            for (int j = 0; j < count / thread_num; ++j) {
                // 【核心优势】：直接从池里拿现成的连接，不需要 connect
                shared_ptr<Connection> sp = cp->getConnection();
                if (sp) {
                    string sql = "insert into user(name, age) values('worker', 20)";
                    if(sp->update(sql)) {
                        success_cnt++;
                    }
                }
            }
        });
    }
    for (auto &t : threads) t.join();

    auto end = high_resolution_clock::now();
    duration<double, milli> ms = end - start;
    cout << thread_num << " 线程 | 使用连接池(长连接复用) | 耗时: " << ms.count() << " ms | 成功插入: " << success_cnt << endl;
    
    saveToCSV(thread_num, "WithPool", ms.count());
}

int main(int argc, char* argv[]) {
    int data_count = TOTAL_DATA;
    if(argc > 1) data_count = atoi(argv[1]);

    // 定义测试梯度 (针对你的 i7-4070，建议测到 64 线程)
    vector<int> thread_counts = {1, 4, 8, 16, 32, 64};

    cout << "--- 数据库连接池压力测试 (总数据量: " << data_count << ") ---" << endl;

    for (int threads : thread_counts) {
        cout << "\n>>> 正在测试 " << threads << " 线程并发..." << endl;
        
        // 运行不使用连接池测试
        testWithoutPool(data_count, threads);
        
        // 关键：在两次压测之间睡眠一下，让系统回收资源，防止端口耗尽
        this_thread::sleep_for(seconds(2));

        // 运行使用连接池测试
        testWithPool(data_count, threads);
        
        this_thread::sleep_for(seconds(2));
    }

    return 0;
}