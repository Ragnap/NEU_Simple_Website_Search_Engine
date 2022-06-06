#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
const char* const TEMP_INDEX_PATH = "./simple_website_search_engine/tempIndex/tempIndex";  //临时索引文件夹路径
const char* const EXTERIOR_INDEX_PATH = "./simple_website_search_engine/exteriorIndex.txt";  //倒序索引文件路径

#define MAX_SIZE 100000
#define TREE_SIZE 100  //叶子节点数量

//记时功能，便于效率分析
clock_t startTime, endTime;
void checkTime(bool print) {
    endTime = clock();
    if(print)
        cerr << " -> stage time use:" << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
    startTime = clock();
}

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
};
// 倒序索引生成器类
class exteriorSort {
public:
    // 对所有的单个临时索引使用基数排序
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
            radixSort(len);
            // sort(data, data + len, cmp);

            // 输出文件部分
            tempInedxFile.open(TEMP_INDEX_PATH + to_string(fileID) + "_sorted.txt", ios::out);
            for(int i = 0; i < len; i++) {
                tempInedxFile << data[i].first << '\t' << data[i].second << endl;
            }
            tempInedxFile.close();
            fileID++;
        }
    }
    // 对所有文件进行败者树优化的k路外部合并排序
    void mergeFilesSort() {
        fileSize = 1;
        ofstream exteriorIndexFile(EXTERIOR_INDEX_PATH);
        initTree();
        int minData = treeLess[1];
        int lastWordID = -1;  //上一个单词的编号
        while(data[minData].first != INT_MAX) {
            cerr << "choose file: " << minData << "  " << data[minData].first << ":" << data[minData].second << endl;
            if(data[minData].first != lastWordID) {  //新的单词
                exteriorIndexFile << endl << data[minData].first << "\t\t";
                lastWordID = data[minData].first;
            }
            exteriorIndexFile << data[minData].second << " ";
            updateTree(minData);
            minData = treeLess[1];
        }
    }

    // 初始化败者树
    void initTree() {
        for(int i = 0; i < fileSize; i++) {
            ifstream tempInedxFile(TEMP_INDEX_PATH + to_string(i) + "_sorted.txt");
            tempInedxFile >> data[i].first >> data[i].second;
            // lastPos[i] = tempInedxFile.tellg();
            lastPos[i] = 6;
            tempInedxFile.clear();
            tempInedxFile.close();
        }
        buildTree(1, 0, fileSize - 1);
    }
    // 构建败者树
    void buildTree(int i, int l, int r) {
        if(l == r) {
            treeLess[i] = l;
            treeMore[i] = l;
            inTree[l] = i;  //第l个文件对应树中节点下标为i
            return;
        }
        int mid = (l + r) >> 1;
        int lson = i << 1;
        int rson = i << 1 | 1;
        buildTree(lson, l, mid);
        buildTree(rson, mid + 1, r);
        if(data[treeLess[lson]] < data[treeLess[rson]])
            treeLess[i] = treeLess[lson], treeMore[i] = treeLess[rson];
        else
            treeLess[i] = treeLess[rson], treeMore[i] = treeLess[lson];
    }
    // 更新败者树节点值
    void updateTree(int fileID) {
        //更新叶子节点值
        ifstream tempInedxFile(TEMP_INDEX_PATH + to_string(fileID) + "_sorted.txt");
        tempInedxFile.seekg(lastPos[fileID], ios::beg);
        if(tempInedxFile >> data[fileID].first >> data[fileID].second) {  //成功读入
            //保存输入位置
            lastPos[fileID] = tempInedxFile.tellg();
        }
        else {  //读入失败
            data[fileID].first = data[fileID].second = INT_MAX;
        }
        tempInedxFile.clear();
        tempInedxFile.close();

        //更新树上节点值
        int now = inTree[fileID] >> 1;
        int nowLess = fileID;
        while(now) {
            //根据原来的较大数,新的较小数更新树上的每一个节点
            if(data[nowLess] < data[treeMore[now]]) {
                treeLess[now] = nowLess;
                treeMore[now] = treeMore[now];
            }
            else {
                treeLess[now] = treeMore[now];
                treeMore[now] = nowLess;
            }
            nowLess = treeLess[now];
            now >>= 1;
        }
    }

private:
    // 文件总数
    int fileSize;
    // 基数排序
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
    // 用于储存排序用的索引
    Index data[MAX_SIZE];
    Index temp[MAX_SIZE];

    // 用于外部排序时记录文件读取的位置
    // streampos lastPos[TREE_SIZE];
    int lastPos[TREE_SIZE];
    //  维护文件在树中的节点下标
    int inTree[TREE_SIZE];

    // 维护树中较大值的索引(败者)
    int treeMore[TREE_SIZE << 1];

    // 维护树中较小值的索引(胜者)
    int treeLess[TREE_SIZE << 1];
} mysort;
int main() {
    mysort.mergeFilesSort();
    system("pause");
    return 0;
}