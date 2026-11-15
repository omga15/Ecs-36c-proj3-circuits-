#ifndef DEQUE_H
#define DEQUE_H

#include <any>
#include <limits>

using TDequeSize = std::size_t;
constexpr TDequeSize GDequeSizeVariable =  std::numeric_limits<std::size_t>::max();

class CDeque{
    public:
        virtual ~CDeque() = default;

        virtual TDequeSize Size() const = 0;
        virtual TDequeSize MaxSize() const = 0;

        virtual std::any Front() const = 0;
        virtual std::any Back() const = 0;

        virtual bool PushBack(std::any item) = 0;
        virtual bool PushFront(std::any item) = 0;

        virtual bool PopBack() = 0;
        virtual bool PopFront() = 0;
};

#endif
