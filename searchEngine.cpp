//
// �û�������ҳ��
//
#include "base.hpp"
using namespace std;
//��ʱ���ܣ�����Ч�ʷ���
clock_t startTime, endTime;
void checkTime(bool print) {
    endTime = clock();
    if(print)
        cerr << " -> stage time use:" << ((double)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
    startTime = clock();
}
// ����ִ���
WordSegmentation* wordSeg = nullptr;
// ���浥�ʺ͵��ʱ�ŵ�ӳ�����������û�����ı���ļ�
class WordMap {
public:
    WordMap() {
        build();
    }
    //�����ļ������ֵ���
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
    //��ȡһ�����ʵı��,�ڵ���δ����ʱ�������ֵ
    int query(string word) {
        int ID = wordTrie.query(word);
        if(ID == -1)
            return 0x7fffffff;
        else
            return ID;
    }

private:
    int wordCnt;
    Trie wordTrie;  //�����������ŵ��ֵ���
} wordDict;
// ��ҳ����
int websiteNum;
// Ĭ��չʾ����
int baseShowNum = 5;
// ������ҳ����
string websiteLink[PAGE_SIZE];
// ������ҳ����
string websiteTitle[PAGE_SIZE];
// ����ҳ���г��ֹ��ļ����ʵ�������
int websiteWordKind[PAGE_SIZE];
// �������м������ڸ���ҳ���ֵĴ���
int websiteWordTime[PAGE_SIZE];
// �����û�����ĵ���
vector<string> inputWords;
// ��ʼ����ҳ����
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
// �����ʶ�Ӧ�����,��������ļ�
void getWordsID() {
    ofstream inputWordID(INPUT_WORD_ID_PATH);
    for(int i = 0; i < inputWords.size(); i++) {
        inputWordID << wordDict.query(inputWords[i]) << endl;
    }
    inputWordID.close();
}

// ���в���
void getResult() {
    system("getSearchResult.exe");
}
// ����������
void showResult() {
    ifstream result(RESULT_PATH);
    int relativeNum;  //�йص���ҳ����
    result >> relativeNum;
    if(relativeNum == 0) {  //δ������
        print_red("û�������صĽ�����볢������������");
        print_red("�����������");
        getchar();
    }
    else {
        string header, input, temp;
        header = "������ʱ " + to_string((double)(endTime - startTime)) + " ms , ���� " + to_string(relativeNum) + " �������ҳ";
        vector<int> matchWebsite;  //��������˳�򱣴����ҳ�ı��
        int ID;  //��ǰ��ҳ���
        for(int i = 0; i < relativeNum; i++) {
            result >> ID;
            result >> websiteWordKind[ID];
            result >> websiteWordTime[ID];
            matchWebsite.push_back(ID);
        }
        int base = 0;  // ��ǰ������ʾ�ĵ�һ����ҳλ��
        int lim = baseShowNum;  //��ʾ����������
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
                temp = "���� " + to_string(websiteWordKind[ID]) + " ���ؼ��ʣ����йؼ��ʳ����� " + to_string(websiteWordTime[ID]) + " ��";
                print_mid(temp);
                cout << endl;
                print_mid("**************************************");
            }
            cout << endl;
            if(base > 0)
                print_yellow("�鿴��һҳ��������1");
            if(base < relativeNum - 1)
                print_yellow("�鿴��һҳ��������2");
            print_yellow("չʾ�������������������0");
            print_yellow("�����ϼ��˵�������������");
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
// �൥��ģʽ����
void inputDividedWord() {
    print_yellow("����һ�����������е��ʣ��Կո�ָ�");
    string input;
    getline(cin, input);  //�����س�
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
// ��������ģʽ����
void inputSentence() {
    if(wordSeg == nullptr) {  //û�д����ִ�������
        cout << endl;
        print_yellow("��һ�δ򿪣����ڴ����ִ���......");
        wordSeg = new WordSegmentation;
        cout << endl;
    }
    print_yellow("����һ����������Ҫ��ѯ�ľ��Ӻ��»س���");
    string input;
    getline(cin, input);  //�����س�
    getline(cin, input);
    inputWords = wordSeg->segmentation(GB2312_to_UTF8(input));
}
//����ÿҳչʾ����
void changeShowNum() {
    print_yellow("������ÿҳչʾ��ҳ��Ĭ������");
    string input;
    getline(cin, input);  //�����س�
    bool showError = 0;
    while(1) {
        if(showError) {
            print_red("���������룡");
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
    print_yellow("���óɹ��������������");
    getchar();
}

//������
int main() {
    set_size(150, 50);
    initWebsite();
    //��ҳ�沿��
    bool showError = 0;
    while(1) {
        system("cls");
        cout << endl;
        if(showError) {
            showError = 0;
            print_red("���������룡");
            cout << endl;
        }
        print_mid("************************************************");
        print_mid("*                 ��ѡ�����                   *");
        print_mid("*                                              *");
        print_mid("*         1.�����������                       *");
        print_mid("*         2.������������(���зִ�)             *");
        print_mid("*         3.������ʾ�������                   *");
        print_mid("*         4.�˳�����                           *");
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
