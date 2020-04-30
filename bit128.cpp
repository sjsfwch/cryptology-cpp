#ifndef BIT128
#define BIT128

#endif 
typedef unsigned long long u64;

struct bit128
{
    u64 low,high;
    u64 operator[](int i);
    bit128(u64 high,u64 low){this->low=low;this->high=high;}
    bit128(){}
    void operator=(const bit128 &x);
    bool operator==(const bit128 &x);
    void operator<<=(int i);
    void operator>>=(int i);
    bit128 operator<<(int i);
    bit128 operator>>(int i);
};

u64 bit128::operator[](int i){
    //为了性能，不做越界检查，调用小心
    return i<64?(low>>i)&0x1:(high>>i)&0x1;
}

void bit128::operator=(const bit128 &x){
    low=x.low;
    high=x.high;
}

bool bit128::operator==(const bit128 &x){
    return low==x.low&&high==x.high;
}

void bit128::operator<<=(int i){
    if (i>=128){
        high=0;
        low=0;
    }
    else if (i>=64){
        high=low<<(i-64);
        low=0;
    }
    else{
        //high的低i位为low的高i位
        high=(high<<i)|(low>>(64-i));
        low<<=i;
    }
}

bit128 bit128::operator<<(int i){
    if (i>=128){
        return bit128(0,0);
    }
    else if (i>=64){
        return bit128(low<<(i-64),0);
    }
    else{
        return bit128((high<<i)|(low>>(64-i)),low<<i);
    }
}

void bit128::operator>>=(int i){
    if (i>=128){
        high=0;
        low=0;
    }
    else if (i>=64){
        low=high>>(i-64);
        high=0;
    }
    else{
        //low的高i位为high的低i位
        low=(low>>i)|(high<<(64-i));
        high>>=i;
    }
}

bit128 bit128::operator>>(int i){
    if (i>=128){
        return bit128(0,0);
    }
    else if (i>=64){
        return bit128(0,high>>(i-64));
    }
    else{
        return bit128(high>>i,(low>>i)|(high<<(64-i)));
    }
}

int main(){
    bit128 t(1,0xFFFFFFFFFFFFFFFF);
    auto x=t>>1;
    return 0;
}