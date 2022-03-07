#ifndef STRINGHEAP_H
#define STRINGHEAP_H

#include<QVector>

class StringHeap
{
    /*小根堆:优先队列*/
public:
    /*成员函数*/

    //默认构造
    StringHeap();

    //判空
    bool isEmpty();

    //获取最小值
    QPair<QString,int> top();

    //插入数据
    void push(QPair<QString,int> q);

    //删除最小值
    void pop();

    //下沉:每次删除数据时
    void sink(int pos);

    //上浮:每次插入数据时
    void swim(int pos);

    //交换数据
    void swap(int pos1,int pos2);

private:
    /*成员变量*/

    //长度
    int N;

    //容器
    QVector<QPair<QString,int>> data;//1~N
};

#endif // STRINGHEAP_H
