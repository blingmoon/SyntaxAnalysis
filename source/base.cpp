//
// Created by 周亮 on 2019/4/18.
//
#include "../head/base.h"

using namespace baseGramer;
Elem::Elem(string name, int key, bool isFinally):name(name),key(key),isFinally(isFinally){};

Express::Express(int* expressions, int length):expression(expressions,expressions+length){};
Express::Express(const vector<int>& expression):expression(expression.begin(),expression.end()){};

int& Express::operator[](int index){

    return expression[index];
}

bool Express::operator>(const Express& b){

    int maxSize = this->expression.size()<b.expression.size() ? this->expression.size() : b.expression.size();

    for (int i = 0; i < maxSize; i++){
        if (expression[i] < b.expression[i]) return false;
        if (expression[i] > b.expression[i])return true;
    }
    if (maxSize == this->expression.size()) return false;
    return true;



}

vector<string>& publicTools::parseProductions(string EX, vector<string>& productions){
    productions.clear();
    string buffer = "";
    int index = 0;		//下标

    //非终结符号,是以->结束的
    for (; index < EX.length(); index++){
        if (EX[index] == '-'){
            if (EX[index + 1] == '>') {
                index = index + 2;
                break;
            }
        }
        buffer += EX[index];
    }
    productions.push_back(buffer);
    buffer = "";

    //解析各个表达式;
    for (int i = index; i < EX.length(); i++){
        if (EX[i] == '?'){
            buffer = buffer + '?' + EX[++i];
        }
        //一个表达式完成
        if (EX[i] == '|'){
            i++;
            productions.push_back(buffer);
            buffer = "";
        }
        buffer = buffer + EX[i];

    }
    if (buffer != ""){
        productions.push_back(buffer);
        buffer = "";
    }


    return productions;
};
vector<string>& publicTools::parseElem(string production, vector<string>& elems){

    elems.clear();
    string temp = "";

    //遍历全字符
    for (int i = 0; i < production.length(); i++){



        if (production[i] != '?'&&production[i] != '$'){
            temp = production[i];
            elems.push_back(temp);
            continue;
        }

        //转义字符修改一下，可能遇见的有?$,??
        if (production[i] == '?'){
            temp = production[++i];
            elems.push_back(temp);
        }

        //到这一步说明前面没有?,后面是一个长的字符串
        if (production[i] == '$'){
            temp = "";
            while (production[++i] != '$')
            {
                if (production[i] == '?'){
                    temp = temp + production[++i];
                }
                temp = temp + production[i];
            }
            elems.push_back(temp);

        }

    }
    return elems;

};
//合并向量a和向量b,没有重复的结果放在向量a中
void publicTools::connetVector(vector<int>& a, const vector<int>& b){
    for (int i = 0; i < b.size(); i++){
        if (!isInA(a, b[i])){
            a.push_back(b[i]);
        }
    }
}



//判断b是不是在向量a中
//在的话返回true，不在的话返回false
bool publicTools::isInA(const vector<int>& a, int b){
    bool result = false;
    for (int i = 0; i < a.size(); i++){
        if (a[i] == b){
            result = true;
            break;
        }
    }
    return result;

}



