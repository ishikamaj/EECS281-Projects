// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the priority queue ADT implemented as a binary heap
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty PQ with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            this->compare = comp;
    } // BinaryPQ


    // Description: Construct a PQ out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, 
    COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        this->compare = comp;
            while (start != end){
                data.push_back(*start);
                ++start;
            }
            updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order,
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        for(size_t i = data.size(); i != 0; --i) {
            fix_down(i);
        }
    } // updatePriorities()


    // Description: Add a new element to the PQ.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE &val) {
        data.push_back(val);
        fix_up(data.size());
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the PQ.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the PQ is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        std::swap(data[0], data.back());
        data.pop_back();
        fix_down(1);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the PQ.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return data.front();
    } // top()


    // Description: Get the number of elements in the PQ.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return data.size();
    } // size()


    // Description: Return true if the PQ is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return data.empty();
    } // empty()


private:
    // Note: This vector *must* be used for your PQ implementation.
    std::vector<TYPE> data;
    // NOTE: You are not allowed to add any member variables.  You don't need
    //       a "heapSize", since you can call your own size() member function,
    //       or check data.size().

    void fix_up(size_t k) {
        while(k > 1 && this->compare(get_element(k/2), get_element(k))) {
            std::swap(get_element(k), get_element(k/2));
            k /= 2;
        }
    }
    void fix_down(size_t k) {
        while(2 * k <= data.size()) {
            size_t j = 2 * k;
            if(j < data.size() && this->compare(get_element(j), 
            get_element(j+1))) ++j;
            if(!(this->compare(get_element(k), get_element(j)))) {
                break;
            } 
            std::swap(get_element(k), get_element(j));
            k = j;
        }
    }
    // Translation of 0-based indexing to 1-based indexing
    const TYPE &get_element(size_t i) const {
        return data[i - 1];
    }
    TYPE &get_element(size_t i) {
        return data[i - 1];
    }
}; // BinaryPQ


#endif // BINARYPQ_H
