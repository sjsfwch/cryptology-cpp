#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <ctime>
#include <iostream>

#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

void swap(char *c1, char *c2) {
    char c = *c1;
    *c1 = *c2;
    *c2 = c;
}

void swap(int *p1, int *p2) {
    int p = *p1;
    *p1 = *p2;
    *p2 = p;
}

class Enigma {
   public:
    Enigma();
    ~Enigma();
    void disorder(char *, int);
    void disorderReflector();
    void setPlugboard(pair<char, char>[], int);
    void initRotorReflector();
    void setRotors(char newRotors[5][27]);
    void setReflector(char reflector[]);
    void encode(char[], int, char[], int[]);
    void decode(char[], int, char[], int[]);
    bool check(char[], int, char[], int[]);
    void crack(char[],int,int[],char[]);
    void resetPlugboard();
    string genCharacterChainByKeyAndRotormap(int rotorMap[], char keys[]);
    void genCharacterChain();
    vector<int> getChainLen(char chain[]);
    string vec2string(vector<int>);
    string getCharacterStringByKeyAndRotormap(int rotorMap[], char keys[]);
    vector<string> getInfoByKeyAndRotormap(int rotorMap[],char keys[]);
   private:
    char rotors[5][27];
    int rotorReflector[5][27];
    char reflector[27];
    char plugboard[27];
    unordered_map<string,vector<string> > characterChain2Key;

};

Enigma::Enigma() {
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 5; j++) rotors[j][i] = char('A' + i);
        reflector[i] = char('A' + i);
        plugboard[i] = char('A' + i);
    }
    disorder(rotors[0], 26);
    disorder(rotors[1], 26);
    disorder(rotors[2], 26);
    disorder(rotors[3], 26);
    disorder(rotors[4], 26);
    // disorder(reflector,26);
    // disorder(plugboard,26);
    disorderReflector();
    initRotorReflector();
}

Enigma::~Enigma() {  }

void Enigma::disorder(char chars[], int n) {
    for (int i = n - 1; i > 0; i--) {
        swap(&chars[i], &chars[rand() % i]);
    }
}

void Enigma::disorderReflector() {
    int index[26];
    for (int i = 0; i < 26; i++) index[i] = i;

    for (int i = 25; i > 0; i--) {
        swap(&index[i], &index[rand() % i]);
    }
    for (int i = 0; i < 26; i += 2) {
        swap(&reflector[index[i]], &reflector[index[i + 1]]);
    }
}

void Enigma::setPlugboard(pair<char, char> newPlugboard[], int n) {
    // 先重置
    for (int i = 0; i < 26; i++) plugboard[i] = i + 'A';
    for (int i = 0; i < n; i++) {
        plugboard[newPlugboard[i].first - 'A'] = newPlugboard[i].second;
        plugboard[newPlugboard[i].second - 'A'] = newPlugboard[i].first;
    }
}

void Enigma::initRotorReflector() {
    for (int i = 0; i < 26; i++) {
        rotorReflector[0][rotors[0][i] - 'A'] = i;
        rotorReflector[1][rotors[1][i] - 'A'] = i;
        rotorReflector[2][rotors[2][i] - 'A'] = i;
        rotorReflector[3][rotors[3][i] - 'A'] = i;
        rotorReflector[4][rotors[4][i] - 'A'] = i;
    }
}

void Enigma::setRotors(char newRotors[5][27]){
    for(int i = 0; i<5;i++)
        strcpy(rotors[i],newRotors[i]);
    initRotorReflector();
}

void Enigma::setReflector(char newReflector[]){
    strcpy(this->reflector,newReflector);
}

void Enigma::resetPlugboard(){
        for (int i = 0; i < 26; i++) 
            plugboard[i] = 'A' + i;
}

void Enigma::encode(char msg[], int N, char key[], int rotorMap[]) {
    int clickCount = 0;
    for (int i = 0; i < N; i++) {
        clickCount += 1;
        char c = msg[i];
        // 插线板
        c = plugboard[c - 'A'];
        // 计算每个滚轮相对于初始位置的相对位置
        int r1 = (rotorReflector[rotorMap[0]][key[0] - 'A'] + clickCount % 26) % 26;
        int r2 = (rotorReflector[rotorMap[1]][key[1] - 'A'] + clickCount / 26 % 26) % 26;
        int r3 = (rotorReflector[rotorMap[2]][key[2] - 'A'] + clickCount / 676 % 26) % 26;
        // 经过r1,r2,r3，reflector,r3,r2,r1
        c = rotors[rotorMap[0]][(c - 'A' + r1) % 26];
        c = rotors[rotorMap[1]][(c - 'A' + r2) % 26];
        c = rotors[rotorMap[2]][(c - 'A' + r3) % 26];
        c = reflector[c - 'A'];
        c = (rotorReflector[rotorMap[2]][c - 'A'] - r3 + 26) % 26 + 'A';
        c = (rotorReflector[rotorMap[1]][c - 'A'] - r2 + 26) % 26 + 'A';
        c = (rotorReflector[rotorMap[0]][c - 'A'] - r1 + 26) % 26 + 'A';
        // 插线板
        c = plugboard[c - 'A'];
        msg[i] = c;
    }
}
void Enigma::decode(char msg[], int N, char key[], int rotorMap[]) {
    encode(msg, N, key, rotorMap);
}

bool Enigma::check(char msg[], int N, char key[], int rotorMap[]) {
    char baseline[N+1];
    strcpy(baseline, msg);
    encode(msg, N, key, rotorMap);
    decode(msg, N, key, rotorMap);
    int flag = strcmp(baseline, msg);
    return flag == 0;
}

void Enigma::genCharacterChain(){
    int rotorMap[3];
    char key[3];
    for(int i =0;i<5;i++){
        for (int j=0;j<5;j++){
            if (i==j)
                continue;
            for(int k=0;k<5;k++){
                if (i==k||j==k)
                    continue;
                rotorMap[0]=i;
                rotorMap[1]=j;
                rotorMap[2]=k;
                for (int a=0;a<26;a++){
                    key[0]=a+'A';
                    for(int b=0;b<26;b++){
                        key[1]=b+'A';
                        for (int c=0;c<26;c++){
                            key[2]=c+'A';
                            string characterChain=genCharacterChainByKeyAndRotormap(rotorMap,key);
                            if (!characterChain2Key.count(characterChain))
                                characterChain2Key[characterChain]=vector<string>();
                            string keys=key[0]+to_string(rotorMap[0])+key[1]+to_string(rotorMap[1])+key[2]+to_string(rotorMap[2]);
                            characterChain2Key[characterChain].push_back(keys);
                        }
                    }
                }
            }
        }
    }
    int count=0;
    for (auto item:characterChain2Key){
        count+=item.second.size();
    }
    cout<<"总共"<<count<<endl;
}

string Enigma::getCharacterStringByKeyAndRotormap(int rotorMap[],char key[]){
    char H0[26];
    char H1[26];
    char H2[26];
    char msg[6];
    for (int i =0;i<26;i++){
        for (int j =0;j<6;j++){
            msg[j]=i+'A';
        }
        encode(msg,6,key,rotorMap);
        H0[msg[0]-'A']=msg[3];
        H1[msg[1]-'A']=msg[4];
        H2[msg[2]-'A']=msg[5];
    }
    string res="";
    for(int i=0;i<26;i++) res+=H0[i];
    for(int i=0;i<26;i++) res+=H1[i];
    for(int i=0;i<26;i++) res+=H2[i];
    return res;
}

vector<int> Enigma::getChainLen(char chain[]){
    int flag[26];
    for (int i=0;i<26;i++)
        flag[i]=0;
    vector<int> chainLen;
    // for (int i=0;i<26;i++) cout<<char(i+'A');
    // cout<<"\n";
    // for (int i=0;i<26;i++) cout<<chain[i];
    // cout<<"\n";
    for(int i =0;i<26;i++){
        // 已被访问过，跳过
        if (flag[i])
            continue;
        int start = i,count=0;
        while(!flag[start]){
            // cout<<chain[start]-'A'<<chain[start]<<endl;
            count+=1;
            flag[start]=1;
            start=chain[start]-'A';
        }
        chainLen.push_back(count);
    }
    sort(chainLen.begin(),chainLen.end());
    return chainLen;
}

string Enigma::vec2string(vector<int> vec){
    string s="";
    for (int i:vec)
        s+=to_string(i)+',';
    return s;
}

string Enigma::genCharacterChainByKeyAndRotormap(int rotorMap[],char key[]){
    char H0[26];
    char H1[26];
    char H2[26];
    char msg[6];
    for (int i =0;i<26;i++){
        for (int j =0;j<6;j++){
            msg[j]=i+'A';
        }
        encode(msg,6,key,rotorMap);
        H0[msg[0]-'A']=msg[3];
        H1[msg[1]-'A']=msg[4];
        H2[msg[2]-'A']=msg[5];
    }
    // for (int i=0;i<26;i++)
    //     printf("%c",H0[i]);
    vector<int> cChain1=getChainLen(H0);
    vector<int> cChain2=getChainLen(H1);
    vector<int> cChain3=getChainLen(H2);
    string chainLen=vec2string(cChain1)+'|'+vec2string(cChain2)+'|'+vec2string(cChain3);
    return chainLen;
}



void Enigma::crack(char code[],int N,int rotorMap[],char key[]){
    // 假设拿到了足够多的信息，拿到了字母链
    string chainLen=genCharacterChainByKeyAndRotormap(rotorMap,key);
    // 假设拿到了足够多的信息，拿到了从AAAAAA到ZZZZZZ加密后的密文
    auto infos=getInfoByKeyAndRotormap(rotorMap,key);
    // int count=0;
    resetPlugboard();
    // 生成所有字母链并分类
    genCharacterChain();
    char cmsg[7]="AAAAAA";
    bool flag=false;
    for (string keyMap:characterChain2Key[chainLen]){
        // count+=1;
        // if (count%100==0) printf("正在搜索第%d条",count);
        flag=true;
        int rotorMapGen[3];
        char keyGen[3];
        rotorMapGen[0]=keyMap[1]-'0';
        rotorMapGen[1]=keyMap[3]-'0';
        rotorMapGen[2]=keyMap[5]-'0';
        keyGen[0]=keyMap[0];
        keyGen[1]=keyMap[2];
        keyGen[2]=keyMap[4];
        for(auto msg:infos){
            for (int i=0;i<6;i++) cmsg[i]=msg[i];
            decode(cmsg,6,keyGen,rotorMapGen);
            // 说明不是这个设置
            if(cmsg[0]!=cmsg[3]||cmsg[1]!=cmsg[4]||cmsg[2]!=cmsg[5]){
                flag=false;
                break;
            }
        }
        if (flag){
            cout<<"破解成功\n";
            cout<<"rotorMap: "<<rotorMapGen[0]<<rotorMapGen[1]<<rotorMapGen[2];
            cout<<"\nKey: "<<keyGen[0]<<keyGen[1]<<keyGen[2]<<endl;
            break;
        }
    }
}

vector<string> Enigma::getInfoByKeyAndRotormap(int rotorMap[],char keys[]){
    char msgKey[7]="AAAAAA";
    vector<string> res;
    for(int i=0;i<26;i++){
        for (int j=0;j<26;j++){
            for(int k=0;k<26;k++){
                msgKey[0]=i+'A';
                msgKey[3]=i+'A';
                msgKey[1]=j+'A';
                msgKey[4]=j+'A';
                msgKey[2]=k+'A';
                msgKey[5]=k+'A';
                encode(msgKey,6,keys,rotorMap);
                string s=msgKey;
                res.push_back(s);
            }
        }
    }
    return res;
}

int main() {
    srand(time(NULL));
    Enigma e;
    int N = 1000000, M = 100;
    char msg[N+1];
    pair<char, char> plugboard[3] = {pair<char, char>('A', 'K'),
                                     pair<char, char>('C', 'F'),
                                     pair<char, char>('B', 'G')};
    e.setPlugboard(plugboard, sizeof(plugboard) / sizeof(plugboard[0]));
    for (int i = 0; i < N; i++) msg[i] = 'A' + rand() % 26;
    char key[3] = {'A', 'B', 'C'};
    int rotorMap[3] = {0, 1, 2};
    clock_t start = clock(), end;
    double duration;
    // for (int i = 0; i < M; i++) e.encode(msg, N, key, rotorMap);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << M * 8 / duration << "Mbps\n";
    if (e.check(msg, N, key, rotorMap)) cout << "加密解密验证通过" << endl;
    char rotors[5][27]={"EKMFLGDQVZNTOWYHXUSPAIBRCJ","AJDKSIRUXBLHWTMCQGZNPYFVOE","BDFHJLCPRTXVZNYEIWGAKMUSQO","ESOVPZJAYQUIRHXLNFTGKDCMWB","VZBRGITYUPSDNHLXAWMJQOFECK"};
    char reflector[27]={"YRUHQSLDPXNGOKMIEBFZCWVJAT"};
    e.setReflector(reflector);
    e.setRotors(rotors);
    // e.genCharacterChain();
    // e.genCharacterChainByKeyAndRotormap(rotorMap,key);
    char msg1[7]="AAAAAA";
    e.encode(msg1, 6, key, rotorMap);
    // e.resetPlugboard();
    e.encode(msg1, 6, key, rotorMap);
    e.crack(msg1,6,rotorMap,key);

    return 0;
}