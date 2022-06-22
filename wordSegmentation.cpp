//
// 构建临时索引、单词表、网站表
//
#include "base.hpp"
using namespace std;
#define PAGE_PER_FILE 50  //一个临时索引文件中网页的数量
//基于jieba的中文分词器
WordSegmentation wordSeg;

//储存单词和单词编号的映射
class WordMap {
public:
    WordMap() {
        wordCnt = 0;
        dict.clear();
    }
    //获取一个单词的编号,在单词未出现时自动添加
    int getID(string word) {
        int ID = wordTrie.query(word);
        if(ID == -1) {
            dict.push_back(word);
            wordTrie.insert(word, ++wordCnt);
            return wordCnt;
        }
        else
            return ID;
    }
    //查询一个单词的编号,在单词未出现时返回最大值表示不存在
    int queryID(string word) {
        int ID = wordTrie.query(word);
        if(ID == -1)
            return 0x7fffffff;
        else
            return ID;
    }
    //根据文件建立字典树
    void buildFromFile() {
        ifstream inFile(WORD_DICT_PATH);
        inFile >> wordCnt;
        for(int i = 0; i < wordCnt; i++) {
            int id;
            string word;
            inFile >> id >> word;
            wordTrie.insert(word, id);
        }
        inFile.close();
    }
    //输出到文件
    void saveToFile() {
        ofstream outputFile(WORD_DICT_PATH);
        outputFile << wordCnt << endl;
        for(int i = 0; i < wordCnt; i++) {
            outputFile << i + 1 << " " << dict[i] << endl;
        }
        outputFile.close();
    }

private:
    int wordCnt;
    vector<string> dict;  //总词典
    Trie wordTrie;  //建立单词与编号的字典树
} wordDict;
//储存网站和网站编号的映射
class WebsiteMap {
public:
    WebsiteMap() {
        websiteCnt = 0;
        dict.clear();
    }
    //在尾部增加一个网站
    void push_back(string word, string header) {
        websiteCnt++;
        dict.push_back(word);
        title.push_back(header);
    }
    //输出到文件
    void saveToFile() {
        ofstream outputFile(WEBSITE_DICT_PATH);
        outputFile << websiteCnt << endl;
        for(int i = 0; i < websiteCnt; i++) {
            outputFile << i + 1 << " " << dict[i] << " " << title[i] << endl;
        }
        outputFile.close();
    }

private:
    int websiteCnt;
    vector<string> dict;  //每个网站的网址
    vector<string> title;  //每个网站的标题
} websiteDict;

//记时功能，便于效率分析
clock_t startTime, endTime;
void checkTime(bool print) {
    endTime = clock();
    if(print)
        cerr << " -> stage time use:" << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
    startTime = clock();
}
int main() {
    // freopen("./Input_data/data.in", "r", stdin);
    // freopen("./Output_data/data.out", "w", stdout);
    // system("chcp 65001");
    /////
    checkTime(0);
    /////
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
        if(pageID % PAGE_PER_FILE == 0) {  //根据前面定义的宏确定一个文件中的网页个数
            outTempIndex.close();
            string targetFile = TEMP_INDEX_PATH + to_string(pageID / PAGE_PER_FILE) + ".txt";
            cerr << "at file: " << targetFile << endl;
            outTempIndex.open(targetFile);
        }
        pageID++;
        // cerr << "now website:" << pageID << endl;

        // 获取标题
        getline(inText, word);
        clearHeadTailSpace(word);

        websiteDict.push_back(herf, word);
        // cerr << word << endl;
        // 开始处理
        while(1) {
            if(word == "|")
                break;
            vector<string> res = wordSeg.segmentation(word);
            for(auto nowWord: res) {
                outTempIndex << wordDict.getID(nowWord) << "\t" << pageID << "\n";
                // outTempIndex << nowWord << "\t" << pageID << "\n";
            }
            getline(inText, word);
            clearHeadTailSpace(word);
        }
        getline(inText, id);
    }

    outTempIndex.close();
    /////
    checkTime(1);
    /////

    wordDict.saveToFile();
    websiteDict.saveToFile();

    /////
    checkTime(1);
    /////

    return 0;
}