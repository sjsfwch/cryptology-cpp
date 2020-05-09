#include<iostream>
#include<vector>
#include<string.h>
#include<bitset>
using namespace std;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

#define char2u32(a,b,c,d) (u32(a)<<24)|(u32(b)<<16)|(u32(c)<<8)|u32(d)
#define R(x,i) (((x) << (32 - i)) | ((x) >> i))
#define SIGMA0(A) (R(A,2)^R(A,13)^R(A,22))
#define SIGMA1(A) (R(A,6)^R(A,11)^R(A,25))

#define Ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define Ma(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define sigma0(A) (R(A,7)^R(A,18)^(A>>3))
#define sigma1(A) (R(A,17)^R(A,19)^(A>>10))

class SHA256{
    const u32 A0 = 0x6A09E667, B0 = 0xBB67AE85, C0 = 0x3C6EF372, D0 = 0xA54FF53A,
              E0 = 0x510E527F, F0 = 0x9B05688C, G0 = 0x1F83D9AB, H0 = 0x5BE0CD19;
    const u32 Kt[64]={
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    u32 W[64];
    public:
    SHA256(){}
    ~SHA256(){}
    vector<u8> expansion(vector<u8> s);
    void setWArray(vector<u8> &s,u64 start);
    vector<u8> encode(vector<u8> &s);
};

void SHA256::setWArray (vector<u8> &s,u64 start){
    
    for (int i=0;i<16;i++)
        W[i]=char2u32(s[start+i*4],s[start+i*4+1],s[start+i*4+2],s[start+i*4+3]);

    for (int i=16;i<64;i++){
        W[i]=sigma1(W[i-2])+W[i-7]+sigma0(W[i-15])+W[i-16];
    }
}

vector<u8> SHA256::expansion(vector<u8> s){
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

vector<u8> SHA256::encode(vector<u8> &s){
    s=expansion(s);
    u32 A=A0,B=B0,C=C0,D=D0,E=E0,F=F0,G=G0,H=H0;
    u32 A1=A0,B1=B0,C1=C0,D1=D0,E1=E0,F1=F0,G1=G0,H1=H0;
    u32 T1,T2;
    int count=s.size()/64;
    for (int i = 0; i < count; i++) {
        A1=A,B1=B,C1=C,D1=D,E1=E,F1=F,G1=G,H1=H;
        setWArray(s, 64 * i);
        for (int t = 0; t < 64; t++) {
            T1 = H + SIGMA1(E) + Ch(E, F, G) + Kt[t] + W[t];
            T2 = SIGMA0(A) + Ma(A, B, C);
            H = G;
            G = F;
            F = E;
            E = D + T1;
            D = C;
            C = B;
            B = A;
            A = T1 + T2;
        }
        A=A1+A;
        B=B1+B;
        C=C1+C;
        D=D1+D;
        E=E1+E;
        F=F1+F;
        G=G1+G;
        H=H1+H;
        
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
    SHA256 sha2;
    clock_t start = clock(), end;
    double duration;
    sha2.encode(s);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 80 / duration << "Mbps\n";
    return 0;
}