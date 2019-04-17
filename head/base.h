//
// Created by 周亮 on 2019/4/17.
//

#ifndef BASE_H_
#define BASE_H_
#include<string>
#include <vector>

//基础的，不管是LR还是LL的分析都需要的
//对LL和LR的各个结构都是继承这个来修改的
namespace baseGramer{
    using namespace std;
    //符号，包括终结符和非终结符号,使用模板是因考虑到LL和LR的表达式不一样的而采用的
    class Elem {
    public://属性
        int key;//和上面的FindTable的Map中int的值对应
        std::string name;//符号的名字
        bool isFinally;//判断是不终结符，是则为true;
    public://方法

        //给这个Elem添加产生式
        //bool addExpression(expr expression);
        //构造函数
        Elem(string name, int key, bool isFinally);

    };


    //表达式,产生式子
    class Express{
    public://属性
        vector<int> expression;//表达式
    public://方法
        Express(int* expressions, int length);

        Express(const vector<int>& expression);

        //[]运算符重载
        int& operator[](int index);

        //重载大于号
        //this>b返回true
        bool operator>(const Express& b);
    };
}
//解析字符串，主要是一些函数
namespace publicTools{
    using namespace std;
    //解析所有的产生式：A->BAB|CAC得到的结果是A,BAB,CAC
    //?作为转义字符,区别|和?|
    vector<string>& parseProductions(string EX, vector<string>& productions);

    //单独解析,Ex为BAB,结果为B,A,B
    //@表示空串,$...$表示...是一个整体，否则一个字符串一个字符串的解析
    vector<string>& parseElem(string production, vector<string>& elems);

    bool isInA(const std::vector<int>& a, int b);
    void connetVector(std::vector<int>& a, const std::vector<int>& b);

}

#endif //SYNTAXANALYSIS_BASE_H
