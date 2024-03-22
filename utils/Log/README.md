# ���߳���־�ࣨ�ɽ��ն��߳���־��

���ڲ��ṹʹ����һ��list�����������棬�������߳������˻����б������������ƶ���ѭ������д����־

��־����:

| ����     | �ȼ� |
|--------|----|
| INFO   | 0  |
| DEBUG  | 1  |
| WARNING | 2  |
| ERROR  | 3  |

���ֶ�������־д��ȼ������������õȼ���д����־�����ݺ�ĵ�һ����������Ҫʹ���ĸ���־����

ʹ�÷���:

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