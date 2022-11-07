#ifndef ARRAY_H
#define ARRAY_H

/*
	Given DTYPE stored by the Array class should have the following:
		constructor(const DTYPE &)
		operator=
		operator==
		ostream& operator<<
		a swapping function used for erasing without assignment operator
		operator< for sorting
*/


#include <iostream>
//#include "Iterator.h"


namespace sale {
	

// example of destroy_data_func in Array class
// deletes given pointer and sets it to nullptr
template<typename DTYPE>
void deletePtrArr(DTYPE* &x) {
	if (!x) return;
	//std::cout << "deleting: " << x << std::endl;
	delete x;
	x = nullptr;
}



// ===================================
// Array Exceptions:


//constexpr int ERROR_CODE_REALLOC = 1;
//constexpr int ERROR_CODE_IDX_OUT_OF_RANGE = 2;
//constexpr int ERROR_CODE_INVALID_IDX_RANGE = 3;


class ArrayCustomException : public std::exception {
 private:
	std::string msg = "An Array Error Ocurred";
 public:
	ArrayCustomException() = default;
	ArrayCustomException(const std::string &msg) : msg(msg) {}
	const char* what() const noexcept override { return this->msg.c_str(); }
};


class ArrayExceptionRealloc : public std::exception {
 public:
	const char* what() const noexcept override { return "Failed to reallocate"; }
};


class ArrayExceptionResize : public std::exception {
 public:
	const char* what() const noexcept override { return "Failed to resize"; }
};


class ArrayExceptionPushBack : public std::exception {
 public:
	const char* what() const noexcept override { return "Failed to push back"; }
};


class ArrayExceptionIdxOutOfRange : public std::exception {
 public:
	const char* what() const noexcept override { return "Idx out of range"; }
};


class ArrayExceptionInvalidIdxRange : public std::exception {
 private:
	const std::string msg = "Given range of indices is invalid";
 public:
	const char* what() const noexcept override { return this->msg.c_str(); }
};


class ArrayExceptionInvalidIterator : public std::exception {
 private:
	const std::string msg = "Iterator does not point to valid node address";
 public:
	const char* what() const noexcept override { return this->msg.c_str(); }
};


// ===================================
// Array and Friend functions Declarations:


template<typename DTYPE>
class Array;

template<typename DTYPE>
class Iterator;  // Iterator is friend of Array

template<typename DTYPE>
void swap(Array<DTYPE> &x, Array<DTYPE> &y);

template<typename DTYPE>
std::ostream& operator<< (std::ostream &os, const Array<DTYPE> &arr);


// ===================================
// Array Definition:


template<typename DTYPE>
class Array
{

 public:

	explicit Array(const int max_size=1,
				   void (*destroy_data_func)(DTYPE &about_to_be_destroyed)=nullptr);
	Array(const Array &arr);
	Array(Array &&arr);
	
	Array<DTYPE>& operator= (Array arr);

	~Array();

	// Constructor Helpers:
	void copy(const Array &arr);
	bool resize(const int new_capacity);

	// Destructor Helpers:
	void clear();

	// Getters:
	int getCapacity() const;
	int size() const;
	auto getDestroyDataFunction() const;

	// Setters:
	void setDestroyDataFunction(void (* destroy_data_func)(DTYPE &));

	// Lookups:
	DTYPE& operator[] (const int idx);
	const DTYPE& operator[] (const int idx) const;

	int find(const int start, const int end, const DTYPE &to_find) const;
	int operator()(const int start, const int end, const DTYPE &to_find) const;
	int operator()(const DTYPE &to_find) const;

	DTYPE& back();
	const DTYPE& back() const;

	// Insertion:
	void push_back(const DTYPE &to_add);
	void pop_back();

	// Deletion:
	void erase_swap(const int idx, void (* const swap_func)(DTYPE &x, DTYPE &y));
	void erase_assign(const int idx);

	// Misc. Methods:
	void sort();
	int fill(const DTYPE &to_set);

	// Friends and family :)
	friend class Iterator<DTYPE>;
	friend void swap <DTYPE> (Array &x, Array &y);
	friend std::ostream& operator<< <DTYPE> (std::ostream &os, const Array &arr);

	// Iterator Interface:
	
	Iterator<DTYPE> getIter();  // literally same as begin()
	Iterator<DTYPE> begin();
	Iterator<DTYPE> end();

	Iterator<DTYPE>& erase_swap(Iterator<DTYPE> &it,
								void (* const swap_func)(DTYPE &x, DTYPE &y));
	Iterator<DTYPE>& erase_assign(Iterator<DTYPE> &it);

 protected:

	virtual std::ostream& displaySelf(std::ostream &os) const;

 private:

	// Attributes:
	int cur_size;  // num of els in array
	int capacity;  // num of allocated els for the array
	DTYPE* data;  // the actual array
	int uninit_start;  // idx from which uninitialised allocated space begins
	// func to process data that is about to be overwritten or
	// freed from memory, all errors that can be caught will be ignored
	void (*destroy_data_func)(DTYPE &about_to_be_destroyed);

	// Constructor's helpers:
	void move(Array &arr);

};


}  // end of sale namespace




// ======================================================================
// ======================================================================
// ======================================================================

// Implementation:


namespace sale {


// ===================================
// Helper Functions:

namespace {


	template <typename DTYPE>
	void merge(Array<DTYPE> &arr, const int start,
			   const int left_size, const int right_size,
			   DTYPE* &arr_B)
	{
		for (int i = 0; i < right_size; ++i) {
			arr_B[i] = arr[start + left_size + i];
		}

		// sort in place, ptrs to first and second half (a and b)
		int ptr_arr = start + left_size + right_size - 1;
		int ptr_a = start + left_size - 1;
		int ptr_b = right_size - 1;
		for ( ; ptr_a >= start && ptr_b >= 0; --ptr_arr) {
			if (arr_B[ptr_b] < arr[ptr_a]) {
				arr[ptr_arr] = arr[ptr_a];
				ptr_a--;
			} else {
				arr[ptr_arr] = arr_B[ptr_b];
				ptr_b--;
			}
		}

		// add the leftovers of sigle array
		while (ptr_a >= start) {
			arr[ptr_arr--] = arr[ptr_a--];
		}

		while (ptr_b >= 0) {
			arr[ptr_arr--] = arr_B[ptr_b--];
		}
	}


	template <typename DTYPE>
	void mergeSort(Array<DTYPE> &arr) {
		if (arr.size() <= 1) return;

		const int n = arr.size();
		DTYPE* additional_space = new DTYPE[n / 2 + 1];

		for (int halfs_size = 1; halfs_size <= n; halfs_size <<= 1) {
			for (int start = 0; start + 2 * halfs_size <= n; start += 2 * halfs_size) {
				merge(arr, start, halfs_size, halfs_size, additional_space);
			}
			// merge the leftover
			int leftover_size = n % (2 * halfs_size);
			if (leftover_size < halfs_size) continue;
			int leftover_start = n - leftover_size;
			merge(arr, leftover_start, halfs_size, leftover_size - halfs_size,
				additional_space);
		}

		delete[] additional_space;
	}


}  // end of anonymous namespace of helper functions


// ===================================
// Array class Methods:


// ===================================
// Destructor:


template <typename DTYPE>
Array<DTYPE>::~Array() {
	this->clear();
}


// ===================================
// Constructors and Assignment Operators:


template<typename DTYPE>
Array<DTYPE>::Array(const int max_size, void (*destroy_data_func)(DTYPE &))
	: capacity(0), cur_size(0), destroy_data_func(destroy_data_func), uninit_start(0)
{
	// very important to set to nullptr for realloc not to raise errors
	this->data = nullptr;
	this->resize(max_size);
}

template<typename DTYPE>
Array<DTYPE>::Array(const Array& arr)
	: cur_size(0), capacity(0), destroy_data_func(nullptr), data(nullptr), uninit_start(0)
{
	this->copy(arr);
}

template<typename DTYPE>
Array<DTYPE>::Array(Array&& arr)
{
	this->move(arr);
}

template<typename DTYPE>
Array<DTYPE>& Array<DTYPE>::operator= (Array arr) {
	// compiler will figure out when Array&& is being converted into
	// Array and will not make a deep copy, so this func is those 2 above combined
	swap(*this, arr);
	return *this;
}


// ============================
// Constructor Helpers:


template<typename DTYPE>
void swap(Array<DTYPE> &x, Array<DTYPE> &y) {
	if (&x == &y) return;
	std::swap(x.cur_size, y.cur_size);
	std::swap(x.capacity, y.capacity);
	std::swap(x.uninit_start, y.uninit_start);
	std::swap(x.destroy_data_func, y.destroy_data_func);
	std::swap(x.data, y.data);
}

template<typename DTYPE>
void Array<DTYPE>::move(Array& arr)
{
	this->cur_size = arr.cur_size;
	arr.cur_size = 0;

	this->capacity = arr.capacity;
	arr.capacity = 0;

	this->uninit_start = arr.uninit_start;
	arr.uninit_start = 0;

	this->destroy_data_func = arr.destroy_data_func;
	arr.destroy_data_func = nullptr;

	this->data = arr.data;
	arr.data = nullptr;
}

template<typename DTYPE>
void Array<DTYPE>::copy(const Array& arr)
{
	DTYPE* tmp = new DTYPE[arr.capacity];
	//if (!tmp) {
	//	//std::cout << "\nresize: Failed to reallocate space\n";
	//	//std::exit(ERROR_CODE_REALLOC);
	//	throw ArrayExceptionRealloc();
	//}

	this->clear();
	this->cur_size = arr.cur_size;
	this->capacity = arr.capacity;
	this->uninit_start = arr.uninit_start;
	this->destroy_data_func = arr.destroy_data_func;
	this->data = tmp;

	for (int i = 0; i < this->capacity; ++i) {
		this->data[i] = arr.data[i];
	}
}

// reallocates space to given size - new capacity
// and returns true on success, raises runtime_error otherwise
// if new_capacity is 0 then just calls clear() and returns true
// ignores errors of festroy_data_func
template<typename DTYPE>
bool Array<DTYPE>::resize(const int new_capacity)
{
	if (new_capacity < 0) {
		throw ArrayCustomException("Given capacity was negative");
		return false;
	}

	// process all the elements to be destroyed
	if (this->destroy_data_func != nullptr) {
		for (int i = this->uninit_start - 1; i + 1 > new_capacity; --i) {
			try {
				this->destroy_data_func(this->data[i]);
			} catch (...) {

			}
		}
	}

	// reallocate the space and do not lose ptr to data in
	// case of reallocation error
	if (new_capacity > 0) {
		DTYPE* tmp = (DTYPE*) realloc(this->data, new_capacity * sizeof(DTYPE));
		if (!tmp) {
			//std::cout << "\nresize: Failed to reallocate space\n";
			//std::exit(ERROR_CODE_REALLOC);
			throw ArrayExceptionRealloc();
		}
		// successful reallocation
		this->data = tmp;
	}
	else {  // new_capacity == 0 so clear out all the data there is left
		delete[] this->data;
		this->data = nullptr;
	}

	this->capacity = new_capacity;
	// update num of existing els in case of shrinking
	if (this->cur_size > new_capacity) this->cur_size = new_capacity;
	if (this->uninit_start > new_capacity) this->uninit_start = new_capacity;

	return true;
}


// ============================
// Destructor Helpers:


template<typename DTYPE>
void Array<DTYPE>::clear()
{
	// process all the elements to be destroyed
	if (this->destroy_data_func != nullptr) {
		for (int i = this->uninit_start - 1; i >= 0; --i) {
			try {
				this->destroy_data_func(this->data[i]);
			} catch (...) {

			}
		}
	}

	this->cur_size = 0;
	this->capacity = 0;
	this->uninit_start = 0;
	//this->destroy_data_func = nullptr;
	delete[] this->data;
	this->data = nullptr;
}


// ============================
// Operator overloads and other friend functions:


// returns index of given element in the given range of the array,
// if no such element exists returns -1
// given range is inclusive
template<typename DTYPE>
int Array<DTYPE>::operator()(const int start, const int end,
							 const DTYPE& to_find) const
{
	return this->find(start, end, to_find);
}

// returns index of given element in the array,
// if no such element exists returns -1
//template<typename DTYPE>
//int Array<DTYPE>::operator()(const DTYPE& to_find) const {
//	return (*this)(0, this->cur_size - 1, to_find);
//}
template<typename DTYPE>
int Array<DTYPE>::operator()(const DTYPE& to_find) const {
	return (*this)(0, this->uninit_start - 1, to_find);
}

// virtual method for ostream logging
template<typename DTYPE>
std::ostream & sale::Array<DTYPE>::displaySelf(std::ostream & os) const
{
	//auto &arr = *this;
	//os << "Size: " << arr.cur_size << "; Capacity: " << arr.capacity << std::endl;
	//if (arr.cur_size <= 0) return os << "Empty array" << std::endl;
	////if (arr.cur_size <= 0) return os;

	//for (int i = 0, n = arr.cur_size - 1; i < n; ++i) {
	//	os << arr.data[i] << ", ";
	//}
	//// do not end with ", "
	//return os << arr.data[arr.cur_size - 1];
	return os;
}

template<typename DTYPE>
std::ostream& operator<< (std::ostream &os, const Array<DTYPE> &arr) {
	arr.displaySelf(os);
	return os;
}


// ============================
// Lookups:


template<typename DTYPE>
DTYPE& Array<DTYPE>::operator[] (const int idx) {
	if (idx < 0 || idx >= this->cur_size) {
		//throw std::runtime_error("Array idx out of range");
		//std::cout << "Array idx out of range\n";
		//std::exit(ERROR_CODE_IDX_OUT_OF_RANGE);
		throw ArrayExceptionIdxOutOfRange();
	}

	return this->data[idx];
}

template<typename DTYPE>
const DTYPE& Array<DTYPE>::operator[](const int idx) const {
	if (idx < 0 || idx >= this->cur_size) {
		//throw std::runtime_error("Array idx out of range");
		//std::cout << "Array idx out of range\n";
		//std::exit(ERROR_CODE_IDX_OUT_OF_RANGE);
		throw ArrayExceptionIdxOutOfRange();
	}

	return this->data[idx];
}

// returns reference to last el in the arr
// if arr is empty raises ArrayExceptionIdxOutOfRange
template<typename DTYPE>
DTYPE& Array<DTYPE>::back()
{
	return (*this)[this->cur_size - 1];
}

// returns const reference to last el in the arr
// if arr is empty raises ArrayExceptionIdxOutOfRange
template<typename DTYPE>
const DTYPE& Array<DTYPE>::back() const
{
	return (*this)[this->cur_size - 1];
}

// returns index of given element in the given range of the array,
// if no such element exists returns -1
// given range is inclusive
template<typename DTYPE>
int Array<DTYPE>::find(const int start, const int end,
					   const DTYPE& to_find) const
{
	if (this->cur_size <= 0) return -1;
	// automatically fix the given range
	//if (start < 0) start = 0;
	//else if (start >= this->max_size) start = this->max_size - 1;
	//if (end >= this->max_size) end = this->max_size - 1;

	if (start < 0 || start >= this->cur_size || end < 0 || end >= this->cur_size) {
		//throw std::runtime_error("Array: Invalid given range");
		//std::cout << "Array: Invalid given range\n";
		//std::exit(ERROR_CODE_INVALID_IDX_RANGE);
		throw ArrayExceptionInvalidIdxRange();
	}

	for (int i = start; i <= end; ++i) {
		if (this->data[i] == to_find) return i;
	}

	return -1;
}


// ============================
// Insertion:


// copies given element to the end of given array
// whenever capacity gets exceeded it doubles in size
template<typename DTYPE>
void Array<DTYPE>::push_back(const DTYPE &to_add) {
	// if there has not been enough allocated space
	// allocate more
	if (this->cur_size >= this->capacity) {
		if (this->capacity <= 0) this->capacity = 1;
		if (!this->resize(2 * this->capacity)) {
			// failed to reallocate
			//throw std::runtime_error("Failed to resize");
			//std::exit(ERROR_CODE_REALLOC);
			std::cout << "\npush_back: Failed to resize\n";
			throw ArrayExceptionPushBack();
			return;
		}
	}
	// overwriting and "destroying" old value if there is any(is not uninit)
	else if (this->destroy_data_func && this->cur_size < this->uninit_start) {
		try {
			this->destroy_data_func(this->data[this->cur_size]);
		} catch (...) {

		}
	}

	//this->data[this->cur_size] = std::move(to_add);
	this->data[this->cur_size] = to_add;
	++this->cur_size;
	// update the uninitalised allocated space idx in case
	// now there are fewer uninit els
	if (this->cur_size > this->uninit_start) {
		this->uninit_start = this->cur_size;
	}
}


// ============================
// Deletion:


// decreases cur_size, and
// shrinks capacity(and destorys els) by 2 whenever
// the number of els left is less than half the capacity
// and only then before destruction of the object calls destroy_data_func
// if it was previously set
template<typename DTYPE>
void Array<DTYPE>::pop_back()
{
	if (this->cur_size <= 0) return;

	--this->cur_size;
	//this->data[this->cur_size - 1].~DTYPE();

	// shrink 2 times whenever one half is empty
	if (this->cur_size < this->capacity / 2) {
		if (!this->resize(this->capacity / 2)) {
			// failed to reallocate
			++this->cur_size;
			//std::cout << "\npop_back: Failed to resize\n";
			//std::exit(ERROR_CODE_REALLOC);
			throw ArrayExceptionResize();
			return;
		}
	}
}


// removes an idx-th(0-indexed) el from the array and calls
// its destructor. Uses given swap_func
// to move the el to delete to the end before realloc is called as
// well as to shift the elements into their new positions
template<typename DTYPE>
void Array<DTYPE>::erase_swap(const int idx,
	void (* const swap_func)(DTYPE &x, DTYPE &y))
{
	// in case of invalid idx just return
	if (idx < 0 || idx >= this->cur_size) {
		//throw std::runtime_error("Array idx out of range");
		//std::cout << "erase_swap: Array idx out of range\n";
		//std::exit(ERROR_CODE_IDX_OUT_OF_RANGE);
		throw ArrayExceptionIdxOutOfRange();
		return;
	}

	// move el to delete to the end by swapping it with last el
	if (idx != this->cur_size - 1) {
		swap_func(this->data[idx], this->data[this->cur_size - 1]);
	}

	// remove the last el by resizing and call its destructor that way
	//--this->cur_size;
	if (!this->resize(this->cur_size - 1)) {
		//throw std::runtime_error("Failed to reallocate space");
		//std::cout << "erase_swap: Failed to reallocate space\n";
		//std::exit(ERROR_CODE_REALLOC);
		throw ArrayExceptionRealloc();
	}

	// shift all els after idx(deleted el) in left by 1 place
	// and bubble up the previosuly swapped el back to the end
	for (int i = idx + 1, n = this->cur_size; i < n; ++i) {
		swap_func(this->data[i - 1], this->data[i]);
	}
}

// moves an idx-th(0-indexed) el to end of the array while
// the shifting of els is done by asigning over previous values,
// calls destroy_data_func only on resize or push_back
// that steps over cur arr's size
// resize happens when number of els left is less than 1/2 capacity
template<typename DTYPE>
void Array<DTYPE>::erase_assign(const int idx)
{
	// in case of invalid idx just return
	if (idx < 0 || idx >= this->cur_size) {
		//throw std::runtime_error("Array idx out of range");
		//std::cout << "erase_assign: Array idx out of range\n";
		//std::exit(ERROR_CODE_IDX_OUT_OF_RANGE);
		throw ArrayExceptionIdxOutOfRange();
		return;
	}

	if (idx != this->cur_size - 1) {
		// el to delete is not the last element of array

		DTYPE tmp = this->data[idx];
		// shift all els after idx(deleted el) in left by 1 place
		// by calling assignment operator and overwrite the el to delete
		for (int i = idx + 1, n = this->cur_size; i < n; ++i) {
			this->data[i - 1] = this->data[i];
		}
		// move el to delete to the end of the array and keep it in memory
		// until next resize or push_back that steps over it
		this->data[this->cur_size - 1] = tmp;
	}

	// remove the last el by resizing and call its destructor that way
	--this->cur_size;

	if (this->cur_size < this->capacity / 2) {
		if (!this->resize(this->capacity / 2)) {
			++this->cur_size;
			//throw std::runtime_error("Failed to reallocate space");
			std::cout << "erase_assign: Failed to reallocate space\n";
			//std::exit(ERROR_CODE_REALLOC);
			throw ArrayExceptionRealloc();
		}
	}
}


// ============================
// Getters:


template<typename DTYPE>
int Array<DTYPE>::getCapacity() const {
	return this->capacity;
}

template<typename DTYPE>
int Array<DTYPE>::size() const {
	return this->cur_size;
}

// returns true if array uses a destroy_data_func
template<typename DTYPE>
auto Array<DTYPE>::getDestroyDataFunction() const
{
	return this->destroy_data_func;
}


// ============================
// Setters:


template<typename DTYPE>
void Array<DTYPE>::setDestroyDataFunction
(void(*destroy_data_func)(DTYPE& about_to_be_destroyed))
{
	this->destroy_data_func = destroy_data_func;
}


// ============================
// Misc. Methods


template <typename DTYPE>
void Array<DTYPE>::sort() {
	mergeSort(*this);
}

// pushes back given data until the
// size becomes equal to capacity and
// returns the number of els pushed back
template<typename DTYPE>
int Array<DTYPE>::fill(const DTYPE& to_set)
{
	int num_pushes = 0;
	for ( ; this->cur_size < this->capacity; ++num_pushes) {
		this->push_back(to_set);
	}
	return num_pushes;
}


}  // end of sale namespace

#endif
