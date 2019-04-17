//
// Created by 周亮 on 2019/4/17.
//

#ifndef LL_H_
#define LL_H_
#include<string>
#include<map>
#include<vector>
#include "base.h"
using namespace std;

//LL型文法的特例,主要是方法的不同
namespace llGramer{
    using namespace baseGramer;

    void clearNull(const vector<int>& a, vector<int>& result, int value = -1);
    //LL文法的表达式
    class ExpressLL :public Express
    {
    public://方法
        //构造函数
        ExpressLL(int* expressions, int length);
        ExpressLL(const vector<int>& expression);


    private:

    };

    //LL的符号表示
    class ElemLL :public Elem
    {
    public://属性
        vector<ExpressLL> expression_of_set;//这个非终结符的集合

        //在[begin,end）中第index下标值相同的值的个数
        //返回的个数
       // int getSameCount(int begin, int end, int index);
    public://方法

        //构造函数
        ElemLL(string name, int key, bool isFinally);

        //添加表达式
        bool addExpression(ExpressLL expression);

        //提取左因子
        /*
        例子 A->1 2 3 4|1 2 4 5|1 2 5|2 4 5|2 4 5 7|2 4
        这个提取后
        result：1 2
        begin：0
        count:3
        返回值是true说明现在这个非终结符有左因子,并且左因子是result,在A中是从[begin,bengin+count）
        返回值为false说明现在这个非终结符没有左因子了
        */
        bool extractLeftFactor(vector<int>&result, int& begin, int& count);

        //给左因子排序
        void sortExpresssion();

    };


    //继承FindTable主要是为了重写方法
    class GramerLL{

    private://属性

        map<string, int> name_of_keys;//利用map来快速查找

        vector<ElemLL> elems;//各个元素


        //这个在提取左因子之后需要立刻的构造
        vector<int> finally_charIndex;//终结符的表
        vector<int> not_finally_charIndex;//非终结符的表

        //LL文法特有的
        //前面一个int对应的findTable里面的非终结符的下标，后一个vector<int>对应的是终结符号的下标
        vector<vector<int>> First;
        vector<vector<int>> Follow;

        //LL分析预测表
        //二维数组，第一个为非终结符号
        vector<vector<int>> predictTable;//预测表对应上面的两个vector<int>

        //保存记录，主要的是计算First需要使用
        vector<int> firstTemp;

    private://方法
        //find查找，找到的话返回下标，
        //找不到则返回-2
        //-1表示为空，空使用@来表示
        int findName(string name_of_elem);

        //添加符号,可以是终结符也可以是非终结符
        int addElem(string name, bool isFinally);


        //消除直接左递归
        //先检查elems[i]有没有直接左递归，如果就需要将他消除,最后会在elems后面添加一些新的东西
        void clearLeft(int elemIndx);

        //替换,消除直接左递归的第一步替换
        void replaceEx(int oldElemIndex, int replaceIndex);

        //消除左递归
        void clearLeftRE();

        //消除左因子
        void clearLeftFactor();

        //修改元素
        void updateElem(int elemIndex, int begin, int end, vector<int>& factory);

        //二表生成
        //根据findtable和Elems来产生终结符和非终结符下标,主要是为了后来的查表使用
        //同时在这里初始化一下First和Follw表
        void get_tow_charIndex();


        //first,follow集合
        void createFirstSet();//获得First集合
        void createFollowSet();//生成Follow集合

        //计算elem[elemIndex]的first集合,并且将结果放在First[elemIndex中]
        //返回值，如果是true，说明这个不是空串（elemIndex！=-1）;
        //		  如果是false，说明这个是空串，elemIndex=-1,级这个First集合为（-1）空
        bool getFirst(int elemIndex);

        //计算elems[elemIndex]的expressIndex下标的表达式的first集合，结果放在result里面
        //choose对tempFirst有影响，区别的第一次还是第二次针对的是这样的句型：A->BAb | a     B->b | @
        void caluteExpressionFirst(int elemIndex, int expressionIndex, vector<int>& result,bool choose=true);

        //获得一个表达式子的First;
        void getFollowByFirst(int elemIndex, ExpressLL express, int begin, vector<int>& result);
        //调整Follo集合，将follow[index]的非终结符号添加
        void adjustFollow(int index);


        //预测分析表
        void get_predictTable();//产生预测分析表



    public://方法

        //构造函数
        GramerLL(string* generate, int length);

        //根据预测分析表进行判断
        bool sim(char* testString);

    };







}

#endif
