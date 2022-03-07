#include "stringheap.h"
#include <QDebug>

StringHeap::StringHeap()
{
    this->data.push_back(QPair<QString,int>("",0));//占第一位
    this->N = 0;
}

//判空
bool StringHeap::isEmpty()
{
    if(this->N==0)
        return true;
    else
        return false;
}

//获取最小值
QPair<QString,int> StringHeap::top()
{
    //小根堆
    return this->data[1];
}

//插入数据
void StringHeap::push(QPair<QString,int> q)
{
    //插入最后方
    this->data.push_back(q);

    //增加长度
    this->N++;

    //上浮
    this->swim(N);
}

//删除最小值
void StringHeap::pop()
{
    //最后一位上位，然后下沉
    this->data[1] = this->data.last();

    //删除最后一个元素
    this->data.pop_back();

    //减少长度
    this->N--;

    //下沉
    this->sink(1);
}

//下沉:每次删除数据时
void StringHeap::sink(int pos)
{
    //调用
    int i;
    while(2*pos<= this->N)
    {
        i = 2*pos;
        if(i<this->N && this->data[i].first>this->data[i+1].first) i++;//i指向子节点中最小的
        if(this->data[pos].first<this->data[i].first) break;//已经找到相应位置
        //否则交换
        swap(pos,i);
        pos=i;
    }
}

//上浮:每次插入数据时
void StringHeap::swim(int pos)
{
    while(pos>1&&this->data[pos/2].first > this->data[pos].first)
    {
        swap(pos,pos/2);
        pos = pos/2;
    }
}

//交换数据
void StringHeap::swap(int pos1,int pos2)
{
    QString temp;
    int index;
    temp = this->data[pos1].first;
    index = this->data[pos1].second;
    this->data[pos1].first = this->data[pos2].first;
    this->data[pos1].second = this->data[pos2].second;
    this->data[pos2].first = temp;
    this->data[pos2].second = index;
}
