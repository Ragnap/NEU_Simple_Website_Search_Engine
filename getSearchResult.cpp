#include "base.hpp"
using namespace std;
// 网页排序文件
struct websiteValue {
    int ID;  //网页编号
    int wordKind;  //该网页中有出现过的检索词的种类数
    int wordTime;  //所有检索词在该网页出现的次数
    bool operator>(const websiteValue& a) {
        if(wordKind != a.wordKind)
            return wordKind > a.wordKind;
        else if(wordTime != a.wordTime)
            return wordTime > a.wordTime;
        else
            return ID > a.ID;
    }
    bool operator<(const websiteValue& a) {
        if(wordKind != a.wordKind)
            return wordKind < a.wordKind;
        else if(wordTime != a.wordTime)
            return wordTime < a.wordTime;
        else
            return ID < a.ID;
    }
} website[PAGE_SIZE], temp[PAGE_SIZE];
// 倒序索引检索数组，维护的是单词出现位置
vector<int> wordTabel[WORD_SIZE];
// 网站总数
int websiteNum;
// 初始化
void initWebsite() {
    // 获取网站数量
    ifstream websiteDict(WEBSITE_DICT_PATH);
    websiteDict >> websiteNum;
    websiteDict.close();
    // 初始化网站ID
    for(int i = 1; i <= websiteNum; i++)
        website[i].ID = i;
}
// 构建检索数组
void buildWordTabel() {
    ifstream exteriorIndex(EXTERIOR_INDEX_PATH);
    string nowLine;  //当前读入行
    int i;  //当前字符串处理位置
    int wordID;  //当前单词编号
    int websiteID;  //当前网站编号
    while(getline(exteriorIndex, nowLine)) {
        i = 0;
        wordID = getNumber(nowLine, i);
        websiteID = getNumber(nowLine, i);
        while(websiteID != -1) {
            wordTabel[wordID].push_back(websiteID);
            websiteID = getNumber(nowLine, i);
        }
        //在队尾插入一个最大值便于二分查找
        wordTabel[wordID].push_back(0x7fffffff);
    }
    exteriorIndex.close();
}
// 遍历每一个网页编号，更新每个网页的权值，利用二分查找确定它含多少个带检索单词
void searchWord(int wordID) {
    if(wordID < 1 || wordID >= WORD_SIZE)  //单词编号不在单词表里
        return;
    int l, r, mid;  //二分查找变量
    int end = wordTabel[wordID].size() + 1;  //二分的最大右界，也表示不存在
    int upper;  //上界（最后一个等于待查找值的下标）
    int lower;  //下界（第一个等于待查找值的下标）
    for(int websiteID = 1; websiteID <= websiteNum; websiteID++) {
        // 找到下界，第一个等于 websiteID 的网页编号
        l = 0, r = end;
        while(l < r) {
            mid = (l + r) >> 1;
            if(wordTabel[wordID][mid] >= websiteID)
                r = mid;
            else
                l = mid + 1;
        }
        lower = l;
        // 找到上界，第一个大于 websiteID 的网页编号
        l = 0, r = end;
        while(l < r) {
            mid = (l + r) >> 1;
            if(wordTabel[wordID][mid] > websiteID)
                r = mid;
            else
                l = mid + 1;
        }
        upper = l;
        // [lower,upper) 这个左闭右开区间内的值全部等于websiteID
        if(upper <= lower) {  //区间长度为0，未找到
            // cerr << "no wordID:" << websiteID << endl;
            continue;
        }
        else {
            // cerr << websiteID << ":" << lower << "~" << upper << endl;
            website[websiteID].wordKind++;  //增加种类
            website[websiteID].wordTime += upper - lower;  //区间长度就是出现次数
        }
    }
}
// 归并排序 [l,r) 区间
void mergeSort(int l, int r) {
    if(r - l <= 1)
        return;
    int mid = l + ((r - l) >> 1);
    mergeSort(l, mid);
    mergeSort(mid, r);
    int p = l, q = mid, s = l;
    while(s < r) {
        if(p >= mid || (q < r && website[p] < website[q]))
            temp[s++] = website[q++];
        else
            temp[s++] = website[p++];
    }
    for(int i = l; i < r; i++)
        website[i] = temp[i];
}
// 对所有网页进行排序
void sortWebsites() {
    mergeSort(1, websiteNum + 1);
}
// 将排序后的网站输出
void saveToFile() {
    int totalHaveNum = 0;  //至少含有关键字的数量
    for(int i = 1; i <= websiteNum; i++) {
        if(website[i].wordKind == 0) {
            break;
        }
        totalHaveNum++;
    }
    ofstream resultOutput(RESULT_PATH);
    resultOutput << totalHaveNum << endl;
    for(int i = 1; i <= totalHaveNum; i++) {
        resultOutput << website[i].ID << " " << website[i].wordKind << " " << website[i].wordTime << endl;
    }
    resultOutput.close();
}
// 对所有根据网页的权重进行排序
int main() {
    initWebsite();
    buildWordTabel();
    ifstream dataIn(INPUT_WORD_ID_PATH);
    int wordID = 0;
    while(dataIn >> wordID) {
        searchWord(wordID);
    }
    dataIn.close();
    sortWebsites();
    saveToFile();
    return 0;
}