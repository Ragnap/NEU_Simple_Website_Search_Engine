#include "cppjieba/include/cppjieba/Jieba.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
const char* const DICT_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/jieba.dict.utf8";  //最大概率法分词所使用的词典路径
const char* const HMM_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/hmm_model.utf8";  //隐式马尔科夫模型分词所使用的词典路径
const char* const USER_DICT_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/user.dict.utf8";  //用户自定义词典路径
const char* const IDF_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/idf.utf8";  // IDF路径
const char* const STOP_WORD_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/stop_words.utf8";  //停用词路径
const char* const REMOVE_WORD_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/REMOVE_words.utf8";  //无意义词路径
const char* const TEXT_PATH = "./algorithm_class/wordSegmetation/cppjieba/dict/REMOVE_words.utf8";  //文本路径
#define REMOVE_MEANLESS_WORD 1  //当值为1时移除无意义词
class WordSegmentation {
public:
    WordSegmentation()
        : tool(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)  //初始化Jieba类对象
    {
        buildRemoveWord();
    }
    //返回str的分词结果
    vector<string> segmentation(const string str) {
        vector<string> res;
        tool.CutForSearch(str, res);
        if(REMOVE_MEANLESS_WORD) {
            vector<string> temp;
            for(auto nowWord: res) {
                if(removeWord[nowWord] != 1)  //检查是否在无意义词表里
                    temp.push_back(nowWord);
            }
            return temp;
        }
        else
            return res;
    }
    //构建无意义词表
    void buildRemoveWord() {
        ifstream in(REMOVE_WORD_PATH);
        string word;
        while(in >> word) {
            removeWord[word] = 1;
        }
    }

private:
    cppjieba::Jieba tool;  // jieba分词类
    map<string, bool> removeWord;  //无意义词表
} wordSeg;
class WordMap {  //储存单词和单词编号的映射
public:
    WordMap() {
        wordCnt = 0;
        map.clear();
    }
    //查询是否在映射表里，在的话返回对应编号，否则返回0
    int find(string word) {
        for(int i = 0; i < wordCnt; i++) {
            if(word == map[i])
                return i + 1;
        }
        return 0;
    }
    //获取一个单词的编号
    int getID(string word) {
        int ID = find(word);
        if(ID == 0) {
            map.push_back(word);
            return ++wordCnt;
        }
        else
            return ID;
    }

private:
    int wordCnt;
    vector<string> map;
} wordMap;
//去掉收尾空格
string& clearHeadTailSpace(string& str) {
    if(str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}
//读取一行内容
string getStringWord() {
    string word;
    getline(cin, word);
    clearHeadTailSpace(word);
    return word;
}

int main() {
    freopen("./Input_data/data.in", "r", stdin);
    freopen("./Output_data/data.out", "w", stdout);

    string id, herf, title, word;
    // cin >> id;
    // vector<string> res = wordSeg.segmentation(id);
    // for(auto i: res) {
    //     cout << i << endl;
    // }
    int cnt = 0;
    while(1) {
        cout << endl << "##############" << endl;
        getline(cin, id);
        if(id.empty())
            break;
        getline(cin, herf);
        cout << id << " " << herf << endl;
        cout << "# ------------" << endl;

        getline(cin, title);
        cout << "title:" << endl << " -> ";
        vector<string> res = wordSeg.segmentation(title);
        for(auto nowWord: res) {
            cout << nowWord << " ";
        }
        cout << endl;

        word = getStringWord();
        while(1) {
            // cout << "#  --" << word << endl;
            if(word == "|")
                break;
            res = wordSeg.segmentation(word);
            for(auto nowWord: res) {
                cout << wordMap.getID(nowWord) << ":" << nowWord << endl;
            }
            cout << endl;
            word = getStringWord();
        }
        getline(cin, id);
    }

    return 0;
}