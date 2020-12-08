//
// Created by Lance on 2020/12/5.
//

#include <bits/stdc++.h>
#include "grammar_2.h"

using namespace std;
const int MAXX = 550;
grammar G;

/**
 * S'->#S#
 */
grammar change(grammar &G) {

    char vn = get_Vn(G.Vn);
    string tem = "#" + G.Vn.substr(0, 1);
    tem += "#";
    G.P[vn].push_back(tem);
    G.S = vn;
    G.Vt += '#';
    G.Vn = G.S + G.Vn;
    return G;
}
/**
 * 求FIRSTVT
 */
map<char, set<char> > FIRSTVT;
map<pair<char, char>, bool> F;
stack<pair<char, char> > S;

void insert(char P, char a) {
    if (!F[{P, a}]) {
        F[{P, a}] = 1;
        S.push({P, a});
    }
}

void getFIRSTVT(grammar G) {
    FIRSTVT.clear();
    F.clear();
    for (auto vn : G.Vn)
        for (auto a : G.P[vn])
            if (is_Vt(a[0]))
                insert(vn, a[0]);
            else if (is_Vt(a[1]))
                insert(vn, a[1]);

    while (!S.empty()) {
        auto top = S.top();
        S.pop();
        for (auto vn : G.Vn)
            for (auto a : G.P[vn])
                if (a[0] == top.first)
                    insert(vn, top.second);
    }
    // 二维数组转为集合
    for (auto vn : G.Vn)
        for (auto vt : G.Vt)
            if (F[{vn, vt}])
                FIRSTVT[vn].insert(vt);
    // 打印
    cout << "FIRSTVT集合:" << endl;
    for (auto it : FIRSTVT) {
        cout << "FIRSTVT(" << it.first << ')' << " : ";
        for (auto setIt : it.second)
            cout << setIt << ' ';
        puts("");
    }
    puts("");
}

/**
 * 求LASTVT
 */
map<char, set<char> > LASTVT;
void getLASTVT(grammar G) {
    LASTVT.clear();
    F.clear();
    for (auto vn : G.Vn)
        for (auto a : G.P[vn])
            if (is_Vt(a[a.size() - 1]))
                insert(vn, a[a.size() - 1]);
            else if (is_Vt(a[a.size() - 2]))
                insert(vn, a[a.size() - 2]);

    while (!S.empty()) {
        auto top = S.top();
        S.pop();
        for (auto vn : G.Vn)
            for (auto a : G.P[vn])
                if (a[a.size() - 1] == top.first)
                    insert(vn, top.second);
    }
    // 二维数组转为集合
    for (auto vn : G.Vn)
        for (auto vt : G.Vt)
            if (F[{vn, vt}])
                LASTVT[vn].insert(vt);
    // 打印
    cout << "LASTVT集合:" << endl;
    for (auto it : LASTVT) {
        cout << "LASTVT(" << it.first << ')' << " : ";
        for (auto setIt : it.second)
            cout << setIt << ' ';
        puts("");
    }
    puts("");
}


/**
 * 构造优先关系表
 */
map<pair<char, char>, char> relationshipTable;
void getPriorityRelationshipTable(grammar G) {
    for (auto vn : G.Vn) {
        for (auto x : G.P[vn]) {

            for (int i = 0; i < x.size() - 1; i++) {
                // Xi和Xi+1均为终结符, 置Xi=Xi+1
                if (is_Vt(x[i]) && is_Vt(x[i + 1]))
                    relationshipTable[{x[i], x[i + 1]}] = '=';
                // i≤n-2且Xi和Xi+2都为终结符，但Xi+1为非终结符, 置Xi=Xi+2:
                if (i <= x.size() - 2 && is_Vt(x[i]) && is_Vt(x[i + 2]) && !is_Vt(x[i + 1]))
                    relationshipTable[{x[i], x[i + 2]}] = '=';
                // Xi为终结符而Xi+1为非终结符Xi<FIRSTVT(Xi+1)
                if (is_Vt(x[i]) && !is_Vt(x[i + 1]))
                    for (auto it : FIRSTVT[x[i + 1]])
                        relationshipTable[{x[i], it}] = '<';
                // Xi为非终结符而Xi+1为终结符LASTVT(Xi)>Xi+1
                if (!is_Vt(x[i]) && is_Vt(x[i + 1]))
                    for (auto it : LASTVT[x[i]])
                        relationshipTable[{it, x[i + 1]}] = '>';
            }
        }
    }
    // 打印
    cout << "优先关系表\n   ";
    for (auto x : G.Vt) cout << x << "  ";
    puts("");
    for (auto x : G.Vt) {
        cout << x << "  ";
        for (auto y : G.Vt) {
            cout << relationshipTable[{x, y}] << "  ";
        }
        cout << '\n';
    }

}

/**
 * 算符优先分析
 */
bool ERROR() {
    cout << "出错了!!!!!!!!!!" << endl;
    return false;
}

bool operatorPrecedenceAnalysis(grammar G, string str) {
    str = str + "#";
    // top 指向栈顶，j指向最接近栈顶的终结符
    int top = 1, j = 0, index = 0;
    char a;
    char S[MAXX];
    S[top] = '#';
    while (true) {
        a = str[index++];
        j = (is_Vt(S[top]) ? top : top - 1);
        while (relationshipTable[{S[j], a}] == '>') {
            while (true) {
                char Q = S[j];
                j = (is_Vt(S[j - 1]) ? j - 1 : j - 2);

                if (relationshipTable[{S[j], Q}] == '<') {
                    break;
                }
            }
            /**
             * 归约
             * S[j + 1]...S[k]归约成N
             */
            top = j + 1;
            S[top] = G.S;
        }
        if (relationshipTable[{S[j], a}] == '<' || relationshipTable[{S[j], a}] == '=') {
            top++;
            S[top] = a;
        } else return ERROR();
        if (a == '#') break;
    }
    return true;
}

int main() {
    char ch;
    G = grammar_read("D:\\work\\clion\\POC\\sy3\\1.txt");
    change(G);

    print_G(G);

    getFIRSTVT(G);
    getLASTVT(G);
    getPriorityRelationshipTable(G);
//    cout << operatorPrecedenceAnalysis(G, "i+i");

    FILE* fpin2;
    if((fpin2=fopen(R"(D:\work\clion\POC\sy3\jz.txt)","r"))==NULL) {
        cout<<"文件路径错误\n";
        return 0;
    }
    string str = "";
    vector<string> query, copy;
    while ((ch) != EOF) {
        str = "";
        while ((ch = GetBC(fpin2)) != ';') {
            if (ch == EOF) break;
            str += ch;
        }
        query.push_back(str);
        if (ch == EOF) break;
        ch = GetBC(fpin2);
    }
    string test_in;

    copy = query;
    for (auto &it : query) it = lexical(it);
    for (auto it : query) cout << it << endl;
    for (int i = 0; i < query.size(); i++) {
        cout << copy[i] << endl;
        cout << (operatorPrecedenceAnalysis(G, query[i]) ? "正确" : "错误") << endl;
    }
    return 0;
}
