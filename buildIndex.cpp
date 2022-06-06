#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
const char* const TEMP_INDEX_PATH = "./simple_website_search_engine/tempIndex/tempIndex";  //临时索引文件夹路径
#define MAX_SIZE 100000
// 储存单个索引的数据结构
struct Index {
    int first;  // 单词编号
    int second;  // 网站编号
    //重载<运算符，便于比较，以单词编号为第一关键字，网站编号为第二关键字
    bool operator<(const Index& x) {
        if(first != x.first)  //单词编号不同
            return first < x.first;
        else
            return second < x.second;
    }
} data[MAX_SIZE];
bool cmp(Index x, Index y) {
    if(x.first != y.first)  //单词编号不同
        return x.first < y.first;
    else
        return x.second < y.second;
}
//记时功能，便于效率分析
clock_t startTime, endTime;
void checkTime(bool print) {
    endTime = clock();
    if(print)
        cerr << " -> stage time use:" << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
    startTime = clock();
}
//基数排序
Index temp[MAX_SIZE];
void radixSort(int len) {
    int wid = 1;
    //获取最长位数(桶排次数)
    for(register int i = 0; i < len; i++) {
        int cnt = 0, x = data[i].first;
        do {
            cnt++;
            x /= 10;
        } while(x != 0);
        wid = max(wid, cnt);
    }
    int pos[10] = {0};
    int radix = 1;
    while(wid--) {
        for(register int i = 0; i < 10; i++)
            pos[i] = 0;
        for(register int i = 0; i < len; i++) {
            int x = (data[i].first / radix) % 10;
            pos[x]++;
        }
        for(register int i = 1; i < 10; i++)
            pos[i] += pos[i - 1];
        for(register int i = len - 1; i >= 0; i--) {
            int x = (data[i].first / radix) % 10;
            temp[--pos[x]] = data[i];
        }
        for(register int i = 0; i < len; i++)
            data[i] = temp[i];
        radix *= 10;
    }
}
//对所有的单个临时索引使用记数排序
void signalFileSort() {
    fstream tempInedxFile;
    int fileID = 0, len = 0;
    while(1) {
        tempInedxFile.open(TEMP_INDEX_PATH + to_string(fileID) + ".txt", ios::in);
        if(tempInedxFile.fail()) {
            break;
        }
        cerr << "now sorting: " << fileID << endl;
        // 读取数据部分
        len = 0;
        while(tempInedxFile >> data[len].first >> data[len].second) {
            len++;
        }
        tempInedxFile.close();
        tempInedxFile.clear();

        // 排序部分
        // radixSort(len);
        sort(data, data + len, cmp);
        // 输出文件部分
        tempInedxFile.open(TEMP_INDEX_PATH + to_string(fileID) + "_sorted.txt", ios::out);
        for(int i = 0; i < len; i++) {
            tempInedxFile << data[i].first << '\t' << data[i].second << endl;
        }
        tempInedxFile.close();
        fileID++;
    }
}
int main() {
    checkTime(0);
    signalFileSort();
    checkTime(1);
    system("pause");
    return 0;
}