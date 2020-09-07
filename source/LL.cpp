//
// Created by 周亮 on 2019/4/17.
//
#include"../head/LL.h"
#include<sstream>
#include<iostream>
#include<algorithm>
#include<stack>
using namespace std;
using namespace ll_grammar;
using namespace public_tool;




//LL的特殊化

//表达式
ExpressLeft::ExpressLeft(int* expressions, int length) : Express(expressions, length){}

ExpressLeft::ExpressLeft(const vector<int>& expression) : Express(expression){}


//LL符号
ElemLeft::ElemLeft(string name, int key, bool isFinally) : Elem(name, key, isFinally){}

bool ElemLeft::add_expression(ExpressLeft expression){

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
bool ElemLeft::extract_left_factor(vector<int>&result, int& begin, int& count){

    //排序一下，
    //插入排序
    this->sort_expression();
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
void ElemLeft::sort_expression(){
    //采用插入排序方式
    for (int i = 1; i < this->expression_of_set.size(); i++){
        int insertIndex;
        ExpressLeft temp = expression_of_set[i];
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
GrammarLeft::GrammarLeft(string* generate, int length){
    //1.解析所有字符串
    vector<vector<string>> vectorGenrates;//这个是用来存放各个表达式

    //解析各个表达式,第一个是非终结符号，后面的是产生式
    for (int i = 0; i < length; i++){
        vector<string> temp;
        parse_productions(generate[i], temp);
        vectorGenrates.push_back(temp);
    }

    //2.添加非终结符添加到find表中,开始的非终结符就是length个
    for (int i = 0; i < length; i++){
        add_element(vectorGenrates[i][0], false);
    }

    //3.一个个解析产生式,会添加到find表中
    vector<string> result_of_name;
    for (int i = 0; i < length; i++){
        //ElemLeft N_F_E = elements[find_name(vectorGenrates[i][0])];//非终结符，接下来要对这个终结符进行exp的添加
        //对genrates[i]进行分析；
        for (int j = 1; j < vectorGenrates[i].size(); j++){
            parse_elem(vectorGenrates[i][j], result_of_name);
            vector<int> exp;
            for (int m = 0; m < result_of_name.size(); m++){
                int index = find_name(result_of_name[m]);
                if (index == -2){
                    index = add_element(result_of_name[m], true);
                }
                exp.push_back(index);

            }
            elements[find_name(vectorGenrates[i][0])].add_expression(ExpressLeft(exp));
        }
    }

    //4.消除左递归
    this->clear_left_recursion();
    //5.消除左因子
    this->clear_left_factor();

    add_element("#", true);
    //查看输入是不正确
    //测试一下：
    string finnalyC = "finnaly:";
    for (int i = 0; i < elements.size(); i++){
        if (!elements[i].is_finally){
            cout << elements[i].name << "->";
            for (int j = 0; j < elements[i].expression_of_set.size(); j++){
                for (int m = 0; m < elements[i].expression_of_set[j].expression.size(); m++){
                    if (elements[i].expression_of_set[j].expression[m] != -1){
                        cout << elements[elements[i].expression_of_set[j].expression[m]].name;
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
            finnalyC += " " + elements[i].name;
        }
    }
    cout << finnalyC << endl;

    //6.两表生成（在5的时候就不会再添加新的符号了）
    //同时在这里初始化一下First和Follw表
    get_tow_char_index();

    //7.求first集合
    this->create_first_set();

    //检查一下，first集合
    for (int i = 0; i < elements.size(); i++){
        cout << "first[" << elements[i].name << "]={";
        for (int j = 0; j < first[i].size(); j++){
            if (first[i][j] == -1) cout << "@ ";
            else
            {
                cout << elements[first[i][j]].name << " ";
            }
        }
        cout << "}\n";
    }

    //8.求follow集合
    //将#送到开始符号
    this->follow[0].push_back(find_name("#"));
    this->create_follow_set();


    for (int i = 0; i < elements.size(); i++){

        if (elements[i].is_finally) continue;

        cout << "follow[" << elements[i].name << "]={";
        for (int j = 0; j < follow[i].size(); j++){
            if (follow[i][j] == -1) cout << "@ ";
            else
            {
                cout << elements[follow[i][j]].name << " ";
            }
        }
        cout << "}\n";
    }
    //9.构造预测分析表
    this->get_predict_table();


    for (int i = 0; i < predict_table.size(); i++){
        for (int j = 0; j < predict_table[i].size(); j++){
            cout << predict_table[i][j] << " ";
        }
        cout << endl;
    }

}




//find查找，找到的话返回下标，
//找不到则返回-2
//-1表示为空，空使用@来表示
int GrammarLeft::find_name(string name_of_elem){
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
int GrammarLeft::add_element(string name, bool isFinally){
    this->name_of_keys[name] = elements.size();
    ElemLeft t(name, elements.size(), isFinally);
    elements.push_back(t);
    return elements.size() - 1;
}

//消除左递归
void GrammarLeft::clear_left_recursion(){

    //1.消除直接左递归
    for (int i = 0; i < elements.size(); i++){

        //这个元素不是终结符号，需要进行修改
        if (!elements[i].is_finally){
            this->clear_left(i);
        }
    }

    //2.消除间接左递归
    //根据生成规则，第0号下标就是开始符号
    for (int i = 1; i < elements.size(); i++){
        //如果elem[i]是终结符不需要
        if (elements[i].is_finally) continue;
        //先需要改写表达式
        for (int j = 0; j < i; j++){
            //只有非终结符才要改变
            if (!elements[j].is_finally){
                replace_express(i, j);
            }
        }
        //消除直接左递归
        clear_left(i);
    }
}

//消除直接左递归
//先检查elems[i]有没有直接左递归，如果就需要将他消除,最后会在elems后面添加一些新的东西
void GrammarLeft::clear_left(int elemIndx){

    vector<int> expressionIndex;//有左递归的式子下标
    vector<int> notLeft;		//没有左递归的式子下标
    expressionIndex.clear();
    notLeft.clear();

    //有直接左递归压进去
    for (int i = 0; i < elements[elemIndx].expression_of_set.size(); i++){
        if (elements[elemIndx].expression_of_set[i][0] == elemIndx){
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
        ss << elements.size();
        add_element(elements[elemIndx].name + ss.str(), false);

        int newIndex = find_name(elements[elemIndx].name + ss.str());

        vector<ExpressLeft> temp = elements[elemIndx].expression_of_set;

        //更改原来的文法
        //不会出现A->Aa|Ab这种文法，这个有回路了，结论就是expressionIndex.size<elements[elemIndx].expression_of_set[i].size;
        //也不会出现这样的表达式A->A
        elements[elemIndx].expression_of_set.clear();//原来表达式清空

        for (int i = 0; i < notLeft.size(); i++){

            //如果是空串,需要将空串除去
            if (temp[notLeft[i]].expression[0] == -1 && temp[notLeft[i]].expression.size() == 1){
                temp.clear();
            }
            temp[notLeft[i]].expression.push_back(newIndex);//贝塔A1
            elements[elemIndx].add_expression(temp[notLeft[i]].expression);
        }

        //给新的非终结符添加的文法
        //包含空符号
        int  t[1];
        t[0] = -1;
        elements[newIndex].add_expression(ExpressLeft(t, 1));

        for (int i = 0; i < expressionIndex.size(); i++){
            vector<int> a;
            for (int j = 1; j < temp[expressionIndex[i]].expression.size(); j++){
                a.push_back(temp[expressionIndex[i]].expression[j]);
            }
            a.push_back(newIndex);//阿尔法A1
            elements[newIndex].add_expression(a);//默认构造函数出现了
        }

    }

}


//替换,消除直接左递归的第一步替换，elements[oldElemIndex]的表达式集合会改变
void GrammarLeft::replace_express(int oldElemIndex, int replaceIndex){


    //这一段代码和提取直接左递归类似，可以合并的
    vector<int> expressionIndex;//有replaceIndex的式子下标
    vector<int> notLeft;		//没有replaceIndex的式子下标
    expressionIndex.clear();
    notLeft.clear();

    //有replaceIndex压进去
    for (int i = 0; i < elements[oldElemIndex].expression_of_set.size(); i++){
        if (elements[oldElemIndex].expression_of_set[i][0] == replaceIndex){
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
        vector<ExpressLeft> temp = elements[oldElemIndex].expression_of_set;
        elements[oldElemIndex].expression_of_set.clear();

        //先将不用改变的安置好
        for (int i = 0; i < notLeft.size(); i++){

            elements[oldElemIndex].add_expression(temp[notLeft[i]]);

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
            for (int j = 0; j < elements[replaceIndex].expression_of_set.size(); j++){
                vector<int> exTemp;
                exTemp.clear();
                //这个表达式为空
                if (elements[replaceIndex].expression_of_set[j][0] == -1 &&
                    elements[replaceIndex].expression_of_set[j].expression.size() == 1){
                    //直接压入后缀就行
                    for (int index = 0; index < diff.size(); index++){
                        exTemp.push_back(diff[index]);
                    }
                    elements[oldElemIndex].add_expression(exTemp);
                    continue;
                }
                //将表达式压入，上一步已经将空串去除,替换操作
                for (int index = 0; index < elements[replaceIndex].expression_of_set[j].expression.size(); index++){
                    exTemp.push_back(elements[replaceIndex].expression_of_set[j].expression[index]);
                }
                //后缀保留
                for (int index = 0; index < diff.size(); index++){
                    exTemp.push_back(diff[index]);
                }
                elements[oldElemIndex].add_expression(exTemp);
            }

        }
    }

}

//消除左因子
void GrammarLeft::clear_left_factor(){

    //消除左因子,使用的东西,在循环外面定义提高效率
    vector<int> factory(20);
    int begin, count;

    for (int i = 0; i < elements.size(); i++){

        //是终结符则一定不需要消除左因子
        if (elements[i].is_finally) continue;



        while (elements[i].extract_left_factor(factory, begin, count))//这里面会将factory clear的
        {	//i有左因子
            update_elem(i, begin, begin + count, factory);
        }

    }
}

//修改元素
void GrammarLeft::update_elem(int elemIndex, int begin, int end, vector<int>& factory){


    stringstream ss;
    ss << elements.size();
    int newIndex = add_element(elements[elemIndex].name + ss.str(), false);

    //先对新的式子添加表达式,将begin到end添加的
    //产生空串的只有第一个begin有这个倾向，别的没有这个倾向,将begin单独拿出来作为特例
    vector<int> t;
    t.clear();
    if (elements[elemIndex].expression_of_set[begin].expression.size() == factory.size()){
        t.push_back(-1);
    }
    else
    {
        for (int i = factory.size(); i < elements[elemIndex].expression_of_set[begin].expression.size(); i++){
            t.push_back(elements[elemIndex].expression_of_set[begin][i]);
        }
    }
    elements[newIndex].add_expression(t);

    for (int i = begin + 1; i < end; i++){
        t.clear();
        for (int j = factory.size(); j < elements[elemIndex].expression_of_set[i].expression.size(); j++){
            t.push_back(elements[elemIndex].expression_of_set[i][j]);
        }
        elements[newIndex].add_expression(t);
    }


    //修改老元素了
    //1. 在begin下面放下结果
    elements[elemIndex].expression_of_set[begin].expression.clear();
    for (int i = 0; i < factory.size(); i++){
        elements[elemIndex].expression_of_set[begin].expression.push_back(factory[i]);
    }
    elements[elemIndex].expression_of_set[begin].expression.push_back(newIndex);

    //后面的元素提前
    int moveDistance = end - begin;
    for (int i = end; i < elements[elemIndex].expression_of_set.size(); i++){
        elements[elemIndex].expression_of_set[i - moveDistance + 1] = elements[elemIndex].expression_of_set[i];
    }

    //清楚元素，共清楚moveDistance-1个
    for (int i = 0; i < moveDistance - 1; i++){
        elements[elemIndex].expression_of_set.pop_back();
    }
}


//根据findtable和Elems来产生终结符下标
void GrammarLeft::get_tow_char_index(){
    vector<int> t;
    t.clear();

    for (int i = 0; i < elements.size(); i++){
        if (elements[i].is_finally){
            finally_char_index.push_back(i);
        }
        else
        {
            not_finally_char_index.push_back(i);
        }
        first.push_back(t);
        follow.push_back(t);

    }

}



//获得First集合
void GrammarLeft::create_first_set(){

    for (int i = 0; i < elements.size(); i++){
        get_first(i);
    }
}

//计算elem[elemIndex]的first集合,并且将结果放在First[elemIndex中]
//返回值，如果是true，说明这个不是空串（elemIndex！=-1）;
//		  如果是false，说明这个是空串，elemIndex=-1,级这个First集合为（-1）空
bool GrammarLeft::get_first(int elemIndex){

    //空串
    if (elemIndex == -1) return false;

    //这个符号的first集合已经求出来了
    if (first[elemIndex].size() != 0) return true;

    //判断是不是终结符，是的话说明这个first可以直接求
    if (elements[elemIndex].is_finally){
        //是终结符,将自己添加就行
        first[elemIndex].push_back(elemIndex);
    }
    else
    {
        //不是终结符，需要计算这个非终结符的每一个表达式然后在添加上去
        //vector<int> t;
        vector<int> temp;
        this->first_temp.clear();//后续准备
        //计算这个产生式的表达式
        for (int i = 0; i < elements[elemIndex].expression_of_set.size(); i++){
            //计算第i个表达式
            calculate_expression_first(elemIndex, i, temp);
            connect_vector(first[elemIndex], temp);
        }
        //对于这种情况有没有可能出现：
        //A->TAb|c|T
        //T->@|t
        //进行递归了
        for (int i = 0; i < first_temp.size(); i++){
            calculate_expression_first(elemIndex, first_temp[i], temp, false);
            connect_vector(first[elemIndex], temp);
        }

    }

    return true;
}



//计算elems[elemIndex]的expressIndex下标的表达式的first集合，结果放在result里面
void  GrammarLeft::calculate_expression_first(int elemIndex, int expressionIndex, vector<int>& result, bool choose){

    result.clear();
    //vector<int>::iterator it =elements[elemIndex].expression_of_set[expressionIndex].expression.begin() ;
    //vector<int>::iterator end = elements[elemIndex].expression_of_set[expressionIndex].expression.end();




    for (int i = 0; i < elements[elemIndex].expression_of_set[expressionIndex].expression.size(); i++){

        //对于这种情况有没有可能出现：
        //A->TAb|c|T
        //T->@|t
        //进行递归了
        if (elements[elemIndex].expression_of_set[expressionIndex][i] == elemIndex) {

            //后续处理
            if (is_in_vector(first[elemIndex], -1)) continue;
            else{
                if(choose) this->first_temp.push_back(expressionIndex);
                break;
            }

        }

        if (get_first(elements[elemIndex].expression_of_set[expressionIndex][i])){

            if (i == elements[elemIndex].expression_of_set[expressionIndex].expression.size() - 1){
                connect_vector(result, first[elements[elemIndex].expression_of_set[expressionIndex][i]]);
            }
            else
            {
                vector<int> notNULL;//将空串除去
                int length = first[elements[elemIndex].expression_of_set[expressionIndex][i]].size();
                vector<int> tmpF = first[elements[elemIndex].expression_of_set[expressionIndex][i]];
                for (int j = 0; j < length; j++){
                    if (tmpF[j] != -1){
                        notNULL.push_back(tmpF[j]);
                    }
                }
                connect_vector(result, notNULL);
            }

        }
        else
        {
            if (!is_in_vector(result, -1)) result.push_back(-1);
            break;
        }

        //-1不在这个式子里面后面不用继续了
        if (!is_in_vector(first[elements[elemIndex].expression_of_set[expressionIndex][i]], -1)) break;

    }
}


//生成Follow集合
void GrammarLeft::create_follow_set(){

    vector<int> temp;
    //每个非终结符
    for (int i = 0; i < not_finally_char_index.size(); i++){
        //计算Follw需要的是各个表达式子,遍历第i个非终结符的各个表达式
        for (int j = 0; j < elements[not_finally_char_index[i]].expression_of_set.size(); j++){
            //第j个表达式
            ExpressLeft& expreesion = elements[not_finally_char_index[i]].expression_of_set[j];
            for (int m = 0; m < expreesion.expression.size(); m++){


                //如果是空串的话
                if (expreesion[m] == -1){
                    continue;
                }

                //是终结符不用计算
                if (elements[expreesion[m]].is_finally) continue;

                //获得First集合
                this->get_follow_by_first(i, expreesion, m + 1, temp);
                connect_vector(follow[expreesion[m]], temp);

            }

        }
    }

    //考虑follow集合的添加
    for (int i = 0; i < not_finally_char_index.size(); i++){
        adjust_follow(i);
    }

}



void GrammarLeft::get_follow_by_first(int elemIndex, ExpressLeft express, int begin, vector<int>& result){
    result.clear();
    vector<int> tmp;

    //如果是一个空串的话
    if (express[0] == -1){
        result.push_back(elemIndex);
        return;
    }

    for (begin=begin; begin < express.expression.size(); begin++){

        //如果空不在退出
        if (!is_in_vector(first[express[begin]], -1)){
            connect_vector(result, first[express[begin]]);
            break;
        }
        else
        {
            clear_null(first[express[begin]], tmp);
            connect_vector(result, tmp);
        }

    }
    if (begin == express.expression.size()){
        //result.push_back(-1);
        result.push_back(elemIndex);
    }

}

void GrammarLeft::adjust_follow(int index){
    //保存使用
    vector<int> tempFollow = follow[not_finally_char_index[index]];
    for (int i = 0; i < tempFollow.size(); i++){
        if (!elements[tempFollow[i]].is_finally){
            //在原来的Follow中将这个非终结符删除
            vector<int>::const_iterator deleteIt = find(follow[not_finally_char_index[index]].begin(), follow[not_finally_char_index[index]].end(), tempFollow[i]);
            follow[not_finally_char_index[index]].erase(deleteIt);

            //如果两个是一样的
            if (not_finally_char_index[index] != tempFollow[i]) {
                adjust_follow(tempFollow[i]);
                connect_vector(follow[not_finally_char_index[index]], follow[tempFollow[i]]);

            }




        }

    }

}

void ll_grammar::clear_null(const vector<int>& a, vector<int>& result, int value ){
    result.clear();
    for (int i = 0; i < a.size(); i++){
        if (a[i] != value){
            result.push_back(a[i]);
        }
    }

}

//预测分析表
void GrammarLeft::get_predict_table(){
    //1.初始化预测分析表
    vector<int> tmp;
    for (int i = 0; i < finally_char_index.size(); i++){
        tmp.push_back(-1);//-1为错误
    }
    for (int i = 0; i < not_finally_char_index.size(); i++){
        predict_table.push_back(tmp);
    }

    //2.开始计算了，以每一个非终结符为首开始
    for (int i = 0; i < not_finally_char_index.size(); i++){

        //遍历这个非终结符的表达式
        for (int j = 0; j < elements[not_finally_char_index[i]].expression_of_set.size(); j++){

            this->get_follow_by_first(not_finally_char_index[i],
                                      elements[not_finally_char_index[i]].expression_of_set[j], 0, tmp);
            //如果tmp里面有非终结符，则说明有空，如果没有则说明没有空
            for (int m = 0; m < tmp.size(); m++){
                //如果是终结符直接添加
                if (elements[tmp[m]].is_finally){
                    vector<int>::const_iterator it=find(this->finally_char_index.begin(), this->finally_char_index.end(), tmp[m]);

                    //调试使用检测有没出现同一个坑占两个情况
                    cout << "replace[" << i << "," << it - this->finally_char_index.begin() << "]:" << this->predict_table[i][it - this->finally_char_index.begin()] << "->" << j << endl;

                    this->predict_table[i][it - this->finally_char_index.begin()] = j;
                }
                    //如果是非终结符要添加Follow集合
                else
                {
                    for (int k = 0; k < follow[tmp[m]].size(); k++){
                        vector<int>::const_iterator it = find(this->finally_char_index.begin(), this->finally_char_index.end(), follow[tmp[m]][k]);

                        //调试使用检测有没出现同一个坑占两个情况
                        cout << "replace[" << i << "," << it - this->finally_char_index.begin() << "]:" << this->predict_table[i][it - this->finally_char_index.begin()] << "->" << j << endl;

                        this->predict_table[i][it - this->finally_char_index.begin()] = j;
                    }
                }

            }
        }
    }

}


//根据预测分析表进行判断
bool GrammarLeft::sim(char* testString){

    //不使用testString，出现双字符串无法解释;
    vector<string> ipString;
    parse_elem(testString, ipString);


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
    s.push(find_name("#"));
    sprint.push_back(find_name("#"));

    s.push(0);
    sprint.push_back(0);

    string text="";
    while (s.top() != find_name("#"))
    {
        strtmp = "";
        //栈写出来
        for (int i = 0; i < sprint.size(); i++){
            strtmp = strtmp + elements[sprint[i]].name + " ";
            //text = text + elements[sprint[i]].name+" ";
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
        int testI = find_name(ipString[ip]);
        yIt = find(finally_char_index.begin(), finally_char_index.end(), find_name(ipString[ip]));


        //非终结符一定能找到，但是非终结符不一定能找到
        if (yIt == finally_char_index.end()){
            text =text+ "erro\n";
            break;
        }

        if (elements[s.top()].is_finally){
            //如果栈中是终结符
            if (*yIt == s.top()){

                sprint.pop_back();
                text = text + "pop(" + elements[*yIt].name + "), next(" + to_string(ip) + ")\n";
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
            xIt = find(not_finally_char_index.begin(), not_finally_char_index.end(), s.top());
            expressionIndex = this->predict_table[xIt - not_finally_char_index.begin()][yIt - finally_char_index.begin()];

            if (expressionIndex == -1){
                text = text + "erro\n";
                break;
            }

            //出栈;
            sprint.pop_back();
            text = text + "pop(" + elements[*xIt].name + "),push(";
            s.pop();

            //压栈;
            vector<int>& exp = elements[*xIt].expression_of_set[expressionIndex].expression;
            for (int i = exp.size()-1; i >=0; i--){
                if (exp[i] == -1){
                    continue;
                }
                text = text + elements[exp[i]].name + " ";
                s.push(exp[i]);
                sprint.push_back(exp[i]);
            }
            text = text + ")\n";

        }


    }
    cout << text;

    return true;
}








