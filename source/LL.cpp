//
// Created by 周亮 on 2019/4/17.
//
#include"../head/LL.h"
#include<sstream>
#include<iostream>
#include<algorithm>
#include<stack>
using namespace std;
using namespace llGramer;
using namespace publicTools;




//LL的特殊化

//表达式
ExpressLL::ExpressLL(int* expressions, int length) :Express(expressions, length){}

ExpressLL::ExpressLL(const vector<int>& expression) : Express(expression){}


//LL符号
ElemLL::ElemLL(string name, int key, bool isFinally) : Elem(name, key, isFinally){}

bool ElemLL::addExpression(ExpressLL expression){

    this->expression_of_set.push_back(expression);
    return true;
}

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
bool ElemLL::extractLeftFactor(vector<int>&result, int& begin, int& count){

    //排序一下，
    //插入排序
    this->sortExpresssion();
    //this->expression_of_set;
    //初始化一下
    result.clear();
    begin = -1;
    count = 1;//是有多少个元素有这个前缀

    //begin开始有了
    for (int i = 0; i < this->expression_of_set.size() - 1; i++){
        if (expression_of_set[i][0] == expression_of_set[i + 1][0]){
            begin = i;
            break;
        }

    }
    //没有进入if中，说明这个没有公共左因子了
    if (begin == -1) return false;



    //开始计算了,第一个前缀一定是最长的
    for (int i = begin; i < this->expression_of_set.size() - 1; i++){
        if (expression_of_set[i][0] == expression_of_set[i + 1][0]) count++;
        else
        {
            break;
        }
    }

    //先将第一个压入栈
    result.push_back(expression_of_set[begin][0]);

    for (int i = 1; i < this->expression_of_set[begin].expression.size(); i++){
        if (this->expression_of_set[begin][i] == this->expression_of_set[begin + count - 1][i]){
            result.push_back(this->expression_of_set[begin][i]);
        }
        else
        {
            break;
        }

    }

    return true;





}

//排序,给elem的expressSet排序
//给左因子排序
void ElemLL::sortExpresssion(){
    //采用插入排序方式
    for (int i = 1; i < this->expression_of_set.size(); i++){

        int insertIndex;
        ExpressLL temp = expression_of_set[i];
        for (insertIndex = i - 1; insertIndex >= 0; insertIndex--){
            if (expression_of_set[insertIndex] > temp){
                expression_of_set[insertIndex + 1] = expression_of_set[insertIndex];
            }
            else
            {
                break;
            }
        }
        expression_of_set[insertIndex + 1] = temp;



    }

}




//LL文法
//构造函数，重中之中,相当于主函数了
GramerLL::GramerLL(string* generate, int length){
    //1.解析所有字符串
    vector<vector<string>> vectorGenrates;//这个是用来存放各个表达式

    //解析各个表达式,第一个是非终结符号，后面的是产生式
    for (int i = 0; i < length; i++){
        vector<string> temp;
        parseProductions(generate[i], temp);
        vectorGenrates.push_back(temp);
    }

    //2.添加非终结符添加到find表中,开始的非终结符就是length个
    for (int i = 0; i < length; i++){
        addElem(vectorGenrates[i][0], false);
    }

    //3.一个个解析产生式,会添加到find表中
    vector<string> result_of_name;
    for (int i = 0; i < length; i++){

        //ElemLL N_F_E = elems[findName(vectorGenrates[i][0])];//非终结符，接下来要对这个终结符进行exp的添加

        //对genrates[i]进行分析；
        for (int j = 1; j < vectorGenrates[i].size(); j++){
            parseElem(vectorGenrates[i][j], result_of_name);
            vector<int> exp;
            for (int m = 0; m < result_of_name.size(); m++){
                int index = findName(result_of_name[m]);
                if (index == -2){
                    index = addElem(result_of_name[m], true);
                }
                exp.push_back(index);

            }
            elems[findName(vectorGenrates[i][0])].addExpression(ExpressLL(exp));
        }
    }

    //4.消除左递归
    this->clearLeftRE();
    //5.消除左因子
    this->clearLeftFactor();

    addElem("#", true);
    //查看输入是不正确
    //测试一下：
    string finnalyC = "finnaly:";
    for (int i = 0; i < elems.size(); i++){
        if (!elems[i].isFinally){
            cout << elems[i].name << "->";
            for (int j = 0; j < elems[i].expression_of_set.size(); j++){
                for (int m = 0; m < elems[i].expression_of_set[j].expression.size(); m++){
                    if (elems[i].expression_of_set[j].expression[m] != -1){
                        cout << elems[elems[i].expression_of_set[j].expression[m]].name;
                    }
                    else
                    {
                        cout << "@";
                    }
                }
                cout << " | ";
            }
            cout << endl;
        }
        else
        {
            finnalyC = finnalyC + " " + elems[i].name;
        }
    }
    cout << finnalyC << endl;

    //6.两表生成（在5的时候就不会再添加新的符号了）
    //同时在这里初始化一下First和Follw表
    get_tow_charIndex();

    //7.求first集合
    this->createFirstSet();

    //检查一下，first集合
    for (int i = 0; i < elems.size(); i++){
        cout << "First[" << elems[i].name << "]={";
        for (int j = 0; j < First[i].size(); j++){
            if (First[i][j] == -1) cout << "@ ";
            else
            {
                cout << elems[First[i][j]].name << " ";
            }
        }
        cout << "}\n";
    }

    //8.求follow集合
    //将#送到开始符号
    this->Follow[0].push_back(findName("#"));
    this->createFollowSet();


    for (int i = 0; i < elems.size(); i++){

        if (elems[i].isFinally) continue;

        cout << "Follow[" << elems[i].name << "]={";
        for (int j = 0; j < Follow[i].size(); j++){
            if (Follow[i][j] == -1) cout << "@ ";
            else
            {
                cout << elems[Follow[i][j]].name << " ";
            }
        }
        cout << "}\n";
    }
    //9.构造预测分析表
    this->get_predictTable();


    for (int i = 0; i < predictTable.size(); i++){
        for (int j = 0; j < predictTable[i].size(); j++){
            cout << predictTable[i][j] << " ";
        }
        cout << endl;
    }

}




//find查找，找到的话返回下标，
//找不到则返回-2
//-1表示为空，空使用@来表示
int GramerLL::findName(string name_of_elem){
    int result = -2;
    if (name_of_elem == "@"){
        result = -1;
    }
    else
    {
        if (this->name_of_keys.find(name_of_elem) != name_of_keys.end()){
            result = name_of_keys[name_of_elem];
        }
    }

    return result;


}



//添加将name添加到表中，并且将在elem中生成对应的元素，添加到elems中
int GramerLL::addElem(string name, bool isFinally){
    this->name_of_keys[name] = elems.size();
    ElemLL t(name, elems.size(), isFinally);
    elems.push_back(t);
    return elems.size() - 1;
}

//消除左递归
void GramerLL::clearLeftRE(){

    //1.消除直接左递归
    for (int i = 0; i < elems.size(); i++){

        //这个元素不是终结符号，需要进行修改
        if (!elems[i].isFinally){
            this->clearLeft(i);
        }

    }


    //2.消除间接左递归

    //根据生成规则，第0号下标就是开始符号
    for (int i = 1; i < elems.size(); i++){

        //如果elem[i]是终结符不需要
        if (elems[i].isFinally) continue;
        //先需要改写表达式
        for (int j = 0; j < i; j++){
            //只有非终结符才要改变
            if (!elems[j].isFinally){
                replaceEx(i, j);
            }
        }
        //消除直接左递归
        clearLeft(i);

    }
}

//消除直接左递归
//先检查elems[i]有没有直接左递归，如果就需要将他消除,最后会在elems后面添加一些新的东西
void GramerLL::clearLeft(int elemIndx){

    vector<int> expressionIndex;//有左递归的式子下标
    vector<int> notLeft;		//没有左递归的式子下标
    expressionIndex.clear();
    notLeft.clear();

    //有直接左递归压进去
    for (int i = 0; i < elems[elemIndx].expression_of_set.size(); i++){
        if (elems[elemIndx].expression_of_set[i][0] == elemIndx){
            expressionIndex.push_back(i);
        }
        else
        {
            notLeft.push_back(i);
        }
    }

    //消除直接直接左递归,size不等于0说明存在直接左递归，进行消除
    if (expressionIndex.size() != 0){
        //先分配一个elems的位置出来
        stringstream ss;
        ss << elems.size();
        addElem(elems[elemIndx].name + ss.str(), false);

        int newIndex = findName(elems[elemIndx].name + ss.str());

        vector<ExpressLL> temp = elems[elemIndx].expression_of_set;

        //更改原来的文法
        //不会出现A->Aa|Ab这种文法，这个有回路了，结论就是expressionIndex.size<elems[elemIndx].expression_of_set[i].size;
        //也不会出现这样的表达式A->A
        elems[elemIndx].expression_of_set.clear();//原来表达式清空

        for (int i = 0; i < notLeft.size(); i++){

            //如果是空串,需要将空串除去
            if (temp[notLeft[i]].expression[0] == -1 && temp[notLeft[i]].expression.size() == 1){
                temp.clear();
            }
            temp[notLeft[i]].expression.push_back(newIndex);//贝塔A1
            elems[elemIndx].addExpression(temp[notLeft[i]].expression);
        }

        //给新的非终结符添加的文法
        //包含空符号
        int  t[1];
        t[0] = -1;
        elems[newIndex].addExpression(ExpressLL(t, 1));

        for (int i = 0; i < expressionIndex.size(); i++){
            vector<int> a;
            for (int j = 1; j < temp[expressionIndex[i]].expression.size(); j++){
                a.push_back(temp[expressionIndex[i]].expression[j]);
            }
            a.push_back(newIndex);//阿尔法A1
            elems[newIndex].addExpression(a);//默认构造函数出现了
        }

    }

}


//替换,消除直接左递归的第一步替换，elems[oldElemIndex]的表达式集合会改变
void GramerLL::replaceEx(int oldElemIndex, int replaceIndex){


    //这一段代码和提取直接左递归类似，可以合并的
    vector<int> expressionIndex;//有replaceIndex的式子下标
    vector<int> notLeft;		//没有replaceIndex的式子下标
    expressionIndex.clear();
    notLeft.clear();

    //有replaceIndex压进去
    for (int i = 0; i < elems[oldElemIndex].expression_of_set.size(); i++){
        if (elems[oldElemIndex].expression_of_set[i][0] == replaceIndex){
            expressionIndex.push_back(i);
        }
        else
        {
            notLeft.push_back(i);
        }
    }


    //替换开始
    //比起直接左递归替换，这个替换更加暴力
    if (expressionIndex.size() != 0){

        //后来需要查的
        vector<ExpressLL> temp = elems[oldElemIndex].expression_of_set;
        elems[oldElemIndex].expression_of_set.clear();

        //先将不用改变的安置好
        for (int i = 0; i < notLeft.size(); i++){

            elems[oldElemIndex].addExpression(temp[notLeft[i]]);

        }

        //需要改变的
        for (int i = 0; i < expressionIndex.size(); i++){
            vector<int> diff;//后缀
            diff.clear();
            //计算后缀
            for (int j = 1; j < temp[expressionIndex[i]].expression.size(); j++){
                diff.push_back(temp[expressionIndex[i]][j]);
            }

            //替换的表达式遍历，第j+1个表达式
            for (int j = 0; j < elems[replaceIndex].expression_of_set.size(); j++){
                vector<int> exTemp;
                exTemp.clear();
                //这个表达式为空
                if (elems[replaceIndex].expression_of_set[j][0] == -1 &&
                    elems[replaceIndex].expression_of_set[j].expression.size() == 1){
                    //直接压入后缀就行
                    for (int index = 0; index < diff.size(); index++){
                        exTemp.push_back(diff[index]);
                    }
                    elems[oldElemIndex].addExpression(exTemp);
                    continue;
                }
                //将表达式压入，上一步已经将空串去除,替换操作
                for (int index = 0; index < elems[replaceIndex].expression_of_set[j].expression.size(); index++){
                    exTemp.push_back(elems[replaceIndex].expression_of_set[j].expression[index]);
                }
                //后缀保留
                for (int index = 0; index < diff.size(); index++){
                    exTemp.push_back(diff[index]);
                }
                elems[oldElemIndex].addExpression(exTemp);
            }

        }
    }

}

//消除左因子
void GramerLL::clearLeftFactor(){

    //消除左因子,使用的东西,在循环外面定义提高效率
    vector<int> factory(20);
    int begin, count;

    for (int i = 0; i < elems.size(); i++){

        //是终结符则一定不需要消除左因子
        if (elems[i].isFinally) continue;



        while (elems[i].extractLeftFactor(factory, begin, count))//这里面会将factory clear的
        {	//i有左因子
            updateElem(i, begin, begin + count, factory);
        }

    }
}

//修改元素
void GramerLL::updateElem(int elemIndex, int begin, int end, vector<int>& factory){


    stringstream ss;
    ss << elems.size();
    int newIndex = addElem(elems[elemIndex].name + ss.str(), false);

    //先对新的式子添加表达式,将begin到end添加的
    //产生空串的只有第一个begin有这个倾向，别的没有这个倾向,将begin单独拿出来作为特例
    vector<int> t;
    t.clear();
    if (elems[elemIndex].expression_of_set[begin].expression.size() == factory.size()){
        t.push_back(-1);
    }
    else
    {
        for (int i = factory.size(); i < elems[elemIndex].expression_of_set[begin].expression.size(); i++){
            t.push_back(elems[elemIndex].expression_of_set[begin][i]);
        }
    }
    elems[newIndex].addExpression(t);

    for (int i = begin + 1; i < end; i++){
        t.clear();
        for (int j = factory.size(); j < elems[elemIndex].expression_of_set[i].expression.size(); j++){
            t.push_back(elems[elemIndex].expression_of_set[i][j]);
        }
        elems[newIndex].addExpression(t);
    }


    //修改老元素了
    //1. 在begin下面放下结果
    elems[elemIndex].expression_of_set[begin].expression.clear();
    for (int i = 0; i < factory.size(); i++){
        elems[elemIndex].expression_of_set[begin].expression.push_back(factory[i]);
    }
    elems[elemIndex].expression_of_set[begin].expression.push_back(newIndex);

    //后面的元素提前
    int moveDistance = end - begin;
    for (int i = end; i < elems[elemIndex].expression_of_set.size(); i++){
        elems[elemIndex].expression_of_set[i - moveDistance + 1] = elems[elemIndex].expression_of_set[i];
    }

    //清楚元素，共清楚moveDistance-1个
    for (int i = 0; i < moveDistance - 1; i++){
        elems[elemIndex].expression_of_set.pop_back();
    }
}


//根据findtable和Elems来产生终结符下标
void GramerLL::get_tow_charIndex(){
    vector<int> t;
    t.clear();

    for (int i = 0; i < elems.size(); i++){
        if (elems[i].isFinally){
            finally_charIndex.push_back(i);
        }
        else
        {
            not_finally_charIndex.push_back(i);
        }
        First.push_back(t);
        Follow.push_back(t);

    }

}



//获得First集合
void GramerLL::createFirstSet(){

    for (int i = 0; i < elems.size(); i++){
        getFirst(i);
    }
}

//计算elem[elemIndex]的first集合,并且将结果放在First[elemIndex中]
//返回值，如果是true，说明这个不是空串（elemIndex！=-1）;
//		  如果是false，说明这个是空串，elemIndex=-1,级这个First集合为（-1）空
bool GramerLL::getFirst(int elemIndex){

    //空串
    if (elemIndex == -1) return false;

    //这个符号的first集合已经求出来了
    if (First[elemIndex].size() != 0) return true;

    //判断是不是终结符，是的话说明这个first可以直接求
    if (elems[elemIndex].isFinally){
        //是终结符,将自己添加就行
        First[elemIndex].push_back(elemIndex);
    }
    else
    {
        //不是终结符，需要计算这个非终结符的每一个表达式然后在添加上去
        //vector<int> t;
        vector<int> temp;
        this->firstTemp.clear();//后续准备
        //计算这个产生式的表达式
        for (int i = 0; i < elems[elemIndex].expression_of_set.size(); i++){
            //计算第i个表达式
            caluteExpressionFirst(elemIndex, i, temp);
            connetVector(First[elemIndex], temp);
        }
        //对于这种情况有没有可能出现：
        //A->TAb|c|T
        //T->@|t
        //进行递归了
        for (int i = 0; i < firstTemp.size(); i++){
            caluteExpressionFirst(elemIndex, firstTemp[i], temp,false);
            connetVector(First[elemIndex], temp);
        }

    }

    return true;
}



//计算elems[elemIndex]的expressIndex下标的表达式的first集合，结果放在result里面
void  GramerLL::caluteExpressionFirst(int elemIndex, int expressionIndex, vector<int>& result, bool choose){

    result.clear();
    //vector<int>::iterator it =elems[elemIndex].expression_of_set[expressionIndex].expression.begin() ;
    //vector<int>::iterator end = elems[elemIndex].expression_of_set[expressionIndex].expression.end();




    for (int i = 0; i < elems[elemIndex].expression_of_set[expressionIndex].expression.size(); i++){

        //对于这种情况有没有可能出现：
        //A->TAb|c|T
        //T->@|t
        //进行递归了
        if (elems[elemIndex].expression_of_set[expressionIndex][i] == elemIndex) {

            //后续处理
            if (isInA(First[elemIndex], -1)) continue;
            else{
                if(choose) this->firstTemp.push_back(expressionIndex);
                break;
            }

        }

        if (getFirst(elems[elemIndex].expression_of_set[expressionIndex][i])){

            if (i == elems[elemIndex].expression_of_set[expressionIndex].expression.size() - 1){
                connetVector(result, First[elems[elemIndex].expression_of_set[expressionIndex][i]]);
            }
            else
            {
                vector<int> notNULL;//将空串除去
                int length = First[elems[elemIndex].expression_of_set[expressionIndex][i]].size();
                vector<int> tmpF = First[elems[elemIndex].expression_of_set[expressionIndex][i]];
                for (int j = 0; j < length; j++){
                    if (tmpF[j] != -1){
                        notNULL.push_back(tmpF[j]);
                    }
                }
                connetVector(result, notNULL);
            }

        }
        else
        {
            if (!isInA(result, -1)) result.push_back(-1);
            break;
        }

        //-1不在这个式子里面后面不用继续了
        if (!isInA(First[elems[elemIndex].expression_of_set[expressionIndex][i]], -1)) break;

    }
}


//生成Follow集合
void GramerLL::createFollowSet(){

    vector<int> temp;
    //每个非终结符
    for (int i = 0; i < not_finally_charIndex.size(); i++){
        //计算Follw需要的是各个表达式子,遍历第i个非终结符的各个表达式
        for (int j = 0; j < elems[not_finally_charIndex[i]].expression_of_set.size(); j++){
            //第j个表达式
            ExpressLL& expreesion = elems[not_finally_charIndex[i]].expression_of_set[j];
            for (int m = 0; m < expreesion.expression.size(); m++){


                //如果是空串的话
                if (expreesion[m] == -1){
                    continue;
                }

                //是终结符不用计算
                if (elems[expreesion[m]].isFinally) continue;

                //获得First集合
                this->getFollowByFirst(i, expreesion, m+1, temp);
                connetVector(Follow[expreesion[m]], temp);

            }

        }
    }

    //考虑follow集合的添加
    for (int i = 0; i < not_finally_charIndex.size(); i++){
        adjustFollow(i);
    }

}



void GramerLL::getFollowByFirst(int elemIndex,ExpressLL express, int begin, vector<int>& result){
    result.clear();
    vector<int> tmp;

    //如果是一个空串的话
    if (express[0] == -1){
        result.push_back(elemIndex);
        return;
    }

    for (begin=begin; begin < express.expression.size(); begin++){

        //如果空不在退出
        if (!isInA(First[express[begin]], -1)){
            connetVector(result, First[express[begin]]);
            break;
        }
        else
        {
            clearNull(First[express[begin]], tmp);
            connetVector(result, tmp);
        }

    }
    if (begin == express.expression.size()){
        //result.push_back(-1);
        result.push_back(elemIndex);
    }

}

void GramerLL::adjustFollow(int index){
    //保存使用
    vector<int> tempFollow = Follow[not_finally_charIndex[index]];
    for (int i = 0; i < tempFollow.size(); i++){
        if (!elems[tempFollow[i]].isFinally){
            //在原来的Follow中将这个非终结符删除
            vector<int>::const_iterator deleteIt = find(Follow[not_finally_charIndex[index]].begin(), Follow[not_finally_charIndex[index]].end(), tempFollow[i]);
            Follow[not_finally_charIndex[index]].erase(deleteIt);

            //如果两个是一样的
            if (not_finally_charIndex[index] != tempFollow[i]) {
                adjustFollow(tempFollow[i]);
                connetVector(Follow[not_finally_charIndex[index]], Follow[tempFollow[i]]);

            }




        }

    }

}

void llGramer::clearNull(const vector<int>& a, vector<int>& result, int value ){
    result.clear();
    for (int i = 0; i < a.size(); i++){
        if (a[i] != value){
            result.push_back(a[i]);
        }
    }

}

//预测分析表
void GramerLL::get_predictTable(){
    //1.初始化预测分析表
    vector<int> tmp;
    for (int i = 0; i < finally_charIndex.size(); i++){
        tmp.push_back(-1);//-1为错误
    }
    for (int i = 0; i < not_finally_charIndex.size(); i++){
        predictTable.push_back(tmp);
    }

    //2.开始计算了，以每一个非终结符为首开始
    for (int i = 0; i < not_finally_charIndex.size(); i++){

        //遍历这个非终结符的表达式
        for (int j = 0; j < elems[not_finally_charIndex[i]].expression_of_set.size(); j++){

            this->getFollowByFirst(not_finally_charIndex[i], elems[not_finally_charIndex[i]].expression_of_set[j], 0, tmp);
            //如果tmp里面有非终结符，则说明有空，如果没有则说明没有空
            for (int m = 0; m < tmp.size(); m++){
                //如果是终结符直接添加
                if (elems[tmp[m]].isFinally){
                    vector<int>::const_iterator it=find(this->finally_charIndex.begin(), this->finally_charIndex.end(), tmp[m]);

                    //调试使用检测有没出现同一个坑占两个情况
                    cout << "replace[" << i << "," << it - this->finally_charIndex.begin() << "]:" << this->predictTable[i][it - this->finally_charIndex.begin()] << "->" << j<<endl;

                    this->predictTable[i][it - this->finally_charIndex.begin()] = j;
                }
                    //如果是非终结符要添加Follow集合
                else
                {
                    for (int k = 0; k<Follow[tmp[m]].size(); k++){
                        vector<int>::const_iterator it = find(this->finally_charIndex.begin(), this->finally_charIndex.end(), Follow[tmp[m]][k]);

                        //调试使用检测有没出现同一个坑占两个情况
                        cout << "replace[" << i << "," << it - this->finally_charIndex.begin() << "]:" << this->predictTable[i][it - this->finally_charIndex.begin()] << "->" << j<<endl;

                        this->predictTable[i][it - this->finally_charIndex.begin()] = j;
                    }
                }

            }
        }
    }

}


//根据预测分析表进行判断
bool GramerLL::sim(char* testString){

    //不使用testString，出现双字符串无法解释;
    vector<string> ipString;
    parseElem(testString, ipString);


    //存放字符的下标
    stack<int> s;
    vector<int> sprint;//输出栈
    char formatS[] = "%-25s";//栈的格式
    char formatC[] = "%+25s";//字母格式的格式
    string strtmp;		//计算各种格式
    char buffer[100];	//上面的按照格式转化后的结果

    //准备工作
    int ip = 0;//指向测试的数据
    int expressionIndex=0;
    vector<int>::const_iterator xIt;
    vector<int>::const_iterator yIt;

    //初始化话stack;
    s.push(findName("#"));
    sprint.push_back(findName("#"));

    s.push(0);
    sprint.push_back(0);

    string text="";
    while (s.top() != findName("#"))
    {
        strtmp = "";
        //栈写出来
        for (int i = 0; i < sprint.size(); i++){
            strtmp = strtmp + elems[sprint[i]].name + " ";
            //text = text + elems[sprint[i]].name+" ";
        }
        sprintf(buffer, formatS, strtmp.c_str());
        text = text + buffer;

        strtmp = "";
        for (int i = ip; i < ipString.size(); i++){
            strtmp = strtmp + ipString[i];
            //text = text + testString[i];
        }

        sprintf(buffer, formatC, strtmp.c_str());
        text = text + buffer+"\t";




        //开始动作了,字符串上一定是终结符

        //string a = "";
        //a = a + testString[ip];
        int testI = findName(ipString[ip]);
        yIt = find(finally_charIndex.begin(), finally_charIndex.end(), findName(ipString[ip]));


        //非终结符一定能找到，但是非终结符不一定能找到
        if (yIt == finally_charIndex.end()){
            text =text+ "erro\n";
            break;
        }

        if (elems[s.top()].isFinally){
            //如果栈中是终结符
            if (*yIt == s.top()){

                sprint.pop_back();
                text = text + "pop(" + elems[*yIt].name+"), next("+to_string(ip)+")\n";
                s.pop();
                ip++;

            }
            else
            {
                text = text + "erro\n";
                break;
            }
        }
        else
        {
            xIt = find(not_finally_charIndex.begin(), not_finally_charIndex.end(), s.top());
            expressionIndex = this->predictTable[xIt - not_finally_charIndex.begin()][yIt - finally_charIndex.begin()];

            if (expressionIndex == -1){
                text = text + "erro\n";
                break;
            }

            //出栈;
            sprint.pop_back();
            text = text + "pop(" + elems[*xIt].name+"),push(";
            s.pop();

            //压栈;
            vector<int>& exp = elems[*xIt].expression_of_set[expressionIndex].expression;
            for (int i = exp.size()-1; i >=0; i--){
                if (exp[i] == -1){
                    continue;
                }
                text = text + elems[exp[i]].name+" ";
                s.push(exp[i]);
                sprint.push_back(exp[i]);
            }
            text = text + ")\n";

        }


    }
    cout << text;



    return true;
}








