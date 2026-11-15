#ifndef VARIABLE_SIZE_DEQUE_H
#define VARIABLE_SIZE_DEQUE_H

#include <memory>
#include "Deque.h"

class CVariableSizeDeque : public CDeque{ 
    private:
        struct SImplementation;
        std::unique_ptr< SImplementation > DImplementation;

    public:
        CVariableSizeDeque();
        virtual ~CVariableSizeDeque();

        TDequeSize Size() const override;
        TDequeSize MaxSize() const override;

        std::any Front() const override;
        std::any Back() const override;

        bool PushBack(std::any item) override;
        bool PushFront(std::any item) override;

        bool PopBack() override;
        bool PopFront() override;
};

#endif
