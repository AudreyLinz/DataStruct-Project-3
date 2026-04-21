#ifndef MIN_HEAP_HPP
#define MIN_HEAP_HPP

#include <vector>

template<typename T>
class minHeap {
public:
    //Contructors
    minHeap(); 
    minHeap(const std::vector<T>& v); 

    void insert(const T& val);
    void print() const;
    T deleteMin();
    bool empty() const;
private:
    std::vector<T> data;
    void heapify();
    void percolateUp();
    void percolateDown(int i = 0);
    int getLeftKidIndex(int i);
    int getRightKidIndex(int i);
    int getParentIndex(int i);
    int getLastWithKidsIndex();
};

#endif