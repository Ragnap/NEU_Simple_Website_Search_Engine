//
// 用户交互主页面
//
#include "base.hpp"
using namespace std;
//记时功能，便于效率分析
clock_t startTime, endTime;
void checkTime(bool print) {
    endTime = clock();
    if(print)
        cerr << " -> stage time use:" << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
    startTime = clock();
}
// 输入分词器
WordSegmentation* wordSeg = nullptr;
// 储存单词和单词编号的映射用于生成用户输入的编号文件
class WordMap {
public:
    WordMap() {
        build();
    }
    //根据文件建立字典树
    void build() {
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
    //获取一个单词的编号,在单词未出现时返回最大值
    int query(string word) {
        int ID = wordTrie.query(word);
        if(ID == -1)
            return 0x7fffffff;
        else
            return ID;
    }

private:
    int wordCnt;
    Trie wordTrie;  //建立单词与编号的字典树
} wordDict;
// 网页总数
int websiteNum;
// 默认展示个数
int baseShowNum = 5;
// 保存网页链接
string websiteLink[PAGE_SIZE];
// 保存网页标题
string websiteTitle[PAGE_SIZE];
// 该网页中有出现过的检索词的种类数
int websiteWordKind[PAGE_SIZE];
// 保存所有检索词在该网页出现的次数
int websiteWordTime[PAGE_SIZE];
// 保存用户输入的单词
vector<string> inputWords;
// 初始化网页数据
void initWebsite() {
    ifstream websiteData(WEBSITE_DICT_PATH);
    websiteData >> websiteNum;
    int id;
    string text;
    while(websiteData >> id) {
        websiteData >> text;
        websiteLink[id] = UTF8_to_GB2312(text);
        getline(websiteData, text);
        websiteTitle[id] = UTF8_to_GB2312(text);
    }
    websiteData.close();
}
// 将单词对应到编号,并输出到文件
void getWordsID() {
    ofstream inputWordID(INPUT_WORD_ID_PATH);
    for(int i = 0; i < inputWords.size(); i++) {
        inputWordID << wordDict.query(inputWords[i]) << endl;
    }
    inputWordID.close();
}

// 进行查找
void getResult() {
    system("getSearchResult.exe");
}
// 输出搜索结果
void showResult() {
    ifstream result(RESULT_PATH);
    int relativeNum;  //有关的网页数量
    result >> relativeNum;
    if(relativeNum == 0) {  //未检索到
        print_red("没有与此相关的结果！请尝试其他搜索！");
        print_red("按任意键返回");
        getchar();
    }
    else {
        string header, input, temp;
        header = "搜索用时 " + to_string((double)(endTime - startTime)) + " ms , 共有 " + to_string(relativeNum) + " 个相关网页";
        vector<int> matchWebsite;  //按照搜索顺序保存的网页的编号
        int ID;  //当前网页编号
        for(int i = 0; i < relativeNum; i++) {
            result >> ID;
            result >> websiteWordKind[ID];
            result >> websiteWordTime[ID];
            matchWebsite.push_back(ID);
        }
        int base = 0;  // 当前界面显示的第一个网页位置
        int lim = baseShowNum;  //显示个数的限制
        while(1) {
            system("cls");
            cout << endl;
            print_yellow(header);
            cout << endl;
            print_mid("**************************************");
            for(int i = 0; i < lim; i++) {
                cout << endl;
                ID = matchWebsite[base + i];
                print_mid(websiteTitle[ID]);
                print_mid(websiteLink[ID]);
                temp = "包含 " + to_string(websiteWordKind[ID]) + " 个关键词，所有关键词出现了 " + to_string(websiteWordTime[ID]) + " 次";
                print_mid(temp);
                cout << endl;
                print_mid("**************************************");
            }
            cout << endl;
            if(base > 0)
                print_yellow("查看上一页，请输入1");
            if(base < relativeNum - 1)
                print_yellow("查看下一页，请输入2");
            print_yellow("展示所有搜索结果，请输入0");
            print_yellow("返回上级菜单，请输入其他");
            cin >> input;
            if(base > 0 && input == "1") {
                base -= baseShowNum;
                lim = baseShowNum;
            }
            else if(base < relativeNum - 1 && input == "2") {
                base += baseShowNum;
                lim = baseShowNum;
            }
            else if(input == "0") {
                base = 0;
                lim = relativeNum;
            }
            else
                break;
        }
    }
    result.close();
}
// 多单词模式输入
void inputDividedWord() {
    print_yellow("请在一行中输入所有单词，以空格分隔");
    string input;
    getline(cin, input);  //读掉回车
    getline(cin, input);
    inputWords.clear();
    string now;
    for(int i = 0; i < input.length(); i++) {
        if(input[i] == ' ') {
            inputWords.push_back(GB2312_to_UTF8(now));
            now.clear();
        }
        else
            now += input[i];
    }
    if(!now.empty())
        inputWords.push_back(GB2312_to_UTF8(now));
}
// 完整句子模式输入
void inputSentence() {
    if(wordSeg == nullptr) {  //没有创建分词器对象
        cout << endl;
        print_yellow("第一次打开，正在创建分词器......");
        wordSeg = new WordSegmentation;
        cout << endl;
    }
    print_yellow("请在一行中输入想要查询的句子后按下回车键");
    string input;
    getline(cin, input);  //读掉回车
    getline(cin, input);
    inputWords = wordSeg->segmentation(GB2312_to_UTF8(input));
}
//调整每页展示数量
void changeShowNum() {
    print_yellow("请输入每页展示网页的默认数量");
    string input;
    getline(cin, input);  //读掉回车
    bool showError = 0;
    while(1) {
        if(showError) {
            print_red("请重新输入！");
        }
        getline(cin, input);
        int temp = getNumber(input);
        if(temp > 0) {
            baseShowNum = temp;
            break;
        }
        else
            showError = 1;
    }
    print_yellow("设置成功！按任意键返回");
    getchar();
}

//主进程
int main() {
    set_size(150, 50);
    initWebsite();
    //主页面部分
    bool showError = 0;
    while(1) {
        system("cls");
        cout << endl;
        if(showError) {
            showError = 0;
            print_red("请重新输入！");
            cout << endl;
        }
        print_mid("************************************************");
        print_mid("*                 请选择操作                   *");
        print_mid("*                                              *");
        print_mid("*         1.搜索多个单词                       *");
        print_mid("*         2.搜索单个句子(进行分词)             *");
        print_mid("*         3.设置显示结果个数                   *");
        print_mid("*         4.退出程序                           *");
        print_mid("*                                              *");
        print_mid("************************************************");
        string option;
        cin >> option;
        if(option == "1") {
            inputDividedWord();
            startTime = clock();
            getWordsID();
            getResult();
            endTime = clock();
            showResult();
        }
        else if(option == "2") {
            inputSentence();
            startTime = clock();
            getWordsID();
            getResult();
            endTime = clock();
            showResult();
        }
        else if(option == "3") {
            changeShowNum();
        }
        else if(option == "4") {
            break;
        }
        else
            showError = 1;
    }

    return 0;
}
