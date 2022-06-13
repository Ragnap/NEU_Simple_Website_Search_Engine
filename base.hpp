#include "cppjieba/include/cppjieba/Jieba.hpp"
#include "myTrie.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <windows.h>
//////////////////////////////////////////常量定义///////////////////////////////////////
// jieba词典路径
const char* const DICT_PATH = "./cppjieba/dict/jieba.dict.utf8";  //最大概率法分词所使用的词典路径
const char* const HMM_PATH = "./cppjieba/dict/hmm_model.utf8";  //隐式马尔科夫模型分词所使用的词典路径
const char* const USER_DICT_PATH = "./cppjieba/dict/user.dict.utf8";  //用户自定义词典路径
const char* const IDF_PATH = "./cppjieba/dict/idf.utf8";  // IDF路径
const char* const STOP_WORD_PATH = "./cppjieba/dict/stop_words.utf8";  //停用词路径
const char* const REMOVE_WORD_PATH = "./cppjieba/dict/REMOVE_words.utf8";  //无意义词路径
//用户输入词路径
const char* const INPUT_WORD_PATH = "./inputWord.txt";
//用户输入词编号路径
const char* const INPUT_WORD_ID_PATH = "./inputWordID.txt";
//网站文本路径
const char* const TEXT_PATH = "./websiteText.txt";
//临时索引文件夹路径
const char* const TEMP_INDEX_PATH = "./tempIndex/tempIndex";
//单词编号文件
const char* const WORD_DICT_PATH = "./mapWord.txt";
//网站编号文件
const char* const WEBSITE_DICT_PATH = "./mapWebsite.txt";
//倒序索引文件路径
const char* const EXTERIOR_INDEX_PATH = "./exteriorIndex.txt";
//查询结果网站编号路径
const char* const RESULT_PATH = "./result.txt";
// 最大网页数目
const int PAGE_SIZE = 3500;
// 最大单词编号
const int WORD_SIZE = 100000;
// 移除无意义词
const bool REMOVE_MEANLESS_WORD = 1;

////////////////////////////////////////// 类定义 ///////////////////////////////////////

// jieba分词器类
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
                //检查是否在无意义词表里
                // if(removeWord[nowWord] != 1)  // map实现
                if(!removeWord.check(nowWord))  // trie实现
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
            // removeWord[word] = 1;
            removeWord.insert(word);
        }
    }

private:
    cppjieba::Jieba tool;  // jieba分词类
    // map<string, bool> removeWord;  // 无意义词表,map实现
    Trie removeWord;  // 无意义词表,trie实现
};

//////////////////////////////////////////参数设置///////////////////////////////////////

//控制台宽度
int WID = 150;
//字体颜色控制
void set_color(int Color) {
    /*
     * 0:黑，1：蓝 ， 2： 绿 ， 3：湖蓝， 4： 红 ， 5:  紫 ， 6：黄 ， 7： 白
     * 8:灰，9:淡蓝，10：淡绿，11：浅绿，12：淡红，13：淡紫，14:淡黄，15：亮白
     */
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
}
//控制台的大小设置
void set_size(int Width, int Height) {
    // 重新设置窗口大小
    WID = Width;
    char temp[50] = "";
    sprintf(temp, "mode con cols=%d lines=%d", Width, Height);
    system(temp);
    // 重新设置缓冲区大小
    HANDLE handle_out;
    COORD size = {WID, (SHORT)1000};
    handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(handle_out, size);
}

//////////////////////////////////////////输入函数///////////////////////////////////////
//去掉字符串收尾空格
string& clearHeadTailSpace(string& str) {
    if(str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}
//从字符串str的pos位置开始读取一个正数，若无法获得数字就返回-1
int getNumber(std::string& str, int& pos) {
    int number = 0;
    while((pos < str.length()) && (str[pos] > '9' || str[pos] < '0')) {  //过滤非数字字符
        pos++;
    }
    if(pos == str.length())
        return -1;
    while((pos < str.length()) && (str[pos] >= '0' && str[pos] <= '9')) {  //读取所有数字字符
        number = number * 10 + str[pos++] - '0';
    }
    return number;
}
//从字符串str的0位置开始读取一个正数，若无法获得数字就返回-1
int getNumber(std::string& str) {
    int number = 0, pos = 0;
    while((pos < str.length()) && (str[pos] > '9' || str[pos] < '0')) {  //过滤非数字字符
        pos++;
    }
    if(pos == str.length())
        return -1;
    while((pos < str.length()) && (str[pos] >= '0' && str[pos] <= '9')) {  //读取所有数字字符
        number = number * 10 + str[pos++] - '0';
    }
    return number;
}
//////////////////////////////////////////输出函数///////////////////////////////////////

//输出number个空格
void print_space(int number) {
    while(number--) {
        std::cout << " ";
    }
}
//居中输出一串字符
void print_mid(std::string text) {
    print_space((WID - text.length()) >> 1);
    std::cout << text << std::endl;
}
//居中输出一串黄色字符
void print_yellow(std::string text) {
    set_color(6);
    print_mid(text);
    set_color(7);
}
//居中输出一串红色字符
void print_red(std::string text) {
    set_color(4);
    print_mid(text);
    set_color(7);
}

//////////////////////////////////////////转换函数///////////////////////////////////////

// GB2312转换到UTF8
std::string GB2312_to_UTF8(const char* GB2312_str) {
    int len = MultiByteToWideChar(CP_ACP, 0, GB2312_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, GB2312_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    delete[] wstr;
    std::string UTF8_str = str;
    delete[] str;
    return UTF8_str;
}
std::string GB2312_to_UTF8(std::string GB2312_str) {
    return GB2312_to_UTF8(GB2312_str.c_str());
}

// UTF8转换到GB2312
std::string UTF8_to_GB2312(const char* UTF8_str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, UTF8_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, UTF8_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    delete[] wstr;
    std::string GB2312_str = str;
    delete[] str;
    return GB2312_str;
}
std::string UTF8_to_GB2312(std::string UTF8_str) {
    return UTF8_to_GB2312(UTF8_str.c_str());
}