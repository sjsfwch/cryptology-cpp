#include<stdio.h>
#include<random>
#include<assert.h>
#include<bitset>
#include<iostream>
#include <ctime>


using namespace std;


#define M 1000000
#define getbit(x,y) (((x)>>(y))&0x1)

#define expansion(x) getbit(x, 31) | ((x & 31) << 1) | \
    (((x >> 3) & 31) << 6) | (((x >> 7) & 31) << 12) | \
    (((x >> 11) & 31) << 18) | (((x >> 15) & 31) << 24) | \
    (((x >> 19) & 31) << 30) | (((x >> 23) & 31) << 36) | \
    (((x >> 27) & 31) << 42) | (getbit(x, 0) << 47)

typedef uint64_t u64;




template <typename T>
void swap(T *c1, T *c2) {
    T c = *c1;
    *c1 = *c2;
    *c2 = c;
}
u64 getRandom64bit(){
    u64 res =u64(rand())<<32|u64(rand());
    return res;
}

template <typename T>
int getBit(T data, int i){
    if (i<0 ||i>sizeof(data)*8-1)
        return data;
    return (data>>i)&0x01;
}

template <typename T>
T circularLeftShift(T data,int i,int N){
    //进行总数据量为N的i位循环左移
    T cut=0b0;
    for (int i =0;i<N;i++){
        cut<<=1;
        cut|=0b1;
    }
    //注意不参与移位的高位要置0
    return ((data >> (N - i)) | (data << i))&cut;
}

template <typename T>
T bitPermutation(T data,int box[],int boxSize){
    //对data进行bit级别的混淆
    T res=0;
    for (int i=boxSize-1;i>=0;i--){
        res<<=1;
        if (getbit(data,box[i]))
            res|=1;
    }
    return res;
}

class DES {
    
    int inversePermutationBox[64], permutationBox[64]={
            58,50,42,34,26,18,10,2,
            60,52,44,36,28,20,12,4,
            62,54,46,38,30,22,14,6,
            64,56,48,40,32,24,16,8,
            57,49,41,33,25,17,9,1,
            59,51,43,35,27,19,11,3,
            61,53,45,37,29,21,13,5,
            63,55,47,39,31,23,15,7,
        };
    int sboxes[8][64] = {
        {14, 4,  13, 1, 2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0, 7,
         0,  15, 7,  4, 14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3, 8,
         4,  1,  14, 8, 13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5, 0,
         15, 12, 8,  2, 4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6, 13},
        {15, 1,  8,  14, 6,  11, 3,  4,  9,  7, 2,  13, 12, 0, 5,  10,
         3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9, 11, 5,
         0,  14, 7,  11, 10, 4,  13, 1,  5,  8, 12, 6,  9,  3, 2,  15,
         13, 8,  10, 1,  3,  15, 4,  2,  11, 6, 7,  12, 0,  5, 14, 9},
        {10, 0,  9,  14, 6, 3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,
         13, 7,  0,  9,  3, 4,  6,  10, 2,  8,  5,  14, 12, 11, 15, 1,
         13, 6,  4,  9,  8, 15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,
         1,  10, 13, 0,  6, 9,  8,  7,  4,  15, 14, 3,  11, 5,  2,  12},
        {7,  13, 14, 3, 0,  6,  9,  10, 1,  2, 8, 5,  11, 12, 4,  15,
         13, 8,  11, 5, 6,  15, 0,  3,  4,  7, 2, 12, 1,  10, 14, 9,
         10, 6,  9,  0, 12, 11, 7,  13, 15, 1, 3, 14, 5,  2,  8,  4,
         3,  15, 0,  6, 10, 1,  13, 8,  9,  4, 5, 11, 12, 7,  2,  14},
        {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0, 14, 9,
         14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9, 8,  6,
         4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3, 0,  14,
         11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
        {12, 1,  10, 15, 9, 2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11,
         10, 15, 4,  2,  7, 12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8,
         9,  14, 15, 5,  2, 8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,
         4,  3,  2,  12, 9, 5,  15, 10, 11, 14, 1,  7,  6,  0,  8,  13},
        {4,  11, 2,  14, 15, 0, 8,  13, 3,  12, 9, 7,  5,  10, 6, 1,
         13, 0,  11, 7,  4,  9, 1,  10, 14, 3,  5, 12, 2,  15, 8, 6,
         1,  4,  11, 13, 12, 3, 7,  14, 10, 15, 6, 8,  0,  5,  9, 2,
         6,  11, 13, 8,  1,  4, 10, 7,  9,  5,  0, 15, 14, 2,  3, 12},
        {13, 2,  8,  4, 6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,
         1,  15, 13, 8, 10, 3,  7,  4,  12, 5,  6,  11, 0,  14, 9,  2,
         7,  11, 4,  1, 9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,
         2,  1,  14, 7, 4,  10, 8,  13, 15, 12, 9,  0,  3,  5,  6,  11},
    };
    int expansionBox[48] = {
        32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9,  10, 11,
        12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
        22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};
    int permutationFuncBox[32] = {
        16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
        2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25};
    int shiftBox[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
    int pc1[56] = {57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
                   10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
                   63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
                   14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};
    int pc2[48] = {14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
                   23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
                   41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
                   44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
    u64 keys[16]={0};
public:
    void setInversePermutationBoxes();
    u64 initPermutation(u64 data);
    u64 inversePermutation(u64 data);
    DES();
    ~DES();
    void setBoxDataWithZeroStart();
    u64* genKey(u64 initKey);
    u64 F(u64 key,u64 halfData);
    int getSboxIndex(u64 B);
    u64 encryption(u64 data,int encryption);
    u64 decryption(u64 code);
    u64* cbcEncode(u64 data[],int num,u64 initKey,u64 IV);
    u64* cbcDecode(u64 data[],int num,u64 initKey,u64 IV);
};

DES::DES(){
    setBoxDataWithZeroStart();
    setInversePermutationBoxes();
}

DES::~DES(){}

void DES::setBoxDataWithZeroStart(){
    //让各种变换box从0开始
    for (int i=0;i<64;i++) permutationBox[i]--;
    for (int i=0;i<48;i++) expansionBox[i]--;
    for (int i=0;i<32;i++) permutationFuncBox[i]--;
    for (int i=0;i<56;i++) pc1[i]--;
    for (int i=0;i<48;i++) pc2[i]--;
}

void DES::setInversePermutationBoxes (){
    for(int i=0;i<64;i++){
        inversePermutationBox[permutationBox[i]]=i;
    }
}

u64 DES::initPermutation(u64 data){
    return bitPermutation(data,permutationBox,64);
}

u64 DES::inversePermutation(u64 data){
    return bitPermutation(data,inversePermutationBox,64);
}


u64* DES::genKey(u64 initKey){
    //pc1
    u64 pc1Res=bitPermutation(initKey,pc1,56);
    u64 Ri=pc1Res>>28,Li=(pc1Res<<36)>>36;//R为高位，L为低位
    
    for(int i=0;i<16;i++){
        //循环左移
        Li=circularLeftShift(Li,shiftBox[i],28);
        Ri=circularLeftShift(Ri,shiftBox[i],28);
        //合并
        keys[i]=Ri<<28|Li;
        //pc2
        keys[i]=bitPermutation(keys[i],pc2,48);
    }
    return keys;
}

u64 DES::F(u64 key,u64 halfData){
    //Expansion
    u64 exHalfData=0;
    // exHalfData=bitPermutation(halfData,expansionBox,48);
    exHalfData = expansion(halfData);
    //异或上key
    exHalfData^=key;
    //经过Sbox变换
    int index1,index2,index3,index4,index5,index6,index7,index8;
    u64 tmpRes=0,res=0;
    for(int i=0;i<4;i+=4){
        tmpRes<<=8;

        index1 = (getbit(exHalfData,i * 6 + 5)<<5) | (getbit(exHalfData,i * 6)<<4) |
                    (getbit(exHalfData,i * 6 + 4)<<3) | (getbit(exHalfData,i * 6 + 3)<<2) |
                    (getbit(exHalfData,i * 6 + 2)<<1) | (getbit(exHalfData,i * 6 + 1));
        
        index2 = (getbit(exHalfData,(i+1) * 6 + 5)<<5) | (getbit(exHalfData,(i+1) * 6)<<4) |
                    (getbit(exHalfData,(i+1) * 6 + 4)<<3) | (getbit(exHalfData,(i+1) * 6 + 3)<<2) |
                    (getbit(exHalfData,(i+1) * 6 + 2)<<1) | (getbit(exHalfData,(i+1) * 6 + 1));
        
        tmpRes|=(sboxes[i+1][index2]<<4)|sboxes[i][index1];
    }
    // index1 = (getbit(exHalfData,5)<<5) | ((exHalfData&1)<<4) | ((exHalfData>>1)&0xF);
    // index2 = (getbit(exHalfData>>6,5)<<5) | (((exHalfData>>6)&1)<<4) | ((exHalfData>>7)&0xF);
    // index3 = (getbit(exHalfData>>12,5)<<5) | (((exHalfData>>12)&1)<<4) | ((exHalfData>>13)&0xF);
    // index4 = (getbit(exHalfData>>18,5)<<5) | (((exHalfData>>18)&1)<<4) | ((exHalfData>>19)&0xF);
    // index5 = (getbit(exHalfData>>24,5)<<5) | (((exHalfData>>24)&1)<<4) | ((exHalfData>>25)&0xF);
    // index6 = (getbit(exHalfData>>30,5)<<5) | (((exHalfData>>30)&1)<<4) | ((exHalfData>>31)&0xF);
    // index7 = (getbit(exHalfData>>36,5)<<5) | (((exHalfData>>36)&1)<<4) | ((exHalfData>>37)&0xF);
    // index8 = (getbit(exHalfData>>42,5)<<5) | (((exHalfData>>42)&1)<<4) | ((exHalfData>>43)&0xF);
    tmpRes |= (sboxes[0][index1]) | (sboxes[1][index2]<<4);/* |
              (sboxes[2][index3] << 8) | (sboxes[3][index4] << 12) |
              (sboxes[4][index5] << 16) | (sboxes[5][index6] << 20) |
              (sboxes[6][index7] << 24) | (sboxes[7][index8] << 28)*/
    // permutation
    res=bitPermutation(tmpRes,permutationFuncBox,32);
    return res;
}

u64 DES::encryption(u64 data,int encryption=0){
    //initPermutation
    data=initPermutation(data);
    u64 Li=(data<<32)>>32,Ri=data>>32;//R为高位，L为低位
    u64 lastLi=Li,lastRi=Ri;
    //16轮轮函数
    // if (encryption==0)
        for(int i=0;i<16;i++){
            Li=lastRi;
            Ri=lastLi^F(keys[i],lastRi);
            lastLi=Li;
            lastRi=Ri;

        }
    // else
    //     for(int i=15;i>=0;i--){
    //         Li=lastRi;
    //         Ri=lastLi^F(keys[i],lastRi);
    //         lastLi=Li;
    //         lastRi=Ri;

    //     }
    //结果为L高位，R低位
    u64 res=(Li<<32)|Ri;
    //inversePermutation
    res=inversePermutation(res);
    return res;
}

u64 DES::decryption(u64 data){
    //initPermutation
    data=initPermutation(data);
    u64 Li=(data<<32)>>32,Ri=data>>32;//L为低位，R为高位
    u64 lastLi=Li,lastRi=Ri;
    //16轮轮函数

    for(int i=15;i>=0;i--){
        Li=lastRi;
        Ri=lastLi^F(keys[i],lastRi);
        lastLi=Li;
        lastRi=Ri;
    }

    //结果为L高位，R低位
    u64 res=(Li<<32)|Ri;
    //inversePermutation
    res=inversePermutation(res);
    return res;
}

u64* DES::cbcEncode(u64 data[],int num,u64 initKey,u64 IV){
    u64 Ci=IV;
    for(int i=0;i<num;i++){
        Ci=encryption(data[i]^Ci);
        data[i]=Ci;
    }
    return data;
}

u64* DES::cbcDecode(u64 data[],int num,u64 initKey,u64 IV){
    u64 Ci=IV,lastCi=IV;
    for(int i=0;i<num;i++){
        lastCi=Ci;
        Ci=data[i];
        data[i]=decryption(Ci)^lastCi;
    }
    return data;
}


int main(){
    DES des;
    u64 IV=getRandom64bit(),initKey=getRandom64bit();
    des.genKey(initKey);
    assert(IV==des.decryption(des.encryption(IV)));

    u64 *msg=new u64[M],*answer=new u64[M];
    for (int i=0;i<M;i++) msg[i]=getRandom64bit();
    for (int i=0;i<M;i++) answer[i]=msg[i];

    clock_t start = clock(), end;
    
    double duration;
    des.cbcEncode(msg,M,initKey,IV);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 64 / duration << "Mbps\n";
    des.cbcDecode(msg,M,initKey,IV);
    for(int i=0;i<M;i++) assert(msg[i]==answer[i]);
    printf("正确性测试通过");
    delete []msg;
    delete []answer;
    return 0;
}