
#include<iostream>
#include<vector>
#include<string.h>
#include<bitset>
using namespace std;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;
#define S(x,i) ((x) >> (32 - i) | ((x) << i))
#define char2u32(a,b,c,d) (u32(a)<<24)|(u32(b)<<16)|(u32(c)<<8)|u32(d)

#define F1 ((B & C) | ((~B) & D))
#define F2 (B ^ C ^ D)
#define F3 ((B & C) | (B & D) | (C & D))
#define F4 (B ^ C ^ D)

class SHA1{
    const u32 A0 = 0x67452301,B0 = 0xEFCDAB89,C0 = 0x98BADCFE,D0 = 0x10325476,E0 = 0xC3D2E1F0;
    const u32 Kt[4]={0x5a827999,0x6ed9eba1,0x8f1bbcdc,0xca62c1d6};

    u32 A,B,C,D,E;
    u32 W[80];
    public:
    SHA1(){};
    ~SHA1(){};

    vector<u8> expansion(vector<u8> s);
    void resetBuffer();
    u32 Ft(u32 t);
    vector<u8> encode(vector<u8> s);
    void setWArray(vector<u8> &s,u64 start);
};

// u32 SHA1::Kt(u32 t){
//     if (t < 20)
//         return 0x5a827999;
//     else if (t < 40)
//         return 0x6ed9eba1;
//     else if (t < 60)
//         return 0x8f1bbcdc;
//     else
//         return 0xca62c1d6;
// }

vector<u8> SHA1::expansion(vector<u8> s){
    int len=s.size();
    u64 bitLen=s.size()*8;
    int appendNum=56-(len)%64;
    if (appendNum<=0)
        appendNum=64+appendNum;
    s.push_back(128);
    for(int i=0;i<appendNum-1;i++)
        s.push_back(0);
    // 原始长度加到末尾
    for(int i=7;i>=0;i--){
        s.push_back((bitLen>>(i*8))&0xFF);
    }
    for(int i=0;i<s.size();i++) cout<<u32(s[i])<<" ";
    return s;
    
};

void SHA1::resetBuffer (){
    A=A0;
    B=B0;
    C=C0;
    D=D0;
    E=E0;
}

// u32 SHA1::Ft(u32 t) {
//     if (t < 20)
//         return (B & C) | ((~B) & D);
//     else if (t < 40)
//         return B ^ C ^ D;
//     else if (t < 60)
//         return (B & C) | (B & D) | (C & D);
//     else
//         return B ^ C ^ D;
// }


void SHA1::setWArray (vector<u8> &s,u64 start){
    
    for (int i=0;i<16;i++)
        W[i]=char2u32(s[start+i*4],s[start+i*4+1],s[start+i*4+2],s[start+i*4+3]);
    // for(int i=0;i<16;i++) cout<<bitset<32>(W[i])<<endl;
    // cout<<endl;
    // memcpy(W,&s[start],64);
    // for(int i=0;i<16;i++) cout<<bitset<32>(W[i])<<endl;
    for (int i=16;i<80;i++){
        W[i]=S(W[i-3]^W[i-8]^W[i-14]^W[i-16],1);
    }
}

vector<u8> SHA1::encode(vector<u8> s ){
    // 补位
    s=expansion(s);
    int count=s.size()/64;
    // 重制缓冲区
    resetBuffer();
    u32 tmpA=A,tmpB=B,tmpC=C,tmpD=D,tmpE=E;
    u32 tmp;
    for(int i=0;i<count;i++){
        setWArray(s,64*i);
        for(int round=0;round<20;round++){
            tmp=E + F1 + S(A, 5) + W[round] + Kt[0];
            E=D;
            D=C;
            C=S(B,30);
            B=A;
            A=tmp;
        }
        for(int round=20;round<40;round++){
            tmp=E + F2 + S(A, 5) + W[round] + Kt[1];
            E=D;
            D=C;
            C=S(B,30);
            B=A;
            A=tmp;
        }
        for(int round=40;round<60;round++){
            tmp=E + F3 + S(A, 5) + W[round] + Kt[2];
            E=D;
            D=C;
            C=S(B,30);
            B=A;
            A=tmp;
        }
        for(int round=60;round<80;round++){
            tmp=E + F4 + S(A, 5) + W[round] + Kt[3];
            E=D;
            D=C;
            C=S(B,30);
            B=A;
            A=tmp;
        }
        A=A+tmpA;
        B=B+tmpB;
        C=C+tmpC;
        D=D+tmpD;
        E=E+tmpE;
        tmpA=A,tmpB=B,tmpC=C,tmpD=D,tmpE=E;
    }
    vector<u8> res;
    for(int i=3;i>=0;i--) res.push_back((A>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((B>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((C>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((D>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((E>>(i*8))&0xFF);
    
    printf("%08x%08x%08x%08x%08x\n\n", A, B, C, D, E);
    return res;
}

int main(){
    vector<u8> s(5,50);

    SHA1 sha1;
    clock_t start = clock(), end;
    double duration;
    sha1.encode(s);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 80 / duration << "Mbps\n";
    return 0;
}