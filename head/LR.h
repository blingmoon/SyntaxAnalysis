//
// Created by 周亮 on 2019/4/17.
//

#ifndef LR_H_
#define LR_H_
#include<string>
#include<map>
#include<vector>
#include<algorithm>
#include "base.h"
using namespace std;


//LR型文法的特例,主要是方法的不同
namespace lRGramer{
    using namespace baseGramer;

    void clearNull(const vector<int>& a, vector<int>& result, int value = -1);
    //LR文法的表达式
    class ExpressLR :public Express {
    public://方法
        //构造函数
        ExpressLR(int* expressions, int length);
        ExpressLR(const vector<int>& expression);


    private:

    };

    //LR的符号表示
    class ElemLR :public Elem {
    public://属性
        vector<ExpressLR> expression_of_set;//这个非终结符的集合

        //在[begin,end）中第index下标值相同的值的个数
        //返回的个数
        //int getSameCount(int begin, int end, int index);
    public://方法

        //构造函数
        ElemLR(string name, int key, bool isFinally);

        //添加表达式
        bool addExpression(ExpressLR expression);

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
        //bool extractLeftFactor(vector<int>&result, int& begin, int& count);

        //给左因子排序
        //void sortExpresssion();

    };




    //LR的项目
    class Project {
    public://属性
        int elemKey;
        int expressIndex;
        int projectPoint;
    public://方法
        Project(int elemKey, int expressIndex, int peojectPoint);
        bool operator< (const Project& project)const;

        bool operator==(const Project& project)const ;
        bool operator!=(const Project& project) const ;
    };

    bool cmp(Project& a, Project& b);
    class DFAState {

    public://属性
        vector<Project> projects;//项目集

        //状态转移的，next的长度为elem元素的长度
        int* next;
        int elemcount;
        int statesCount;//状态集合的下标，主要是为了提高效率;

    public://方法
        //后面会将projects排一次序
        DFAState(const vector<Project>& project,int index,int elemCount);

        //拷贝构造函数
        DFAState(const DFAState& a);
        ~DFAState();
        //判断两个项目集是不是一样,一样的话返回true不一样的话返回false
        bool operator==(vector<Project>& b);

    private://方法


    };


    //预测分析表的内容

    //移进,归约,错误
    enum Type{
        SMOVE,REDUCE,ERROR,ACC
    };

    //SMOVE只需要一个状态，REDUCE需要表达式的位置,同时状态和表达式两者不会共存，应该为联合体
    struct ReducebyExpress
    {
        int elemIndex;
        int expressIndex;

    };

    union Choose
    {
        int state;
        ReducebyExpress R;
    };

    struct Action
    {
        Type type;
        Choose choose;
    };


    //继承FindTable主要是为了重写方法
    class GramerLR{

    private://属性

        map<string, int> name_of_keys;//利用map来快速查找

        vector<ElemLR> elems;//各个元素


        //这个在提取左因子之后需要立刻的构造
        vector<int> finally_charIndex;//终结符的表
        vector<int> not_finally_charIndex;//非终结符的表

        //前面一个int对应的findTable里面的非终结符的下标，后一个vector<int>对应的是终结符号的下标
        vector<vector<int>> First;
        vector<vector<int>> Follow;

        //保存记录，主要的是计算First需要使用
        vector<int> firstTemp;


        //DFA状态;
        vector<DFAState> states;//DFA状态集合


        //减句柄的表,指导动作的表
        vector<vector<Action>>  actionTable;




    private://方法
        //find查找，找到的话返回下标，
        //找不到则返回-2
        //-1表示为空，空使用@来表示
        int findName(string name_of_elem);

        //添加符号,可以是终结符也可以是非终结符
        int addElem(string name, bool isFinally);

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
        void caluteExpressionFirst(int elemIndex, int expressionIndex, vector<int>& result, bool choose = true);

        //获得一个表达式子的First;
        void getFollowByFirst(int elemIndex, ExpressLR express, int begin, vector<int>& result);
        //调整Follo集合，将follow[index]的非终结符号添加
        void adjustFollow(int index);


        //构造DFA
        void createDFAStates();

        //smove运算，得到的是一个状态集合,求input状态经过elem[elemIndex]后得到的DFA项目集合
        void smove(DFAState& input,int elemIndex,vector<Project>& reasult );

        //求空闭包,求result集合的空闭包，会添加，最后会增加到result里面
        void closure(vector<Project>& result);

        void createActionTable();


    public://方法

        //构造函数
        GramerLR(string* generate, int length);

        //根据预测分析表进行判断
        bool sim(char* testString);

    };


    bool isInA(const vector<Project>& a, const Project& b);
    void connetVector(vector<Project>& a, const vector<Project>& b);




}

#endif