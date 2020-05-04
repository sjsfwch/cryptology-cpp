
#include <stdio.h>
#include <vector>
#include<math.h>
#include<iostream>

#define L64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
// #define char2u64(a,b,c,d,e,f,g,h) ((u64(a)<<56)|(u64(b)<<48)|(u64(c)<<40)|(u64(d)<<32)|(u64(e)<<24)|(u64(f)<<16)|(u64(g)<<8)|u64(h))
#define char2u64(a,b,c,d,e,f,g,h) ((u64(a))|(u64(b)<<8)|(u64(c)<<16)|(u64(d)<<24)|(u64(e)<<32)|(u64(f)<<40)|(u64(g)<<48)|(u64(h)<<56))
#define getbit(x,y) (((x)>>(y))&0x1)

typedef u_int64_t u64;
typedef u_int8_t u8;

using namespace std;



class SHA3_256{
    u64 A[5][5];
    const int c=2*256,r=1600-2*256;//c=2d,r=b-c 本算法b取1600
    u8 buffer[136];//sha3-256的blocksize是r/8=136
    u8 S[200];
    const int P[5][5] = {{0, 36, 3, 105, 210},
                         {1, 300, 10, 45, 66},
                         {190, 6, 171, 15, 253},
                         {28, 55, 153, 21, 120},
                         {91, 276, 231, 136, 78}};
    u64 RC[24]={
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
    };
    public : 
    SHA3_256() {}
    ~SHA3_256(){}
    vector<u8> padding(vector<u8> &s);
    vector<u8> sponge(vector<u8> s);
    void keccak();
    void convertS2A();
    void convertA2S();
    void initRC();
    u64 rc(int t,int pos);
};

vector<u8> SHA3_256::padding(vector<u8> &s){
    int q=r/8-(s.size()%(r/8));
    if (q==1) s.push_back(0x86);
    else if(q==2) s.push_back(0x06),s.push_back(0x80);
    else{
        s.push_back(0x06);
        while(q-->2) s.push_back(0x00);
        s.push_back(0x80);
    }
    return s;
}

void SHA3_256::convertS2A(){
    int start=0;
    for(int y=0;y<5;y++){
        for(int x=0;x<5;x++){
            A[x][y]=char2u64(S[start],S[start+1],S[start+2],S[start+3],S[start+4],S[start+5],S[start+6],S[start+7]);
            start+=8;
        }
    }
}

void SHA3_256::convertA2S(){
    for(int y=0;y<5;y++){
        for(int x=0;x<5;x++){
            // S[x+5*y]=(A[x][y]>>56)&0xFF;
            // S[x+5*y+1]=(A[x][y]>>48)&0xFF;
            // S[x+5*y+2]=(A[x][y]>>40)&0xFF;
            // S[x+5*y+3]=(A[x][y]>>32)&0xFF;
            // S[x+5*y+4]=(A[x][y]>>24)&0xFF;
            // S[x+5*y+5]=(A[x][y]>>16)&0xFF;
            // S[x+5*y+6]=(A[x][y]>>8)&0xFF;
            // S[x+5*y+7]=A[x][y]&0xFF;
            S[x+5*y]=(A[x][y])&0xFF;
            S[x+5*y+1]=(A[x][y]>>8)&0xFF;
            S[x+5*y+2]=(A[x][y]>>16)&0xFF;
            S[x+5*y+3]=(A[x][y]>>24)&0xFF;
            S[x+5*y+4]=(A[x][y]>>32)&0xFF;
            S[x+5*y+5]=(A[x][y]>>40)&0xFF;
            S[x+5*y+6]=(A[x][y]>>48)&0xFF;
            S[x+5*y+7]=(A[x][y]>>56)&0xFF;
        }
    }
}

void SHA3_256::keccak(){
    u64 C[5]={0},D[5]={0},tmpA[5][5];;
    for (int r = 0; r < 24; r++) {
        // theta变换
        for (int x = 0; x < 5; x++)
            C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
        for (int x = 0; x < 5; x++)
            D[x] = C[(x + 4) % 5] ^ L64(C[(x + 1) % 5], 1);
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                A[x][y] = A[x][y] ^ D[x];
            }
        }

        // rho变换
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                A[x][y] = L64(A[x][y], P[x][y]);
            }
        }

        // pi变换

        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                tmpA[x][y] = A[(x + 3 * y) % 5][x];
            }
        }
        // chi变换
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                A[x][y] =
                    tmpA[x][y] ^ ((~tmpA[(x + 1) % 5][y]) & tmpA[(x + 2) % 5][y]);
            }
        }
        // iota变换
        A[0][0]^=RC[r];
        convertA2S();
    }
}

void SHA3_256::initRC(){
    for(int r=0;r<24;r++)
        for(int i=0;i<=6;i++){
            RC[r]|=rc(i+7*r,(1<<i)-1);
        }
}

u64 SHA3_256::rc(int t,int pos){
    if (t%255==0) return u64(0x1)<<pos;
    // 低8位分别为R0,...,R8
    u64 R=0x80,tmp=0;
    t%=255;
    for(int i=0;i<t;i++){
        // 本来就有0，不用加这样低9位变成R0,...,R8
        tmp&=0x1;
        // R0
        R^=(tmp<<8);
        // R4
        R^=(tmp<<4);
        // R5
        R^=(tmp<<3);
        // R6
        R^=(tmp<<2);
        R>>=1;
    }
    // 把R0放到对应的位置
    return ((R>>7)&0x1)<<pos;
}

vector<u8> SHA3_256::sponge(vector<u8> s){
    s=padding(s);
    int rByte=r/8,dByte=256/8,cByte=200-r/8;
    int n=s.size()/rByte;
    // 初始化S
    for(int i=0;i<200;i++) S[i]=0;
    u8 tmp[200]={0};
    int start=0;
    // absorbing
    for(int i=0;i<n;i++){
        for(int j=0;j<rByte;j++){
            // s[0]放在tmp[0]
            tmp[j]=s[j+start];
            S[j]^=tmp[j];
        }
        start+=rByte;
        for(int j=rByte;j<200;j++) S[j]^=tmp[j];
        convertS2A();
        keccak();
    }
    // squeezing
    for(int i=0;i<rByte;i++) buffer[i]=S[i];
    for(int i=0;i<256/8;i++) printf("%x",S[i]);
    return s;
}

int main(){
    vector<u8> s(5,49);

    SHA3_256 sha3;
    clock_t start = clock(), end;
    double duration;
    sha3.sponge(s);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 8 / duration << "Mbps\n";
    return 0;
}