
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