
#ifndef MY_TRIE_H
#define MY_TRIE_H

#include <string>
using namespace std;

/*
    实现兼容中文的字典树
    其中中文以 UTF-8 编码储存，编码特性为最多由4个字节进行表示，那么则直接将每一个字节视为一个无符号8位值，进行建树

    * UTF-8参考资料：https://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html
 */

#define ONE_BYTE 256
// 字典树节点定义
struct TrieNode {
    bool end;  //是否为某个单词的结尾
    int id;  //该单词的编号
    TrieNode* son[ONE_BYTE];
    TrieNode() {
        end = 0;
        id = 0;
        for(int i = 0; i < ONE_BYTE; i++)
            son[i] = nullptr;
    }
};
// Trie树类定义
class Trie {
public:
    Trie() {
        root = new TrieNode();
    }
    //将字符串插入到Trie树中
    void insert(const string& x) {
        TrieNode* u = root;
        int v;
        for(int i = 0; i < x.length(); i++) {
            v = ((int)x[i] & (0xFF));
            if(u->son[v] == nullptr)
                u->son[v] = new TrieNode();
            u = u->son[v];
        }
        u->end = 1;
    }
    //检查字符串是否在Trie树中
    bool check(const string& x) {
        TrieNode* u = root;
        int v;
        for(int i = 0; i < x.length(); i++) {
            v = ((int)x[i] & (0xFF));
            if(u->son[v] == nullptr)
                return 0;
            u = u->son[v];
        }
        return u->end;
    }
    //返回字符串对应的值
    int query(const string& x) {
        TrieNode* u = root;
        int v;
        for(int i = 0; i < x.length(); i++) {
            v = ((int)x[i] & (0xFF));
            if(u->son[v] == nullptr)
                return -1;
            u = u->son[v];
        }
        return u->id;
    }

private:
    TrieNode* root;
};

#endif