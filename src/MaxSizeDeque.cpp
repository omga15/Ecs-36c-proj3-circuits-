#include "MaxSizeDeque.h"

struct CMaxSizeDeque::SImplementation{

    // Size-related values
    TDequeSize capacity;
    TDequeSize size;

    // Indices to track the front and back of the deque
    int front;
    int back;

    // Will be responsible for the dynamically allocated, fixed-size array
    std::unique_ptr<std::any[]> array;

    // Constructor that initializes all member variables, and allocates the underlying array
    SImplementation(TDequeSize maxSize) : capacity(maxSize), size(0), front(0), back(0), array(std::make_unique<std::any[]>(capacity)) {}

};

CMaxSizeDeque::CMaxSizeDeque(TDequeSize maxsize){
    DImplementation = std::make_unique<SImplementation>(maxsize);
}

CMaxSizeDeque::~CMaxSizeDeque(){

}

TDequeSize CMaxSizeDeque::Size() const{

    // Time & Space Complexity: O(1)
    return DImplementation->size;

}

TDequeSize CMaxSizeDeque::MaxSize() const{

    // Time & Space Complexity: O(1)
    return DImplementation->capacity;

}

std::any CMaxSizeDeque::Front() const{

    if(DImplementation->size == 0) return std::any();

    // Time & Space Complexity: O(1)
    return DImplementation->array[DImplementation->front];

}

std::any CMaxSizeDeque::Back() const{

    // Time & Space Complexity: O(1)
    if(DImplementation->size == 0) return std::any();

    return DImplementation->array[DImplementation->back];

}

bool CMaxSizeDeque::PushBack(std::any item){

    // Time & Space Complexity: O(1)
    if(DImplementation->size == DImplementation->capacity) return false;

    // If the array has no elements, don't move the back index
    if(DImplementation->size != 0) {

        DImplementation->back++;

        if(DImplementation->back == DImplementation->capacity) DImplementation->back = 0;

    }

    DImplementation->array[DImplementation->back] = item;

    DImplementation->size++;

    return true;

}

bool CMaxSizeDeque::PushFront(std::any item){

    // Time & Space Complexity: O(1)
    if(DImplementation->size == DImplementation->capacity) return false;

    // If the array has no elements, don't move the front index
    if(DImplementation->size != 0) {

        DImplementation->front--;

        if(DImplementation->front < 0) DImplementation->front = DImplementation->capacity - 1;

    }

    DImplementation->array[DImplementation->front] = item;

    DImplementation->size++;

    return true;

}

bool CMaxSizeDeque::PopBack(){

    // Time & Space Complexity: O(1)
    if(DImplementation->size == 0) return false;

    // If the array has only one element, don't move the back index
    if(DImplementation->size != 1) {

        DImplementation->back--;

        if(DImplementation->back < 0) DImplementation->back = DImplementation->capacity - 1;

    }

    DImplementation->size--;

    return true;

}

bool CMaxSizeDeque::PopFront(){

    // Time & Space Complexity: O(1)
    if(DImplementation->size == 0) return false;

    // If the array has only one element, don't move the front index
    if(DImplementation->size != 1) {

        DImplementation->front++;

        if(DImplementation->front >= DImplementation->capacity) DImplementation->front = 0;

    }

    DImplementation->size--;

    return true;

}
