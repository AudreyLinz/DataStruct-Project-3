#include "minHeap.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>

template<typename T>
minHeap<T>::minHeap() {}

template<typename T>
minHeap<T>::minHeap(const std::vector<T>& v) : data(v) {
    heapify();
}

template<typename T>
bool minHeap<T>::empty() const {
    return data.empty();
}

template<typename T>
void minHeap<T>::insert(const T& val) {
    data.push_back(val);
    percolateUp();
}

template<typename T>
void minHeap<T>::percolateUp() {
    int child_idx = data.size() - 1;
    while(child_idx > 0) {
        int parent_idx = (child_idx - 1) / 2;
        if(data[child_idx] < data[parent_idx]) {
            std::swap(data[child_idx], data[parent_idx]);
            child_idx = parent_idx;
        } else break;
    }
}

template<typename T>
T minHeap<T>::deleteMin() {
    if(data.empty()) throw std::logic_error("Empty heap");
    T res = data[0];
    data[0] = data.back();
    data.pop_back();
    if(!data.empty()) percolateDown(0);
    return res;
}

template<typename T>
void minHeap<T>::percolateDown(int i) {
    int index = i;
    while(true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if(left < (int)data.size() && data[left] < data[smallest]) smallest = left;
        if(right < (int)data.size() && data[right] < data[smallest]) smallest = right;
        
        if(smallest != index) {
            std::swap(data[index], data[smallest]);
            index = smallest;
        } else break;
    }
}

template<typename T>
void minHeap<T>::heapify() {
    for(int i = (data.size()/2) - 1; i >= 0; i--) percolateDown(i);
}

template<typename T> 
int minHeap<T>::getLeftKidIndex(int i) { 
    return 2*i+1;
}

template<typename T> 
int minHeap<T>::getRightKidIndex(int i) { 
    return 2*i+2; 
}

template<typename T> 
int minHeap<T>::getParentIndex(int i) { 
    return (i-1)/2; 
}

template<typename T> 
int minHeap<T>::getLastWithKidsIndex() { 
    return data.size()/2 - 1; 
}