#include "cppjieba/include/cppjieba/Jieba.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
// jieba词典路径
const char* const DICT_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/jieba.dict.utf8";  //最大概率法分词所使用的词典路径
const char* const HMM_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/hmm_model.utf8";  //隐式马尔科夫模型分词所使用的词典路径
const char* const USER_DICT_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/user.dict.utf8";  //用户自定义词典路径
const char* const IDF_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/idf.utf8";  // IDF路径
const char* const STOP_WORD_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/stop_words.utf8";  //停用词路径
const char* const REMOVE_WORD_PATH = "./simple_website_search_engine/wordSegmetation/cppjieba/dict/REMOVE_words.utf8";  //无意义词路径
//数据文件储存路径
const char* const TEXT_PATH = "./simple_website_search_engine/websiteText.txt";  //文本路径
const char* const OUTPUT_TEMP_TEXT_PATH = "./simple_website_search_engine/tempIndex/tempIndex";  //临时索引文件夹路径
const char* const WORD_MAP_PATH = "./simple_website_search_engine/mapWord.txt";  //单词编号文件
const char* const WEBSITE_MAP_PATH = "./simple_website_search_engine/mapWebsite.txt";  //网站编号文件
#define REMOVE_MEANLESS_WORD 1  //当值为1时移除无意义词
#define PAGE_PER_FILE 50  //一个临时索引文件中网页的数量
//基于jieba的中文分词器
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
//储存单词和单词编号的映射
class WordMap {
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
    //输出到文件
    void saveToFile() {
        ofstream outputFile(WORD_MAP_PATH);
        outputFile << wordCnt << endl;
        for(int i = 0; i < wordCnt; i++) {
            outputFile << i + 1 << " " << map[i] << endl;
        }
        outputFile.close();
    }

private:
    int wordCnt;
    vector<string> map;
} wordMap;
//储存网站和网站编号的映射
class WebsiteMap {
public:
    WebsiteMap() {
        websiteCnt = 0;
        map.clear();
    }
    //在尾部增加一个网站
    void push_back(string word) {
        websiteCnt++;
        map.push_back(word);
    }
    //输出到文件
    void saveToFile() {
        ofstream outputFile(WEBSITE_MAP_PATH);
        outputFile << websiteCnt << endl;
        for(int i = 0; i < websiteCnt; i++) {
            outputFile << i + 1 << " " << map[i] << endl;
        }
        outputFile.close();
    }

private:
    int websiteCnt;
    vector<string> map;
} websiteMap;
//去掉收尾空格
string& clearHeadTailSpace(string& str) {
    if(str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

int main() {
    freopen("./Input_data/data.in", "r", stdin);
    freopen("./Output_data/data.out", "w", stdout);

    int pageID = 0;
    ifstream inText(TEXT_PATH);
    ofstream outTempIndex;
    while(1) {
        string id, herf, title, word;
        //分词开始
        getline(inText, id);
        if(id.empty())
            break;

        //读取链接
        getline(inText, herf);
        //对应到网站编号
        if(pageID % PAGE_PER_FILE == 0) {  //每30个网页用一个文件
            outTempIndex.close();
            string targetFile = OUTPUT_TEMP_TEXT_PATH + to_string(pageID / PAGE_PER_FILE) + ".txt";
            cerr << "at file: " << targetFile << endl;
            outTempIndex.open(targetFile);
        }
        pageID++;
        cerr << "now website:" << pageID << endl;
        websiteMap.push_back(herf);

        //处理标题
        // getline(inText, title);
        // outTempIndex << "title:" << endl << " -> ";
        // vector<string> res = wordSeg.segmentation(title);
        // for(auto nowWord: res) {
        //     outTempIndex << nowWord << " ";
        // }
        // outTempIndex << endl;

        //处理正文
        getline(inText, word);
        clearHeadTailSpace(word);
        while(1) {
            if(word == "|")
                break;
            vector<string> res = wordSeg.segmentation(word);
            for(auto nowWord: res) {
                outTempIndex << wordMap.getID(nowWord) << "\t" << pageID << "\n";
            }

            getline(inText, word);
            clearHeadTailSpace(word);
        }
        getline(inText, id);
    }
    outTempIndex.close();
    wordMap.saveToFile();
    websiteMap.saveToFile();
    return 0;
}