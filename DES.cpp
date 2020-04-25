#include<stdio.h>
#include<random>
using namespace std;

typedef u_int8_t u8;
template <typename T>
void swap(T *c1, T *c2) {
    T c = *c1;
    *c1 = *c2;
    *c2 = c;
}
class DES {
    int permutationBox[64],inversePermutationBox[64];
    void disorderPermutationBoxes();
    void resetPermutationBoxes();
    void setPermutationBoxes(int box[]);
    DES();
    ~DES();
};

DES::DES(){}

DES::~DES(){}

void DES::resetPermutationBoxes(){
    for (int i = 0;i<64;i++){
        permutationBox[i]=i;
        inversePermutationBox[permutationBox[i]]=i;
    }

}

void DES::disorderPermutationBoxes(){
    for(int i = 63;i>=0;i--){
        swap(&permutationBox[i],&permutationBox[rand() % i]);
        inversePermutationBox[permutationBox[i]]=i;
    }
}

void DES::setPermutationBoxes (int box[64]){
    for(int i=0;i<64;i++){
        permutationBox[i]=box[i];
        inversePermutationBox[permutationBox[i]]=box[i];
    }
}