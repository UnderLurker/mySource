# 单线程日志类（可接收多线程日志）

类内部结构使用了一个list，用来做缓存，若工作线程抢到了缓存列表锁，则将内容移动至循环链表，写入日志

日志级别:

| 级别     | 等级 |
|--------|----|
| INFO   | 0  |
| DEBUG  | 1  |
| WARNING | 2  |
| ERROR  | 3  |

可手动设置日志写入等级，低于所设置等级不写入日志，依据宏的第一个参数区分要使用哪个日志对象

使用方法:

```c++
CLOG_DECLARE_1(print, "E:/code/mySource/log/")

void print(int number, const char* content) {
    int len = number;
    while(len--){
        LogDebug(print, "%s%d", content, len);
        LogInfo(print, "%s%d", content, len);
        LogWarn(print, "%s%d", content, len);
        LogError(print, "%s%d", content, len);
    }
}

void clogTest() {
    CLOG_TYPE(print, ERROR)
    thread th0(print, 5, "this is 0/");
    thread th1(print, 5, "this is 1/");
    th0.join();
    th1.join();
}

int main(){
    clogTest();
    CLOG_END(print)
    return 0;
}
```