#include "VariableSizeDeque.h"

struct Node {

    // Variable to hold the data
    std::any data;

    // Maintains the pointers to the next and previous nodes
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;

    Node(std::any item) : data(item), next(nullptr) {}

};

struct CVariableSizeDeque::SImplementation{

    // Pointers to keep track of the head and tail of the list
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;

    // Keeps track of how many nodes are in the list
    TDequeSize size;

};

CVariableSizeDeque::CVariableSizeDeque(){
    DImplementation = std::make_unique<SImplementation>();
}

CVariableSizeDeque::~CVariableSizeDeque(){

}

TDequeSize CVariableSizeDeque::Size() const{

    // Time & Space Complexity: O(1)
    return DImplementation->size;

}

TDequeSize CVariableSizeDeque::MaxSize() const{

    // Time & Space Complexity: O(1)
    return GDequeSizeVariable;

}

std::any CVariableSizeDeque::Front() const{

    // Time & Space Complexity: O(1)
    if(DImplementation->size == 0) return std::any();

    return DImplementation->head->data;

}

std::any CVariableSizeDeque::Back() const{

    // Time & Space Complexity: O(1)
    if(DImplementation->size == 0) return std::any();

    return DImplementation->tail->data;

}

bool CVariableSizeDeque::PushBack(std::any item){

    // Time & Space Complexity: O(1)

    // Make a new node with the desired data, allocated on the heap
    std::shared_ptr<Node> temp = std::make_shared<Node>(item);

    // If the list is empty, set both pointers to that node (as head = tail in this case)
    // Else, insert a new node at the end, and fix the pointers accordingly
    if(DImplementation->size == 0) {

        DImplementation->head = temp;

        DImplementation->tail = temp;

    } else {

        DImplementation->tail->next = temp;

        temp->prev = DImplementation->tail;

        DImplementation->tail = temp;

    }

    DImplementation->size++;

    return true;

}

bool CVariableSizeDeque::PushFront(std::any item){

    // Time & Space Complexity: O(1)

    // Make a new node with the desired data, allocated on the heap
    std::shared_ptr<Node> temp = std::make_shared<Node>(item);

    // If the list is empty, set both pointers to that node (as head = tail in this case)
    // Else, insert a new node at the beginning, and fix the pointers accordingly
    if(DImplementation->size == 0) {

        DImplementation->head = temp;

        DImplementation->tail = temp;

    } else {

        DImplementation->head->prev = temp;

        temp->next = DImplementation->head;

        DImplementation->head = temp;

    }

    DImplementation->size++;

    return true;

}

bool CVariableSizeDeque::PopBack(){

    // Time & Space Complexity: O(1)

    if(DImplementation->size == 0) return false;

    DImplementation->tail = DImplementation->tail->prev.lock();

    // If there was only one node, popping the back is equivalent to emptying the list
    if(DImplementation->size == 1) DImplementation->head = nullptr;

    DImplementation->size--;

    return true;

}

bool CVariableSizeDeque::PopFront(){

    // Time & Space Complexity: O(1)

    if(DImplementation->size == 0) return false;

    DImplementation->head = DImplementation->head->next;

    // If there was only one node, popping the front is equivalent to emptying the list
    if(DImplementation->size == 1) DImplementation->tail = nullptr;

    DImplementation->size--;

    return true;

}

