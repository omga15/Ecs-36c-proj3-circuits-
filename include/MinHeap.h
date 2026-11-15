#ifndef MINHEAP
#define MINHEAP

#include <vector>
#include <stdexcept>
#include <utility>

template<typename T>
class MinHeap {

    private:
        std::vector<T> tree;

        void floatDown(std::size_t index) {

            std::size_t size = tree.size();
            std::size_t min = index;

            while(true) {

                std::size_t left = 2 * index + 1;
                std::size_t right = 2 * index + 2;

                if(left < size && tree[left] < tree[min]) min = left;
                if(right < size && tree[right] < tree[min]) min = right;

                if(index == min) break;

                std::swap(tree[index], tree[min]);

                index = min;

            }

        }

        void floatUp(std::size_t index) {

            while(index > 0) {

                std::size_t parent = (index - 1) / 2;

                if(tree[index] >= tree[parent]) break;

                std::swap(tree[index], tree[parent]);

                index = parent;

            }

        }

    public:
        MinHeap() = default;
        ~MinHeap() = default;
        
        const T& top() const {

            if(tree.empty()) throw std::out_of_range("[TOP] Heap is empty.");

            return tree[0];

        }

        void push(const T& value) {

            tree.push_back(value);

            floatUp(tree.size() - 1);

        }

        void pop() {

            if(tree.empty()) throw std::out_of_range("[POP] Heap is empty.");

            tree[0] = std::move(tree.back());

            tree.pop_back();

            if(!tree.empty()) floatDown(0);

        }

        bool empty() const {

            return tree.empty();

        }  
        
        std::size_t size() const {

            return tree.size();

        }
};

#endif