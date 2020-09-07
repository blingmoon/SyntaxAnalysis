//
// Created by 周亮 on 2019/4/17.
//

#include"../head/LR.h"
#include<sstream>
#include<iostream>
#include<algorithm>
#include<stack>
using namespace std;
using namespace lr_grammar;
using namespace public_tool;

//bool is_in_vector(const vector<int>& a, int b);
//void connect_vector(vector<int>& a, const vector<int>& b);


//project的实现
Project::Project(int elemKey, int expressIndex, int peojectPoint){
    this->elemKey = elemKey;
    this->expressIndex = expressIndex;
    this->projectPoint = peojectPoint;
}
bool Project::operator< (const Project& project)const{

    //this<project为true


    if (elemKey > project.elemKey) return false;
    if (elemKey < project.elemKey) return true;

    //这里说明elemKey相同
    if (expressIndex > project.expressIndex) return false;
    if (expressIndex < project.expressIndex) return true;


    //这里说明expressIndex相同
    if (projectPoint > project.projectPoint) return false;
    if (projectPoint < project.projectPoint) return true;

    //这里就说明两个相等了
    return false;

}

bool Project::operator==(const Project& project)const {

    bool result=false;

    if (elemKey == project.elemKey&&expressIndex == project.expressIndex&&projectPoint == project.projectPoint){
        result = true;
    }
    return result;
}
bool Project::operator!=(const Project& project)const {
    return !(*this==project);
}





//LR的特殊化
//表达式
ExpressLR::ExpressLR(int* expressions, int length):Express(expressions, length){}

ExpressLR::ExpressLR(const vector<int>& expression):Express(expression){}


//LR符号
ElemRight::ElemRight(string name, int key, bool isFinally) : Elem(name, key, isFinally){}

bool ElemRight::add_expression(ExpressLR expression){

    this->expression_of_set.push_back(expression);
    return true;
}



//项目集合
//LR特有的
DFAState::DFAState(const vector<Project>& project, int index,int elemCount){
    for (int i = 0; i < project.size(); i++){
        this->projects.push_back(project[i]);
    }
    sort(this->projects.begin(), this->projects.end());
    this->statesCount = index;

    this->next = new int[elemCount];
    this->elementCount = elemCount;
    for (int i = 0; i < elemCount; i++){
        next[i] = -1;
    }
}

DFAState::DFAState(const DFAState& a){
    for (int i = 0; i < a.projects.size(); i++){
        this->projects.push_back(a.projects[i]);
    }
    this->statesCount = a.statesCount;

    this->next = new int[a.elementCount];
    this->elementCount = a.elementCount;
    for (int i = 0; i < elementCount; i++){
        next[i] = a.next[i];
    }
}
bool DFAState::operator==(vector<Project>& b){
    //两个项目集合的数量不一样
    if (this->projects.size() != b.size()){
        return false;
    }
    bool flag = true;

    sort(b.begin(), b.end() , cmp);

    for (int i = 0; i < b.size(); i++){
        if (projects[i] != b[i]){
            flag = false;
            break;
        }

    }

    return flag;


}
DFAState::~DFAState(){

    delete[] this->next;
}





//LR文法
//构造函数，重中之中,相当于主函数了
GrammarLR::GrammarLR(string* generate, int length){

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
        add_elem(vectorGenrates[i][0], false);
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
                    index = add_elem(result_of_name[m], true);
                }
                exp.push_back(index);

            }
            elements[find_name(vectorGenrates[i][0])].add_expression(ExpressLR(exp));
        }
    }

    add_elem("#", true);
    //添加拓广文法
    string name = elements[0].name + "'";
    int index = this->add_elem(name, false);
    vector<int> exp;
    exp.push_back(0);
    elements[index].add_expression(exp);

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
            finnalyC = finnalyC + " " + elements[i].name;
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


    //生成DFA状态表
    this->create_dfa_states();

    //检测DFA
    for (int i = 0; i < this->states.size(); i++){

        for (int j = 0; j < states[i].projects.size(); j++){

            Project tmp = states[i].projects[j];
            cout << elements[tmp.elemKey].name << "->";
            for (int m = 0; m < elements[tmp.elemKey].expression_of_set[tmp.expressIndex].expression.size(); m++){

                if (elements[tmp.elemKey].expression_of_set[tmp.expressIndex][m] != -1){
                    cout << elements[elements[tmp.elemKey].expression_of_set[tmp.expressIndex][m]].name;
                }
                else
                {
                    cout << "@";
                }


            }
            cout << "  ProjectPoint:"<<tmp.projectPoint<<endl;

        }
        for (int j = 0; j < states[i].elementCount; j++){

            if (states[i].next[j] != -1){

                cout << elements[j].name << "--" << states[i].next[j] << "  ";
            }

        }
        cout << endl;

    }

    //创建指导动作的表
    this->create_action_table();

    //将表打印出来
    char formatS[] = "%+7s";//栈的格式
    string text="";
    char buffer[8];
    for (int i = 0; i < elements.size(); i++){
        sprintf(buffer, formatS, elements[i].name.c_str());
        text = text + buffer;
    }
    text = text + "\n";
    for (int i = 0; i < action_table.size(); i++){

        for (int j = 0; j < action_table[i].size(); j++){
            string t;
            switch (action_table[i][j].type)
            {
                case ERROR:
                    t = " ";
                    break;
                case REDUCE:
                    t = "REDUCE";
                    break;
                case SMOVE:
                    t = "SMOVE";
                    break;
                case ACC:
                    t = "ACC";
                    break;
                default:
                    break;
            }
            sprintf(buffer, formatS,t.c_str());
            text = text + buffer;
        }
        text = text + "\n";
    }
    cout << text;
}




//find查找，找到的话返回下标，
//找不到则返回-2
//-1表示为空，空使用@来表示
int GrammarLR::find_name(string name_of_elem){
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
int GrammarLR::add_elem(string name, bool isFinally){
    this->name_of_keys[name] = elements.size();
    ElemRight t(name, elements.size(), isFinally);
    elements.push_back(t);
    return elements.size() - 1;
}





//修改元素
void GrammarLR::update_elem(int elemIndex, int begin, int end, vector<int>& factory){


    stringstream ss;
    ss << elements.size();
    int newIndex = add_elem(elements[elemIndex].name + ss.str(), false);

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
void GrammarLR::get_tow_char_index(){
    vector<int> t;
    t.clear();

    for (int i = 0; i < elements.size(); i++){
        if (elements[i].is_finally){
            finally_charIndex.push_back(i);
        }
        else
        {
            not_finally_charIndex.push_back(i);
        }
        first.push_back(t);
        follow.push_back(t);

    }

}



//获得First集合
void GrammarLR::create_first_set(){

    for (int i = 0; i < elements.size(); i++){
        get_first(i);
    }
}

//计算elem[elemIndex]的first集合,并且将结果放在First[elemIndex中]
//返回值，如果是true，说明这个不是空串（elemIndex！=-1）;
//		  如果是false，说明这个是空串，elemIndex=-1,级这个First集合为（-1）空
bool GrammarLR::get_first(int elemIndex){

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
            public_tool::connect_vector(first[elemIndex], temp);
        }
        //对于这种情况有没有可能出现：
        //A->TAb|c|T
        //T->@|t
        //进行递归了
        for (int i = 0; i < first_temp.size(); i++){
            calculate_expression_first(elemIndex, first_temp[i], temp, false);
            public_tool::connect_vector(first[elemIndex], temp);
        }

    }

    return true;
}



//计算elems[elemIndex]的expressIndex下标的表达式的first集合，结果放在result里面
void  GrammarLR::calculate_expression_first(int elemIndex, int expressionIndex, vector<int>& result, bool choose){

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
            if (public_tool::is_in_vector(first[elemIndex], -1)) continue;
            else{
                if (choose) this->first_temp.push_back(expressionIndex);
                break;
            }

        }

        if (get_first(elements[elemIndex].expression_of_set[expressionIndex][i])){

            if (i == elements[elemIndex].expression_of_set[expressionIndex].expression.size() - 1){
                public_tool::connect_vector(result, first[elements[elemIndex].expression_of_set[expressionIndex][i]]);
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
                public_tool::connect_vector(result, notNULL);
            }

        }
        else
        {
            if (!public_tool::is_in_vector(result, -1)) result.push_back(-1);
            break;
        }

        //-1不在这个式子里面后面不用继续了
        if (!public_tool::is_in_vector(first[elements[elemIndex].expression_of_set[expressionIndex][i]], -1)) break;

    }
}


bool lr_grammar::cmp(Project& a, Project& b){
    return a < b;
}


bool lr_grammar::is_in_vector(const vector<Project>& a, const Project& b){
    bool result = false;
    for (int i = 0; i < a.size(); i++){
        if (a[i] == b){
            result = true;
            break;
        }
    }
    return result;
}

void lr_grammar::conncet_vector(vector<Project>& a, const vector<Project>& b){
    for (int i = 0; i < b.size(); i++){
        if (!is_in_vector(a, b[i])){
            a.push_back(b[i]);
        }
    }

}


//生成Follow集合
void GrammarLR::create_follow_set(){

    vector<int> temp;
    //每个非终结符
    for (int i = 0; i < not_finally_charIndex.size(); i++){
        //计算Follw需要的是各个表达式子,遍历第i个非终结符的各个表达式
        for (int j = 0; j < elements[not_finally_charIndex[i]].expression_of_set.size(); j++){
            //第j个表达式
            ExpressLR& expreesion = elements[not_finally_charIndex[i]].expression_of_set[j];
            for (int m = 0; m < expreesion.expression.size(); m++){


                //如果是空串的话
                if (expreesion[m] == -1){
                    continue;
                }

                //是终结符不用计算
                if (elements[expreesion[m]].is_finally) continue;

                //获得First集合
                this->get_follow_by_first(i, expreesion, m + 1, temp);
                public_tool::connect_vector(follow[expreesion[m]], temp);

            }

        }
    }

    //考虑follow集合的添加
    for (int i = 0; i < not_finally_charIndex.size(); i++){
        adjust_follow(i);
    }

}



void GrammarLR::get_follow_by_first(int elemIndex, ExpressLR express, int begin, vector<int>& result){
    result.clear();
    vector<int> tmp;

    //如果是一个空串的话
    if (express[0] == -1){
        result.push_back(elemIndex);
        return;
    }

    for (begin = begin; begin < express.expression.size(); begin++){

        //如果空不在退出
        if (!public_tool::is_in_vector(first[express[begin]], -1)){
            public_tool::connect_vector(result, first[express[begin]]);
            break;
        }
        else
        {
            clear_null(first[express[begin]], tmp);
            public_tool::connect_vector(result, tmp);
        }

    }
    if (begin == express.expression.size()){
        //result.push_back(-1);
        result.push_back(elemIndex);
    }

}

void GrammarLR::adjust_follow(int index){
    //保存使用
    vector<int> tempFollow = follow[not_finally_charIndex[index]];
    for (int i = 0; i < tempFollow.size(); i++){
        if (!elements[tempFollow[i]].is_finally){
            //在原来的Follow中将这个非终结符删除
            vector<int>::const_iterator deleteIt = find(follow[not_finally_charIndex[index]].begin(), follow[not_finally_charIndex[index]].end(), tempFollow[i]);
            follow[not_finally_charIndex[index]].erase(deleteIt);

            //如果两个是一样的
            if (not_finally_charIndex[index] != tempFollow[i]) {
                adjust_follow(tempFollow[i]);

                public_tool::connect_vector(follow[not_finally_charIndex[index]], follow[tempFollow[i]]);

            }




        }

    }

}





//构造DFA
void GrammarLR::create_dfa_states(){


    //开始计算,项目的点的下标是下一个
    //项目A->.abb 点的位置为0
    vector<Project> projects;
    projects.push_back(Project(find_name(elements[0].name + "'"), 0, 0));
    closure(projects);

    this->states.push_back(DFAState(projects, 0, elements.size()));


    //开始计算
    //S0是0下标，持续增长的
    for (int stateIndex = 0; stateIndex < this->states.size(); stateIndex++){

        //每一个状态的smove都要计算一下，针对是每一个项目集合，不用针对所有的元素
        for (int i = 0; i < states[stateIndex].projects.size(); i++){

            Project tmp = states[stateIndex].projects[i];

            //这是一个可移进后面没有smove了
            if (tmp.projectPoint == elements[tmp.elemKey].expression_of_set[tmp.expressIndex].expression.size()){
                continue;
            }
            //.后面的数的元素
            int elemIndex = elements[tmp.elemKey].expression_of_set[tmp.expressIndex][tmp.projectPoint];

            //先判断这个元素是不是已经计算了,不是-1已经计算了
            if (states[stateIndex].next[elemIndex] != -1) continue;

            //计算smove;
            smove(states[stateIndex], elemIndex, projects);

            //计算空闭包
            closure(projects);

            int countIndex = states.size();
            //看看这个状态是不是已经出现过了
            for (int i = 0; i < states.size(); i++){

                if (states[i] == projects){
                    countIndex = i;
                    break;
                }
            }

            //新状态，需要被push
            if (countIndex == states.size()){
                states.push_back(DFAState(projects, countIndex, elements.size()));
            }

            //给原来的状态进行调整
            states[stateIndex].next[elemIndex] = countIndex;

        }

    }


}


//smove运算，得到的是一个状态集合,求input状态经过elem[elemIndex]后得到的DFA项目集合
void GrammarLR::smove(DFAState& input, int elemIndex, vector<Project>& result){
    //结果清除一下
    result.clear();
    int elemKey, express, projectPoint;//后面好编写

    for (int i = 0; i < input.projects.size(); i++){

        //先将需要的值赋值

        elemKey = input.projects[i].elemKey;
        express = input.projects[i].expressIndex;
        projectPoint = input.projects[i].projectPoint;

        //先判断是不是在结尾
        if (projectPoint == elements[elemKey].expression_of_set[express].expression.size()) continue;

        //不是在结尾看看是不是对应的字符
        if (elements[elemKey].expression_of_set[express][projectPoint] == elemIndex){
            result.push_back(Project(elemKey, express, projectPoint + 1));
        }
    }

}

//求空闭包,求result集合的空闭包，会添加，最后会增加到result里面
void GrammarLR::closure(vector<Project>& result){

    int elemKey, express, projectPoint;//后面好编写
    vector<Project> tmp;

    for (int i = 0; i < result.size(); i++){

        //先将需要的值赋值
        elemKey = result[i].elemKey;
        express = result[i].expressIndex;
        projectPoint = result[i].projectPoint;
        //先判断是不是在结尾
        if (projectPoint == elements[elemKey].expression_of_set[express].expression.size()) continue;

        int newElemKey = elements[elemKey].expression_of_set[express][projectPoint];
        //是终结符，不用管
        if (elements[newElemKey].is_finally) continue;

        //不是终结符号
        //先将他给赋值过来，后来继续合并
        tmp.clear();
        for (int j = 0; j < elements[newElemKey].expression_of_set.size(); j++){

            tmp.push_back(Project(newElemKey, j, 0));

        }

        conncet_vector(result, tmp);


    }


}



void GrammarLR::create_action_table(){

    //1.初始化一下表,有状态数那么多个行，有元素个数那么多个的列，初始化的所有符列都是ERROR

    vector<Action> t;
    Action action;
    action.type = ERROR;
    for (int i = 0; i < elements.size(); i++){
        t.push_back(action);
    }

    for (int i = 0; i < states.size(); i++){
        action_table.push_back(t);
    }


    //2.以状态为基础遍历填表
    for (int i = 0; i < states.size(); i++){

        //转移的直接提取就行,遍历next函数
        for (int j = 0; j < states[i].elementCount; j++){

            //不等于-1说明可以接受这个转移
            if (states[i].next[j] != -1){

                if (action_table[i][j].type != ERROR){
                    cout << "冲突";
                    cout << action_table[i][states[i].next[j]].type << "to" << " SMOVE";
                }
                action_table[i][j].type = SMOVE;
                action_table[i][j].choose.state = states[i].next[j];

            }

        }


        //归约开始,需要检查是每一个项目，判断点的位置是不是在最后
        for (int j = 0; j < states[i].projects.size(); j++){
            Project tmpP = states[i].projects[j];
            if (elements[tmpP.elemKey].expression_of_set[tmpP.expressIndex].expression.size() == tmpP.projectPoint){

                //表示点在表达式最后，进行归约
                //先判断是不是终结符号,是就是acc而不是REDUCE
                if (tmpP.elemKey == find_name(elements[0].name + "'")){
                    action_table[i][find_name("#")].type = ACC;
                    continue;

                }
                for (int fi = 0; fi < follow[tmpP.elemKey].size(); fi++){

                    if (action_table[i][follow[tmpP.elemKey][fi]].type != ERROR){
                        cout << "冲突";
                        cout << action_table[i][follow[tmpP.elemKey][fi]].type << "to" << " REDUCE";
                    }
                    action_table[i][follow[tmpP.elemKey][fi]].type = REDUCE;
                    action_table[i][follow[tmpP.elemKey][fi]].choose.R.elemIndex = tmpP.elemKey;
                    action_table[i][follow[tmpP.elemKey][fi]].choose.R.expressIndex = tmpP.expressIndex;
                }

            }


        }






    }

}

void lr_grammar::clear_null(const vector<int>& a, vector<int>& result, int value){
    result.clear();
    for (int i = 0; i < a.size(); i++){
        if (a[i] != value){
            result.push_back(a[i]);
        }
    }

}




//仿真运算了，这一部分需要先进行一些数据结构的定义

//栈的元素
struct stackElem
{
    int states;
    int elemKey;
};

//根据预测分析表进行判断
bool  GrammarLR::sim(char* testString){

    bool result = true;
    //不使用testString，出现双字符串无法解释;
    vector<string> ipString;
    public_tool::parse_elem(testString, ipString);

    stack<stackElem> s;//指导栈
    vector<stackElem> elemprint;//输出作用
    char formatS[] = "%-25s";//栈的格式
    char formatC[] = "%+15s         ";//字母格式的格式
    string bufS;
    char buffer[200];		//格式化的辅助
    stringstream sstream;
    string tmp;				//字符串中间
    string text;			//输出的串
    int ipIndex = 0;//ip的指向

    stackElem e;
    e.elemKey = find_name("#");//#是开始字符
    e.states = 0;//0是初态
    s.push(e);
    elemprint.push_back(e);
    ReduceByExpress r;
    int st;
    while (action_table[e.states][find_name(ipString[ipIndex])].type == REDUCE || action_table[e.states][find_name(
            ipString[ipIndex])].type == SMOVE){
        e = s.top();

        //1.输出栈
        tmp = "";
        for (int i = 0; i < elemprint.size(); i++){
            tmp = tmp + elements[elemprint[i].elemKey].name;
            sstream.clear();
            sstream << elemprint[i].states;
            sstream >> buffer;
            tmp = tmp + buffer;
        }
        sprintf(buffer, formatS, tmp.c_str());
        text = text + buffer;
        //2.当前输入
        tmp = "";
        for (int i = ipIndex; i < ipString.size(); i++){
            tmp = tmp + ipString[i];
        }
        sprintf(buffer, formatC, tmp.c_str());
        text = text + buffer;

        switch (action_table[e.states][find_name(ipString[ipIndex])].type)
        {
            //不会出现
            case ERROR: break;

            case REDUCE:
                //归约操作
                //3.进行的动作
                text = text + "REDUCE:";
                //get表达式
                r = action_table[e.states][find_name(ipString[ipIndex])].choose.R;
                text = text + elements[r.elemIndex].name + "->";
                for (int i = 0; i < elements[r.elemIndex].expression_of_set[r.expressIndex].expression.size(); i++){
                    if (elements[r.elemIndex].expression_of_set[r.expressIndex][i] == -1){ continue; }
                    text = text + elements[elements[r.elemIndex].expression_of_set[r.expressIndex][i]].name;
                }
                text = text + "\n";

                //出栈
                for (int i = 0; i < elements[r.elemIndex].expression_of_set[r.expressIndex].expression.size(); i++){
                    s.pop();
                    elemprint.pop_back();
                }
                //替换终结符操作
                e = s.top();
                if (action_table[e.states][r.elemIndex].type == ERROR){
                    text = text + "ERROR";
                    break;//跳出swtich
                }
                e.elemKey = r.elemIndex;
                e.states = action_table[e.states][r.elemIndex].choose.state;
                s.push(e);
                elemprint.push_back(e);
                break;
            case SMOVE:
                //3.进行的动作
                text = text + "SMOVE:";
                //get表达式
                st = action_table[e.states][find_name(ipString[ipIndex])].choose.state;
                sstream.clear();
                sstream << st;
                sstream >> buffer;
                text = text + buffer + "\n";

                //入栈就行
                e.elemKey = find_name(ipString[ipIndex]);
                e.states = st;
                s.push(e);
                elemprint.push_back(e);
                ipIndex++;
                break;
                //不会出现
            case ACC: break;
            default:
                break;
        }
    }


    if (action_table[e.states][find_name(ipString[ipIndex])].type == ERROR) {
        result = false;
        text = text + "ERROR\n";
    }
    else
    {
        text = text + "ACC\n";
    }
    cout << text;
    return result;
}



