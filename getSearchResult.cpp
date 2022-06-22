#include "base.hpp"
using namespace std;
// ��ҳ�����ļ�
struct websiteValue {
    int ID;  //��ҳ���
    int wordKind;  //����ҳ���г��ֹ��ļ����ʵ�������
    int wordTime;  //���м������ڸ���ҳ���ֵĴ���
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
// ���������������飬ά�����ǵ��ʳ���λ��
vector<int> wordTabel[WORD_SIZE];
// ��վ����
int websiteNum;
// ��ʼ��
void initWebsite() {
    // ��ȡ��վ����
    ifstream websiteDict(WEBSITE_DICT_PATH);
    websiteDict >> websiteNum;
    websiteDict.close();
    // ��ʼ����վID
    for(int i = 1; i <= websiteNum; i++)
        website[i].ID = i;
}
// ������������
void buildWordTabel() {
    ifstream exteriorIndex(EXTERIOR_INDEX_PATH);
    string nowLine;  //��ǰ������
    int i;  //��ǰ�ַ�������λ��
    int wordID;  //��ǰ���ʱ��
    int websiteID;  //��ǰ��վ���
    while(getline(exteriorIndex, nowLine)) {
        i = 0;
        wordID = getNumber(nowLine, i);
        websiteID = getNumber(nowLine, i);
        while(websiteID != -1) {
            wordTabel[wordID].push_back(websiteID);
            websiteID = getNumber(nowLine, i);
        }
        //�ڶ�β����һ�����ֵ���ڶ��ֲ���
        wordTabel[wordID].push_back(0x7fffffff);
    }
    exteriorIndex.close();
}

// ����ÿһ����ҳ��ţ�����ÿ����ҳ��Ȩֵ��ͨ���򵥱������ʵ���ҳ�б�ʵ��
void searchWord(int wordID) {
    if(wordID < 1 || wordID >= WORD_SIZE)  //���ʱ�Ų��ڵ��ʱ���
        return;
    //���ݵ��������Ե��ʳ��ֹ�����ҳ���м�Ȩ
    for(int i = 0; i < wordTabel[wordID].size(); i++) {
        if(wordTabel[wordID][i] == 0x7fffffff)  //�����Ľ��������ֹ���˳�
            break;
        //���һ����ҳ��ͬʱ����ʾ����һ����ҳ���һ�γ���,��ʱ��������������ֻ֤����һ��
        if(wordTabel[wordID][i] != wordTabel[wordID][i + 1])
            website[wordTabel[wordID][i]].wordKind++;
        //���ִ�������
        website[wordTabel[wordID][i]].wordTime++;
    }
}
// �鲢���� [l,r) ����
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
// ��������ҳ��������
void sortWebsites() {
    mergeSort(1, websiteNum + 1);
}
// ����������վ���
void saveToFile() {
    int totalHaveNum = 0;  //���ٺ��йؼ��ֵ�����
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
// �����и�����ҳ��Ȩ�ؽ�������
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