#include<iostream>
#include<vector>

using namespace std;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

#define L(x,i) ((x) >> (32 - i) | ((x) << i))
#define FF1(x,y,z) (x^y^z)
#define FF2(x,y,z) ((x&y)|(x&z)|(y&z))
#define GG1(x,y,z) (x^y^z)
#define GG2(x,y,z) ((x&y)|((~(x))&z))
#define P0(x) (x^L(x,9)^L(x,17))
#define P1(x) (x^L(x,15)^L(x,23))
#define char2u32(a,b,c,d) (u32(a)<<24)|(u32(b)<<16)|(u32(c)<<8)|u32(d)

class SM3{
    const u32 T1=0x79cc4519,T2=0x7a879d87;
    const u32 A0=0x7380166f,B0=0x4914b2b9,C0=0x172442d7,D0=0xda8a0600,E0=0xa96f30bc,F0=0x163138aa,G0=0xe38dee4d,H0=0xb0fb0e4e;
    u32 W[68],W1[64];
    public:
    SM3(){}
    ~SM3(){}
    vector<u8> expansion(vector<u8> s);
    void setWArray(vector<u8> &s,u64 start);
    vector<u8> encode(vector<u8> s);
};

vector<u8> SM3::expansion(vector<u8> s){
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
    // for(int i=0;i<s.size();i++) cout<<u32(s[i])<<" ";
    return s;
    
};

void SM3::setWArray (vector<u8> &s,u64 start){
    
    for (int i=0;i<16;i++)
        W[i]=char2u32(s[start+i*4],s[start+i*4+1],s[start+i*4+2],s[start+i*4+3]);
    // for(int i=0;i<16;i++) cout<<bitset<32>(W[i])<<endl;
    // cout<<endl;
    // memcpy(W,&s[start],64);
    // for(int i=0;i<16;i++) cout<<bitset<32>(W[i])<<endl;
    for (int i=16;i<68;i++){
        W[i]=P1(W[i-16]^W[i-9]^L(W[i-3],15))^L(W[i-13],7)^W[i-6];
    }
    for(int i=0;i<64;i++) W1[i]=W[i]^W[i+4];
}

vector<u8> SM3::encode(vector<u8> s){
    // set buffer
    u32 A=A0,B=B0,C=C0,D=D0,E=E0,F=F0,G=G0,H=H0;
    u32 A1=A0,B1=B0,C1=C0,D1=D0,E1=E0,F1=F0,G1=G0,H1=H0;

    // expansion
    s=expansion(s);
    int count=s.size()/64;
    u32 SS1,SS2,TT1,TT2;
    for(int i=0;i<count;i++){
        setWArray(s, 64 * i);
        A1=A,B1=B,C1=C,D1=D,E1=E,F1=F,G1=G,H1=H;
        for(int round=0;round<16;round++){
            SS1=L(L(A,12)+E+L(T1,round),7);
            SS2=SS1^L(A,12);
            TT1=FF1(A,B,C)+D+SS2+W1[round];
            TT2=GG1(E,F,G)+H+SS1+W[round];
            D=C;
            C=L(B,9);
            B=A;
            A=TT1;
            H=G;
            G=L(F,19);
            F=E;
            E=P0(TT2);
        }
        for(int round=16;round<64;round++){
            SS1=L(L(A,12)+E+L(T2,round%32),7);
            SS2=SS1^L(A,12);
            TT1=FF2(A,B,C)+D+SS2+W1[round];
            TT2=GG2(E,F,G)+H+SS1+W[round];
            D=C;
            C=L(B,9);
            B=A;
            A=TT1;
            H=G;
            G=L(F,19);
            F=E;
            E=P0(TT2);
        }
        A^=A1;
        B^=B1;
        C^=C1;
        D^=D1;
        E^=E1;
        F^=F1;
        G^=G1;
        H^=H1;
    }
    printf("%08x%08x%08x%08x%08x%08x%08x%08x\n\n", A, B, C, D, E,F,G,H);
    vector<u8> res;
    for(int i=3;i>=0;i--) res.push_back((A>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((B>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((C>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((D>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((E>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((F>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((G>>(i*8))&0xFF);
    for(int i=3;i>=0;i--) res.push_back((H>>(i*8))&0xFF);
    return res;
}

int main(){
    vector<u8> s(10000000,49);
    SM3 sm3;
    clock_t start = clock(), end;
    double duration;
    sm3.encode(s);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 80 / duration << "Mbps\n";
    return 0;
}