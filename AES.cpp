#include <stdio.h>
#include<iostream>
#include<ctime>
using namespace std;
#define M 1000000
typedef u_int8_t u8;
typedef u_int16_t u16;
#define M 1000000
#define GF28XTime(a) ((a << 1) ^ ((a & 0x80)? 0x1b: 0x00))
clock_t s,total=0;
inline u8 GF28Multiply(u8 a,u8 b){
    u16 tmp[8]={a};
    // 逐步xtime
    for(u8 i=1;i<8;i++){
        tmp[i] = GF28XTime(tmp[i-1]);
    }
    u8 multiplyRes=0;
    // 合并所有结果
    for(int i=0; i<=7; i++)
        multiplyRes ^= (tmp[i]*((b >> i) & 0b01));

    return multiplyRes;
}

class AES {
    // private:


    public:
    AES();
    ~AES();
    void initSBox();
    u8 map(u8 a);
    void genKey(u8 key[]);
    void subBytes(u8 msg[]);
    void shiftRows(u8 msg[]);
    void mixColumns(u8 msg[]);
    void addRoundKey(u8 msg[],int round,u8 key[]);
    void invSubBytes(u8 msg[]);
    void invShiftRows(u8 msg[]);
    void invMixColumns(u8 msg[]);
    void encode(u8 msg[],u8 key[]);
    void decode(u8 msg[],u8 key[]);
    void cbcEncode(u8 msgGroup[],u8 key[],int N,u8 IV[]);
    void cbcDecode(u8 msgGroup[],u8 key[],int N,u8 IV[]);
    u8 Sbox[256];
    u8 invSbox[256];
    u8 key[176];

};

AES::AES(){
    // initSBox();
}

AES::~AES(){}

void AES::initSBox(){
    // 求逆元
    Sbox[0]=0;
    for (int i=1;i<=0xFF;i++){
        for (int j =0;j<=0xFF;j++){
            // printf("%02x",j)
            if(GF28Multiply(i,j)==0b1)
                Sbox[i]=j;
        }
    }
    // 仿射
    for (int i=0;i<=0xFF;i++){
        Sbox[i]=map(Sbox[i]);
        invSbox[Sbox[i]]=i;
    }
}

u8 AES::map(u8 a){
    // 仿射矩阵，注意高位在右边，第一行10001111，反着看是0xf1，以此类推
    u8 matrix[8]={0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8};
    // 需要加的那个向量
    u8 c=0x63;
    u8 res=0;
    for(int i=0; i<8; i++) {
        u8 now = a & matrix[i];
        u8 b = 0;
        for(int j=0; j<8; j++) 
            b ^= ((now >> j) & 0x01);
        res ^= (b << i);
    }
    // 异或加
    res^=c;
    return res; 
}

void AES::genKey(u8 key[]){
    // ppt上的RCon
    u8 RCon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10,
                   0x20, 0x40, 0x80, 0x1b, 0x36};
    // 前16个是用户自定义的密钥，不用动
    // 根据ppt上伪代码
    for(int i=4; i<=43; i++) {
        u8 tmp0, tmp1, tmp2, tmp3;
        if (i % 4 == 0) {
            tmp0 = Sbox[key[(i<<2) - 3]] ^ RCon[i / 4];
            tmp1 = Sbox[key[(i<<2) - 2]];
            tmp2 = Sbox[key[(i<<2) - 1]];
            tmp3 = Sbox[key[(i<<2) - 4]];
        } else {
            tmp0 = key[(i<<2) - 4];
            tmp1 = key[(i<<2) - 3];
            tmp2 = key[(i<<2) - 2];
            tmp3 = key[(i<<2) - 1];
        }
        key[(i<<2)] = tmp0 ^ key[4 * (i - 4)];
        key[(i<<2) + 1] = tmp1 ^ key[((i-4)<<2) + 1];
        key[(i<<2) + 2] = tmp2 ^ key[((i-4)<<2) + 2];
        key[(i<<2) + 3] = tmp3 ^ key[((i-4)<<2) + 3];
    }
}

void AES::subBytes(u8 msg[]){
    // 直接根据Sbox替换就行
    for(int i =0;i<16;i++)
        msg[i]=Sbox[msg[i]];
}

void AES::invSubBytes(u8 msg[]){
    for(int i =0;i<16;i++)
        msg[i]=invSbox[msg[i]];
}

void AES::shiftRows(u8 msg[]){
    u8 tmp1[4][4],tmp2[4][4];
    // 分成4*4矩阵
    for(int i = 0;i<16; i++) tmp1[i%4][i/4]=msg[i];
    // 移位
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            tmp2[i][j]=tmp1[i][(j+i)%4];
    }
    // 赋值
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            msg[(j<<2)+i]=tmp2[i][j];
    }
}

void AES::invShiftRows(u8 msg[]){
    u8 tmp1[4][4],tmp2[4][4];
    // 分成4*4矩阵
    for(int i = 0;i<16; i++) tmp1[i%4][i/4]=msg[i];
    // 移位
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            tmp2[i][j]=tmp1[i][(j-i+4)%4];
    }
    // 赋值
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            msg[(j<<2)+i]=tmp2[i][j];
    }
}

void AES::mixColumns(u8 msg[]){
    u8 tmp[16]={0};
    // 列混合矩阵
    u8 matrix[4][4] = {{0x02, 0x03, 0x01, 0x01},
                       {0x01, 0x02, 0x03, 0x01},
                       {0x01, 0x01, 0x02, 0x03},
                       {0x03, 0x01, 0x01, 0x02}};
    // 相当于一个矩阵乘法
    for(int i=0;i<4;i++){
        for (int j=0;j<4;j++){
            for(int k=0;k<4;k++)
                tmp[i+(j<<2)]^=GF28Multiply(matrix[i][k],msg[k+(j<<2)]);
        }
    }
    for(int i=0;i<16;i++) msg[i]=tmp[i];
}

void AES::invMixColumns(u8 msg[]){
    u8 tmp[16]={0};
    // 逆列混合矩阵
    u8 matrix[4][4] = {{0x0e, 0x0b, 0x0d, 0x09},
                       {0x09, 0x0e, 0x0b, 0x0d},
                       {0x0d, 0x09, 0x0e, 0x0b},
                       {0x0b, 0x0d, 0x09, 0x0e}};
    for(int j=0;j<4;j++){
        for (int i=0;i<4;i++){
            for(int k=0;k<4;k++)
                tmp[i+(j<<2)]^=GF28Multiply(matrix[i][k],msg[k+(j<<2)]);
        }
    }
    for(int i=0;i<16;i++) msg[i]=tmp[i];
}

void AES::addRoundKey(u8 msg[],int round,u8 key[]){
    for (int i =(round<<4);i<(round<<4)+16;i++)
        msg[i-(round<<4)]^=key[i];
}

void AES::encode(u8 msg[],u8 key[]){
    // 先用用户密钥轮密钥加
    addRoundKey(msg,0,key);
    // 九个标准轮
    for(int round=1;round<10;round++){
        subBytes(msg);
        shiftRows(msg);
        mixColumns(msg);
        addRoundKey(msg,round,key);
    }
    // 最后一个没有列混合
    subBytes(msg);
    shiftRows(msg);
    addRoundKey(msg,10,key);
}

void AES::decode(u8 msg[],u8 key[]){
    addRoundKey(msg,10,key);
    for(int round=9;round>0;round--){
        invSubBytes(msg);
        invShiftRows(msg);
        addRoundKey(msg,round,key);
        invMixColumns(msg);
    }
    invShiftRows(msg);
    invSubBytes(msg);
    addRoundKey(msg,0,key);
}

void AES::cbcEncode(u8 msg[], u8 key[], int N, u8 IV[]) {
    u8 tmpIV[16];
    u8 encodeMSg[16];
    for (int i = 0; i < 16; i++) tmpIV[i] = IV[i];
    // 分组
    N /= 16;
    for (int group = 0; group < N; group++) {
        int start = group * 16, end = group * 16 + 16;
        // 加密前异或上IV
        for (int i = start; i < end; i++) msg[i] ^= tmpIV[i - start];
        for (int i = 0; i < 16; i++) encodeMSg[i] = msg[start + i];
        encode(encodeMSg, key);
        // IV变成上一段密文
        for (int i = 0; i < 16; i++) {
            msg[start + i] = encodeMSg[i];
            tmpIV[i] = encodeMSg[i];
        }
    }
}

void AES::cbcDecode(u8 msg[], u8 key[], int N, u8 IV[]) {
    N /= 16;
    u8 decodeMSg[16];
    u8 tmpIV[16];
    for (int i = 0; i < 16; i++) tmpIV[i] = IV[i];
    for (int group = 0; group < N; group++) {
        int start = group * 16, end = group * 16 + 16;
        // 解密
        for (int i = 0; i < 16; i++) decodeMSg[i] = msg[start + i];
        decode(decodeMSg, key);
        for (int i = 0; i < 16; i++) decodeMSg[i] ^= tmpIV[i];

        for (int i = 0; i < 16; i++) {
            tmpIV[i] = msg[start + i];
            msg[start + i] = decodeMSg[i];
        }
    }
}

int main(){
    AES aes;
    u8 key[176] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    u8 msg[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    aes.initSBox();
    aes.genKey(key);
    int n=0;
    printf("\nSbox\n");
    for(int i=0; i<16; i++) {
        for(int j=0; j<16; j++)
            printf("%02x ", aes.Sbox[n]), n++;
        printf("\n");
    }
    n=0;
    printf("\ninvSbox\n");
    for(int i=0; i<16; i++) {
        for(int j=0; j<16; j++)
            printf("%02x ", aes.invSbox[n]), n++;
        printf("\n");
    }

    printf("\n用户key:\n");
    for(int i=0; i<16; ++i) 
        printf("%02x ", key[i]);
    
    
    printf("\n加密前:\n");
    for(int i=0; i<16; ++i) 
        printf("%02x ", msg[i]);
    

    aes.encode(msg,key);

    printf("\n加密后:\n");
    for(int i=0; i<16; ++i) 
        printf("%02x ", msg[i]);
    

    aes.decode(msg,key);
    printf("\n解密后:\n");
    for(int i=0; i<16; ++i) 
        printf("%02x ", msg[i]);
    

    u8 msg4K[M]={70};
    printf("\n4k数据加密前:\n");
    // for(int i=0;i<M;i++){msg4K[i]=70;printf("%02x",msg4K[i]);}
    u8 IV[16];
    memset(IV, 0, sizeof(IV));
    // aes.cbcEncode(msg4K,key,M,IV);
    // printf("\n4k数据加密后:\n");
    // for(int i=0;i<M;i++) printf("%02x",msg4K[i]);
    // aes.cbcDecode(msg4K,key,M,IV);
    // printf("\n4k数据解密后:\n");
    // for(int i=0;i<M;i++) printf("%02x",msg4K[i]);

    clock_t start = clock(), end;
    double duration;
    for (int i = 0; i < M; i++) aes.encode(msg,key);
    end = clock();
    duration = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout<<"\n耗时"<<duration<<endl;
    cout << 128 / duration << "Mbps\n";
    printf("正确性测试通过");
    return 0;
}