// #include "Util.h"
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <utility>
#include <thread>

#include "base64.h"
#include "BTree.h"
#include "heap.h"
#include "RBTree.h"
#include "SingleLinkList.h"
#include "Trie.h"
#include "SPSCDeque.h"
#include "LockFreeQueue.h"
using namespace std;
using namespace myUtil;

void base64() {
    string str = Base64::encoding("123");
    cout << str << endl;
    string str1 = Base64::decoding(str);
    cout << str1 << endl;
}

void rbtree() {
    RBTree<int, int, less<>> p;
    // p.insert(1,1);
    // p.insert(2,1);
    // p.insert(3,1);
    // p.insert(4,1);
    // p.insert(5,1);
    // p.insert(6,1);
    // p.insert(7,1);
    p.insert(pair<int, int>(1, 1));
    p.insert(pair<int, int>(2, 1));
    p.insert(pair<int, int>(3, 1));
    p.insert(pair<int, int>(4, 1));
    p.insert(pair<int, int>(5, 1));
    p.insert(pair<int, int>(6, 1));
    p.insert(pair<int, int>(7, 1));
    p.print();
}

void singleLinkList() {
    SingleLinkList<int> list;
    for (int i = 0; i < 4; i++) {
        list.push(i + 1);
    }
    cout << endl;
    cout << "length:" << list.length() << endl;
    for (auto& it : list) {
        cout << it.value() << " ";
    }
    cout << endl;

    SingleLinkList<int> a(list);
    cout << "length:" << a.length() << endl;
    for (auto it = a.begin(); it != a.end(); it++) {
        cout << it->value() << " ";
    }

    vector<int> b = {1, 2, 3, 4, 5};
    SingleLinkList<int> c(&(*b.begin()), &(*b.end()));
    cout << endl;
    cout << "length:" << c.length() << endl;
    for (auto it = c.begin(); it != c.end(); it++) {
        cout << it->value() << " ";
    }

    cout << endl;
    auto d = list;
    cout << "length:" << d.length() << endl;
    for (auto it = d.begin(); it != d.end(); it++) {
        cout << it->value() << " ";
    }
}

void heapTest() {
    vector<int> input {5, 3, 10, 2, 1, 0, 8, 4};
    heap<int, less<>> a; // or heap<int, greater<>> a
    a.print();
    for (int i = 10; i >= -5; i--) {
        a.push_back(i);
    }
    a.pop_back();
    a.push_back(-20);
    a.print();
    cout << "heap.begin:";
    for (auto it = a.begin(); it != a.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    cout << "heap.: :";
    for (auto item : a) {
        cout << item << " ";
    }
    cout << endl;
    cout << "heap.rbegin:";
    for (auto it = a.rbegin(); it != a.rend(); it++) {
        cout << *it << " ";
    }
    cout << endl;
    a.erase(1);
    a.print("erase 1");
    a.erase(17);
    a.print("erase 17");
    a.erase(14);
    a.print("erase 14");
    a.erase(a.begin());
    a.print("erase begin");
    a.erase(a.end());
    a.print("erase end");
    a.erase(a.rbegin());
    a.print("erase rbegin");
    a.erase(a.rend());
    a.print("erase rend");
    a.pop_front();
    a.print("pop_front");
}

class UtilTest : public testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(UtilTest, UtilTest001) {
    std::string test1 = "apple";
    std::string test2 = "apply";
    std::string test3 = "app";
    Trie dict;
    dict.insert(test1);
    dict.insert(test2);
    EXPECT_TRUE(dict.search(test1));
    EXPECT_TRUE(dict.startsWith(test3));
}

TEST_F(UtilTest, SPSCDequeTest) {
    std::atomic<int32_t> writeCount {0};
    std::atomic<int32_t> readCount {0};
    SPSCQueue<int32_t> queue(1000);
    constexpr int32_t number = 10000;
    std::thread write([&]() {
        for (int i = 0; i < number; i++) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
            writeCount++;
        }
    });
    std::thread read([&]() {
        int32_t i = 0;
        while (readCount < number) {
            if (auto val = queue.pop()) {
                readCount++;
                EXPECT_FALSE(val && val != i);
                // if (val && val != i) {
                //     cout << "not equal!!!" << " " << i << endl;
                // }
                i++;
            } else {
                std::this_thread::yield();
            }
        }
    });
    write.join();
    read.join();
    EXPECT_EQ(writeCount.load(), number);
    EXPECT_EQ(readCount.load(), number);
}

void testMultiProducerMultiConsumer() {
    std::cout << "=== 测试5: 多生产者多消费者测试 ===\n";
    const int NUM_PRODUCERS = 10;
    const int NUM_CONSUMERS = 10;
    const int ITEMS_PER_PRODUCER = 2000000;
    const int TOTAL_ITEMS = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    LockFreeQueue<int> queue(ITEMS_PER_PRODUCER);
    
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::atomic<int> producedCount{0};
    std::atomic<int> consumedCount{0};
    
    // 用于验证的计数器
    std::vector<int> received(TOTAL_ITEMS, 0);
    std::mutex resultMutex;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 生产者
    for(int i = 0; i < NUM_PRODUCERS; i++) {
        producers.emplace_back([&, i]() {
            for(int j = 0; j < ITEMS_PER_PRODUCER; j++) {
                int value = i * ITEMS_PER_PRODUCER + j;
                while(!queue.push(value)) {
                    std::this_thread::yield();
                }
                producedCount++;
            }
        });
    }
    
    // 消费者
    for(int i = 0; i < NUM_CONSUMERS; i++) {
        consumers.emplace_back([&]() {
            int localCount = 0;
            while(consumedCount < TOTAL_ITEMS) {
                if(auto val = queue.pop()) {
                    std::lock_guard<std::mutex> lock(resultMutex);
                    received[*val]++;
                    localCount++;
                    consumedCount++;
                } else {
                    if(producedCount >= TOTAL_ITEMS) {
                        break;
                    }
                    std::this_thread::yield();
                }
            }
        });
    }
    
    for(auto& t : producers) t.join();
    
    // 等待所有消费者完成
    for(auto& t : consumers) t.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证每个值都被消费了一次
    bool allGood = true;
    for(int i = 0; i < TOTAL_ITEMS; i++) {
        if(received[i] != 1) {
            std::cerr << "错误: 值 " << i << " 被消费了 " 
                      << received[i] << " 次\n";
            allGood = false;
        }
    }
    
    assert(allGood);
    assert(consumedCount == TOTAL_ITEMS);
    
    std::cout << "生产者: " << NUM_PRODUCERS << " 个\n";
    std::cout << "消费者: " << NUM_CONSUMERS << " 个\n";
    std::cout << "总元素: " << TOTAL_ITEMS << "\n";
    std::cout << "耗时: " << duration.count() << " ms\n";
    std::cout << "✓ 多生产者多消费者测试通过\n\n";
}

TEST_F(UtilTest, LockFreeQueueTest) {
    testMultiProducerMultiConsumer();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
