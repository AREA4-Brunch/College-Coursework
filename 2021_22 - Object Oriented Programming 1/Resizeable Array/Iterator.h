#ifndef ITERATOR_H
#define ITERATOR_H

#include "Array.h"  // Array and Iterator are friends both ways


namespace sale {


// ===================================
// Iterator and Friend functions Declarations:

    
template<typename DTYPE>
class Iterator;

template<typename DTYPE>
bool operator== (const Iterator<DTYPE> &x, const Iterator<DTYPE> &y);

template<typename DTYPE>
bool operator!= (const Iterator<DTYPE> &x, const Iterator<DTYPE> &y);

template<typename DTYPE>
void swap(Iterator<DTYPE> &x, Iterator<DTYPE> &y);

template<typename DTYPE>
Iterator<DTYPE> operator+ (const Iterator<DTYPE> &iter, const int increment);

template<typename DTYPE>
Iterator<DTYPE> operator+ (const int increment, const Iterator<DTYPE> &iter);

template<typename DTYPE>
Iterator<DTYPE> operator- (const Iterator<DTYPE> &iter, const int decrement);

template<typename DTYPE>
Iterator<DTYPE> operator- (const int decrement, const Iterator<DTYPE> &iter);


// ===================================
// Iterator Definition:


template<typename DTYPE>
class Iterator
{

 public:

     // Constructors:
     Iterator(Array<DTYPE>* const arr, const int idx=0);
     Iterator(Iterator<DTYPE> &&iter);
     Iterator(const Iterator &iter);

     Iterator& operator= (Iterator iter);

     // Destructor
     ~Iterator() {}

     // fetching the data
     DTYPE* operator-> () const;
     DTYPE& operator* () const;

     // convert this into object of type bool
     operator bool () const;
     bool operator! () const;

     // converting into int returns the idx in the array,
     // returned value can be out of array's bounds
     operator int() const;

     friend bool operator== <DTYPE> (const Iterator &x, const Iterator &y);
     friend bool operator!= <DTYPE> (const Iterator &x, const Iterator &y);

     Iterator& operator++ ();
     Iterator& operator++ (const int k);

     Iterator& operator-- ();
     Iterator& operator-- (const int k);

     friend Iterator<DTYPE> operator+ <DTYPE> (const Iterator<DTYPE> &iter, const int increment);
     friend Iterator<DTYPE> operator+ <DTYPE> (const int increment, const Iterator<DTYPE> &iter);
     int operator+ (const Iterator &iter) const;
     Iterator& operator+= (const int increment);

     friend Iterator<DTYPE> operator- <DTYPE> (const Iterator<DTYPE> &iter, const int decrement);
     friend Iterator<DTYPE> operator- <DTYPE> (const int decrement, const Iterator<DTYPE> &iter);
     int operator- (const Iterator &iter) const;
     Iterator& operator-= (const int decrement);

 private:

    Array<DTYPE> *arr;  // ptr to instance of arr
    DTYPE *it;

    friend class Array<DTYPE>;
    friend void swap <DTYPE> (Iterator<DTYPE> &x, Iterator<DTYPE> &y);

};


// ======================================================================
// ======================================================================
// ======================================================================
// Implementation:


// ===================================
// Globals:

namespace {

    constexpr const int NINF = 1 << 31;

}


// ===================================
// Constructors and Assignment Operators:


template<typename DTYPE>
Iterator<DTYPE>::Iterator(Array<DTYPE>* const arr, const int idx)
    : arr(arr), it(&(arr->data[idx]))
{

}

template<typename DTYPE>
Iterator<DTYPE>::Iterator(const Iterator& iter)
    : arr(iter.arr), it(iter.it)
{

}

template<typename DTYPE>
Iterator<DTYPE>::Iterator(Iterator<DTYPE>&& iter)
    : arr(iter.arr), it(iter.it)
{
    iter.arr = nullptr;
    iter.it = nullptr;
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator= (Iterator<DTYPE> iter) {
    swap(*this, iter);
    return *this;
}


// ============================
// Constructor Helpers:


template<typename DTYPE>
void swap(Iterator<DTYPE> &x, Iterator<DTYPE> &y) {
    std::swap(x.arr, y.arr);
    std::swap(x.it, y.it);
}


// ============================
// Operator overloads and other friend functions:


template<typename DTYPE>
DTYPE* Iterator<DTYPE>::operator->() const
{
    return this->it;
}

template<typename DTYPE>
DTYPE& Iterator<DTYPE>::operator*() const
{
    return *(this->it);
}

template<typename DTYPE>
Iterator<DTYPE>::operator bool() const
{
    return this->it != nullptr
        && this->it >= this->arr->data
        && this->it < this->arr->data + this->arr->capacity;
}

template<typename DTYPE>
bool Iterator<DTYPE>::operator!() const
{
    return *this == false;
}

template<typename DTYPE>
Iterator<DTYPE>::operator int() const
{
    return (this->it - this->arr->data);
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator++()
{
    ++this->it;
    return *this;
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator++(const int k)
{
    if (k == 0) return ++(*this);
    return *this += k;
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator--()
{
    --this->it;
    return *this;
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator--(const int k)
{
    if (k == 0) return ++(*this);
    return *this -= k;
}

// returns sum of indices to which both iters point to
// in the same array, if iters do not point to the same
// array then NINF is returned
template<typename DTYPE>
int Iterator<DTYPE>::operator+(const Iterator& iter) const
{
    if (this->arr != iter.arr) return NINF;
    return int(*this) + int(iter);
}

// returns difference of indices to which both iters point to
// in the same array, if iters do not point to the same
// array then NINF is returned
template<typename DTYPE>
int Iterator<DTYPE>::operator-(const Iterator& iter) const
{
    if (this->arr != iter.arr) return NINF;
    return int(*this) - int(iter);
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator+=(const int increment)
{
    this->it += increment;
    return *this;
}

template<typename DTYPE>
Iterator<DTYPE>& Iterator<DTYPE>::operator-=(const int decrement)
{
    this->it -= decrement;
    return *this;
}

// returns true if both iterators point to the same address
template<typename DTYPE>
bool operator== (const Iterator<DTYPE> &x, const Iterator<DTYPE> &y)
{
    // if addresses are the same so is the array they point to
    return x.it == y.it;
}

// returns true if the iterators do not point to the same address
template<typename DTYPE>
bool operator!= (const Iterator<DTYPE> &x, const Iterator<DTYPE> &y)
{
    // if addresses are the same so is the array they point to
    return !(x == y);
}

template<typename DTYPE>
Iterator<DTYPE> operator+(const Iterator<DTYPE>& iter, const int increment)
{
    //return Iterator<DTYPE>(iter.arr, iter.it - iter.arr->data + increment);
    return Iterator<DTYPE>(iter.arr, increment + int(iter));
}

template<typename DTYPE>
Iterator<DTYPE> operator+(const int increment, const Iterator<DTYPE>& iter)
{
    //return Iterator<DTYPE>(iter.arr, iter.it - iter.arr->data + increment);
    //return Iterator<DTYPE>(iter.arr, increment + int(iter));
    return iter + increment;
}

template<typename DTYPE>
Iterator<DTYPE> operator-(const Iterator<DTYPE>& iter, const int decrement)
{
    //return Iterator<DTYPE>(iter.arr, iter.it - iter.arr->data - decrement);
    return Iterator<DTYPE>(iter.arr, int(iter) - decrement);
}

template<typename DTYPE>
Iterator<DTYPE> operator-(const int decrement, const Iterator<DTYPE>& iter)
{
    //return Iterator<DTYPE>(iter.arr, iter.it - iter.arr->data - decrement);
    //return Iterator<DTYPE>(iter.arr, int(iter) - decrement);
    return iter - decrement;
}


// ====================================================
// Array Friend Functions:


template<typename DTYPE>
Iterator<DTYPE> Array<DTYPE>::getIter() {
    return Iterator<DTYPE>(this, 0);
}

template<typename DTYPE>
Iterator<DTYPE> Array<DTYPE>::begin() {
    return Iterator<DTYPE>(this, 0);
}

template<typename DTYPE>
Iterator<DTYPE> Array<DTYPE>::end() {
    return Iterator<DTYPE>(this, this->cur_size);
}

// removes an el to which given iter points to from the array and calls
// its destructor. Uses given swap_func to move the el to delete to the
// end before realloc is called as well as to shift the elements into
// their new positions.
// In case of invalid iter leaves it unchanged and returns it.
// Otherwise iterator points to next value in the array or end() if empty.
template<typename DTYPE>
Iterator<DTYPE>& Array<DTYPE>::erase_swap(Iterator<DTYPE>& iter,
    void (* const swap_func)(DTYPE& x, DTYPE& y))
{
    if (!iter || this != iter.arr) return iter;

    int idx = iter.it - this->data;  // idx in array to which iter points to
    this->erase_swap(idx, swap_func);

    // in case of empty array set it to end()
    if (this->cur_size <= 0) iter = this->end();
    else iter.it = &(this->data[idx]);

    return iter;
}

// removes an el to which given iter points to from the array where
// the shifting of els is done by asigning over preious values,
// resize and actual deletion happen when number of
// els left is less than half the capacity
// In case of invalid iter leaves it unchanged and returns it.
// Otherwise iterator points to next value in the array or end() if empty.
template<typename DTYPE>
Iterator<DTYPE>& Array<DTYPE>::erase_assign(Iterator<DTYPE>& iter) {
    if (!iter || this != iter.arr) return iter;

    int idx = iter.it - this->data;  // idx in array to which iter points to
    this->erase_assign(idx);

    // in case of empty array set it to end()
    if (this->cur_size <= 0) iter = this->end();
    else iter.it = &(this->data[idx]);

    return iter;
}

}  // end of sale namespace

#endif
