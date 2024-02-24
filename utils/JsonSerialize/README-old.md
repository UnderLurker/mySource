# Json解析器
## 类结构用途
| 结构名称 |           用途           |
|:----:|:----------------------:|
|   JsonType   |      表示是哪种Json类型       |
|JNumber,JString,JArray,JBool,JObj|      用来偏特化萃取返回值类型      |
|JsonSerialize| 对Json文件进行解析的类，提供了各种方法  |
|JsonValue| 解析出的数据所继承的基类，使用指针存储派生类 |
|JsonKey|        解析出数据的键值        |
|JsonNumber...|  真正存储数据的结构，继承自JsonKey  |
|JsonItem_t,JsonItem_t_type|       用来萃取返回值类型        |

## JsonSerialize的使用方法
   
GetValueByKey用来获取值内容，模板参数为对应的类型，本解析器使用的字符类型均为 *wstring* 或 *wchar_t*   
同时也提供了保存操作，支持将当前的内容保存到json文件中     
提供了更改json值的方法。使用方法见setValueByKey     
insert方法可以插入新的项目

```c++
JsonSerialize json(R"(F:\C++\clion\mySource\sample\test.json)");
json.Load();

auto res = json.GetValueByKey<JArray>(L"configurations")[0].GetValueByKey<JArray>(L"setupCommands")[0];
wcout<<res<<endl;
wcout<<json.GetValueByKey<JString>(L"version")<<endl;

JsonValue* ins=new JsonString(L"abcd");
json.setValueByKey(L"version", ins);

JsonValue* number=new JsonNumber(20);
json.insert(Number,L"number",number);

json.saveJson(R"(F:\C++\clion\mySource\sample\test1.json)");
```

目前该类支持将json直接输入得到对应的对象，使用到了反射的内容，使用方法如下：
```c++
class A :public RObject{
public:
    double type;
    wstring request;
    wstring program;
};

REGISTER_REFLEX(A)
REGISTER_REFLEX_FIELD(A,double,type)
REGISTER_REFLEX_FIELD(A,wstring,request)
REGISTER_REFLEX_FIELD(A,wstring,program)
int main() {
    JsonSerialize json(R"(F:\C++\clion\mySource\sample\test.json)");
    json.Load();
    A* b = json.toRObject<A>("A");
    wcout<<b->type<<L"\t"<<b->program<<L"\t"<<b->request<<endl;
    return 0;
} 
```
当然，只是简单的类（没有成员变量为自定义类型的类才可以实现）