#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

/*
	Singly Linked List with head and tail, NOT THREAD-SAFE.
	Given DTYPE stored by the Linked List class should have the following:
		constructor(const DTYPE &)
		operator== - for searching by value and value based removals
		kernel::detail::linked_list::swap (move constructor and move assign. operator)
                            for extractNode
		operator< for sorting
		//ostream& operator<< for logging the node
*/


#include "MemoryAllocator.hpp"


namespace kernel {
namespace utils {


template<typename DTYPE>
class LinkedList
{

 public:

    struct Node {
        DTYPE data;
        Node *next = nullptr;

        // Node() = default;
        Node(const DTYPE &data) : data(data) {}
        ~Node() = default;

        /**
         * Meant to be used only in supervisor mode where no
         * other allocation on allocator is requested at the same time.
         */
        void* operator new (size_t size) {
            auto &allocator = kernel::MemoryAllocator::getInstance();
            return allocator.mallocBytes(size);
        }

        /**
         * Meant to be used only in supervisor mode where no
         * other deallocation on allocator is requested at the same time.
         */
        void operator delete (void *data) {
            auto &allocator = kernel::MemoryAllocator::getInstance();
            // destructor is called implicitly before operator delete
            // ((Node *) data)->~Node();
            allocator.free(data);
        }
    };


    // ============================
    // Constructors and assignment:


    LinkedList();
    /// Move constructor.
    LinkedList(LinkedList &&list);
    /// Constructs deep copy of the list and its data out of given list.
    LinkedList(const LinkedList &list);

    /// Move and copy assignment operators implemented via copy and swap idiom.
    LinkedList<DTYPE>& operator= (LinkedList list);


    /**
     * Meant to be used only in supervisor mode where no
     * other allocation on allocator is requested at the same time.
     */
    void* operator new (size_t size) {
        auto &allocator = kernel::MemoryAllocator::getInstance();
        return allocator.mallocBytes(size);
    }


    // ============================
    // Destructor


    ~LinkedList();

    /**
     * Meant to be used only in supervisor mode where no
     * other deallocation on allocator is requested at the same time.
     */
    void operator delete (void *data) {
        auto &allocator = kernel::MemoryAllocator::getInstance();
        // destructor is called implicitly before operator delete
        //((LinkedList *) data)->LinkedListOnSteroidsOnSteroids();
        allocator.free(data);
    }


    // ============================
    // Constructor helpers:


    /// Swaps two given linked lists.
    static void swap(kernel::utils::LinkedList<DTYPE> &x,
                     kernel::utils::LinkedList<DTYPE> &y);


    // ============================
    // Destructor helpers:


    /// Removes all elements in the list. Evokes destroy_data_func
    /// on each element before its node's deallocation.
    void clear();


    // =============================
    // Getters:


    /// Returns number of elements in the list.
    int size() const;

    /// Returns whether linked list is empty or not.
    bool empty() const;


    // ============================
    // Setters:


    /// Sets function which is called on objects right before
    /// their removal from the list.
    void setDestroyDataFunction(void (* const destroy_data_func)(DTYPE &));


    // ============================
    // Misc. methods:


    /// Reverses the order of elements in the list.
    void reverse();

    /// Sorts linked list in given order (ascending is default)
    /// inplace in O(N*logN).
    /// Uses operator< for comparison.
    void sort(bool ascending=true);

    /// Sorts linked list in given order (ascending is default)
    /// inplace in O(N*logN).
    /// cmp_func should return true if left arg should come
    /// before the right arg in the final sorted order.
    void sort(bool (*cmp_func)(const DTYPE &x, const DTYPE &y));


    // ============================
    // Adding data:


    /// Appends given data. To check if appending was successful
    /// you can compare lengths before and after the call.
    LinkedList& operator+= (const DTYPE &data);

    /// Inserts given data as new node in the idx-th position and
    /// returns true on success, false otherwise.
    bool insert(const int idx, const DTYPE &data);

    /// Adds given data to end of the linked list and
    /// returns -1 in case of error, length of the list otherwise.
    int push_back(const DTYPE &data);

    /// Adds given data to beginning of the linked list and
    /// returns -1 in case of error, length of the list otherwise.
    int push_front(const DTYPE &data);


    // ============================
    // Adding and removing data helpers:


    /// Links a given node ptr to end of the linked list
    /// returns -1 in case of error, else new length of the list.
    int appendNode(kernel::utils::LinkedList<DTYPE>::Node *new_node);

    /// Creates new node with given data after given node, if
    /// given node is nullptr then prepends the new node.
    /// Returns true in case of success, false otherwise.
    bool insertAfter(kernel::utils::LinkedList<DTYPE>::Node *prev_node,
                     const DTYPE &data);


    // ============================
    // Removing data:


    /// Removes last element from the list.
    /// Returns false if list was empty, true otherwise.
    bool pop_back();

    /// Removes first element and returns pointer to new head,
    /// if list was/is empty returns nullptr.
    kernel::utils::LinkedList<DTYPE>::Node* pop_front();

    /// Removes given node from the linked list and
    /// returns next node, if list was/is empty returns nullptr.
    /// Uses move assignment operator to swap data with next if
    /// given node to remove is not head or tail.
    kernel::utils::LinkedList<DTYPE>::Node*
        deleteLinkedListNode(kernel::utils::LinkedList<DTYPE>::Node *to_delete);

    /// Removes element at given position (0-indexed).
    /// If index is invalid just ignores the call.
    void erase(const int idx);

    /// Removes all occurrences of given data from the list
    /// and returns number of nodes removed.
    /// Uses operator==(const DTYPE &, const DTYPE &)
    /// to compare the data.
    int removeValues(const DTYPE &data);


    // ============================
    // Lookups:


    // Searching data by idx:

    /// Returns reference to element at given position (0-indexed).
    DTYPE& operator[] (const int idx);

    /// Returns const reference to element at given position (0-indexed).
    const DTYPE& operator[] (const int idx) const;

    /// Returns reference to element at given position (0-indexed).
    DTYPE& getDataByIdx(const int idx);

    /// Returns const reference to element at given position (0-indexed).
    const DTYPE& getDataByIdx(const int idx) const;


    // Searching nodes by idx or value:

    /// Returns node at given position (0-indexed).
    kernel::utils::LinkedList<DTYPE>::Node*
        getNodeByIdx(const int idx) const;

    /// Returns first node with given data.
    /// Uses operator==(const DTYPE &, const DTYPE &) for comparison.
    kernel::utils::LinkedList<DTYPE>::Node*
        getNodeByValue(const DTYPE &data);

    /// Returns ptr to node with given data and sets given
    /// ptr prev to node preceding the found node in
    /// case of successful search, otherwise just returns nullptr and
    /// does not modify given ptr prev.
    /// Uses operator==(const DTYPE &, const DTYPE &) for comparison.
    kernel::utils::LinkedList<DTYPE>::Node*
        find(const DTYPE &data,
             kernel::utils::LinkedList<DTYPE>::Node* &prev);


    // ============================
    // Extracting nodes:


    /// Unlinks node at given position (0-indexed).
    kernel::utils::LinkedList<DTYPE>::Node*
        extractByIdx(const int idx);

    /// Unlinks the node with given data from the list and returns it
    /// in case of no such node returns nullptr.
    /// Uses operator==(const DTYPE &, const DTYPE &) for comparison.
    kernel::utils::LinkedList<DTYPE>::Node*
        extractByValue(const DTYPE &data);

    /// Unlinks given node from the list and returns it.
    /// O(1) except for removal of last element when prev is
    /// not provided, then O(N).
    /// If prev is not provided and node is not last in list
    /// then performs swap which requires move assignment operator.
    /// If given node is invalid does nothing and returns nullptr.
    kernel::utils::LinkedList<DTYPE>::Node*
        extractNode(kernel::utils::LinkedList<DTYPE>::Node *node,
                    kernel::utils::LinkedList<DTYPE>::Node *prev=nullptr);


    // ============================
    // Iterator "Interface":


    /// Sets the iterator to the beginning of the list and
    /// prev to nullptr.
    void iter_begin() const;

    /// Advances iterator and prev by one step to the right,
    /// if it was already pointing to end then does nothing.
    void iter_next() const;

    /// Returns true if iterator points to valid node (not nullptr)
    /// false otherwise.
    bool iter_not_end() const;

    /// Returns pointer to data in the node which the
    /// iter points to, if no such node returns nullptr.
    DTYPE* iter_data();

    /// Returns pointer to const data in the node which the
    /// iter points to, if no such node returns nullptr.
    const DTYPE* iter_data() const;


 protected:

    // ============================
    // Linked List Properties:


    int length = 0;  // num of nodes in the list
    Node *head = nullptr;
    Node *tail = nullptr;
    // func to process data that is about to be overwritten or
    // freed from memory
    void (*destroy_data_func)(DTYPE &about_to_be_destroyed) = nullptr;


    // ============================
    // Constructor helpers:


    void move(LinkedList &list);
    void copy(const LinkedList &list);


    // ============================
    // Misc. methods:


    bool isValidIndex(const int idx) const;


 private:

    // ============================
    // Iterator "interface" fields:

    mutable Node *iter = nullptr;  // Node currently being pointed to
    mutable Node *prev = nullptr;


    // ============================
    // Misc. Helpers:


    /// Sorts singly linked list, does not update tail.
    static void mergeSort(LinkedList<DTYPE> &list,
                          const bool ascending=true);

    /// Sorts singly linked list, does not update tail.
    /// cmp_func should return true if left arg should come
    /// before the right arg in the final sorted order.
    static void mergeSort(LinkedList<DTYPE> &list,
                          bool (*cmp_func)(const DTYPE &x, const DTYPE &y));


};


}  // utils namespace
}  // kernel namespace



// ======================================================================
// ======================================================================
// ======================================================================

// Implementation:



// ===================================
// Helper Functions:


namespace kernel {
namespace detail {
namespace linked_list {


    // example of destroy_data_func in LinkedList class
    // deletes given pointer and sets it to nullptr
    //template<typename DTYPE>
    //void deletePtr(DTYPE* &x) {
    //	if (!x) return;
    //	//std::cout << "deleting: " << x << std::endl;
    //	delete x;
    //	x = nullptr;
    //}


    //constexpr int ERROR_CODE_POP_BACK_FAILURE = 1;
    //constexpr int ERROR_CODE_IDX_OUT_OF_RANGE = 2;


    template<typename DTYPE>
    void swap(DTYPE &x, DTYPE &y);


    template<typename SinglyLinkedListNode>
    void mergeStep(SinglyLinkedListNode *&new_tail,
                   const int left_size, const int right_size,
                   const bool ascending = true);


    template<typename SinglyLinkedListNode, typename DTYPE>
    void mergeStep(SinglyLinkedListNode* &new_tail,
                   const int left_size,
                   const int right_size,
                   bool (*cmp_func)(const DTYPE &x, const DTYPE &y));


}  // linked_list namespace
}  // detail namespace
}  // kernel namespace


// ===================================
// Destructor:


template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>::~LinkedList()
{
    this->clear();
}


// ===================================
// Constructors:


template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>::LinkedList()
        : length(0), head(nullptr), tail(nullptr), destroy_data_func(nullptr)
{
}

template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>::LinkedList
    (const kernel::utils::LinkedList<DTYPE> &list)
{
    this->copy(list);
}

template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>::LinkedList(LinkedList &&list)
{
    this->move(list);
}

template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>&
    kernel::utils::LinkedList<DTYPE>::operator= (LinkedList list)
{
    kernel::utils::LinkedList<DTYPE>::swap(*this, list);
    // this->swap(*this, list);
    return *this;
}


// ============================
// Constructor Helpers:


template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::copy
    (const kernel::utils::LinkedList<DTYPE> &list)
{
    this->destroy_data_func = list.destroy_data_func;
    this->iter = this->prev = nullptr;

    // add all the data one by one
    // head, tail and length will be set naturally
    for (Node* cur = list.head; cur != nullptr; cur = cur->next) {
        this->push_back(cur->data);
    }
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::move
    (kernel::utils::LinkedList<DTYPE> &list)
{
    this->length = list.length;
    list.length = 0;

    this->head = list.head;
    list.head = nullptr;

    this->tail = list.tail;
    list.tail = nullptr;

    this->destroy_data_func = list.destroy_data_func;
    list.destroy_data_func = nullptr;

    // iterator interface:

    this->iter = list.iter;
    list.iter = nullptr;

    this->prev = list.prev;
    list.prev = nullptr;
}

//template<typename DTYPE>
//void swap(kernel::utils::LinkedList<DTYPE> &x, kernel::utils::LinkedList<DTYPE> &y) {
template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::swap
    (kernel::utils::LinkedList<DTYPE>& x,
     kernel::utils::LinkedList<DTYPE>& y)
{
    if (&x == &y) return;
    kernel::detail::linked_list::swap(x.length, y.length);
    kernel::detail::linked_list::swap(x.head, y.head);
    kernel::detail::linked_list::swap(x.tail, y.tail);
    kernel::detail::linked_list::swap(x.destroy_data_func, y.destroy_data_func);

    // iterator interface:

    kernel::detail::linked_list::swap(x.iter, y.iter);
    kernel::detail::linked_list::swap(x.prev, y.prev);
}


// ============================
// Destructor Helpers:


template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::clear()
{
    for (Node* cur = this->head, *to_del; cur != nullptr; ) {
        to_del = cur;
        cur = cur->next;
        if (this->destroy_data_func != nullptr) {
//            try {
                this->destroy_data_func(to_del->data);
//            } catch (...) {
//
//            }
        }
        delete to_del;
    }

    this->length = 0;
    this->head = this->tail = nullptr;

    // iterator interface:
    this->iter = this->prev = nullptr;
}


// ============================
// Operator overloads and other friend functions:


template<typename DTYPE>
kernel::utils::LinkedList<DTYPE>&
    kernel::utils::LinkedList<DTYPE>::operator+= (const DTYPE& data)
{
    this->push_back(data);
    return *this;
}

// returns reference to the idx-th node's data
template<typename DTYPE>
DTYPE& kernel::utils::LinkedList<DTYPE>::operator[] (const int idx) {
    return this->getNodeByIdx(idx)->data;
}

// returns reference to the idx-th node's data
template<typename DTYPE>
const DTYPE& kernel::utils::LinkedList<DTYPE>::operator[] (const int idx) const {
    // return (const_cast<kernel::utils::LinkedList<DTYPE>*>(this))->getNodeByIdx(idx)->data;
    return this->getNodeByIdx(idx)->data;
}


// ============================
// Insertion:


template<typename DTYPE>
int kernel::utils::LinkedList<DTYPE>::push_back(const DTYPE &data) {
//    if (-1 == this->appendNode(new Node(data))) {
//        throw LinkedListExceptionPushBack();
//    }
    return this->appendNode(new Node(data));
}

template<typename DTYPE>
int kernel::utils::LinkedList<DTYPE>::push_front(const DTYPE &data) {
    Node *new_node = new Node(data);
    new_node->next = this->head;
    this->head = new_node;

    // update tail if list was previously empty
    if (!this->tail) this->tail = new_node;

    return ++this->length;
}

template<typename DTYPE>
int kernel::utils::LinkedList<DTYPE>::appendNode
    (kernel::utils::LinkedList<DTYPE>::Node *new_node)
{
    if (!new_node) return -1;

    new_node->next = nullptr;

    if (this->head != nullptr) {  // non empty list
        this->tail->next = new_node;
    } else {  // first and only element
        this->head = new_node;
    }

    this->tail = new_node;

    return ++this->length;
}


template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::insert(const int idx,
                                              const DTYPE& data)
{
    if (idx == 0) {  // new first element
        return this->push_front(data) > 0;
    }

    Node *prev = this->getNodeByIdx(idx - 1);
    if (!prev) {
        // const std::string msg = "Invalid idx "
        //                         + std::to_string(idx - 1)
        //                         + " getNodeByIdx returned nullptr.";
        // throw LinkedListCustomException(msg);
        return false;
    }

    return this->insertAfter(prev, data);
}

template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::insertAfter
    (kernel::utils::LinkedList<DTYPE>::Node *prev_node,
     const DTYPE& data)
{
    if (!prev_node || !this->head) {  // new node should be new head
        return this->push_front(data) > 0;
    }

    if (prev_node == this->tail) {  // new node should be new tail
        return this->push_back(data) > 0;
    }

    // list is not empty, new node should not be first
    // nor last so just add as new regular middle node
    Node *new_node = new Node(data);
    new_node->next = prev_node->next;
    prev_node->next = new_node;

    ++this->length;
    return true;
}


// ============================
// Deletion:


template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::pop_front()
{
    if (this->head == nullptr) return nullptr;

    Node *new_head = this->head->next;

    if (this->head->next == nullptr) {  // length is 1
        this->tail = nullptr;
    }

    // delete the head
    if (this->destroy_data_func != nullptr) {
//        try {
            this->destroy_data_func(this->head->data);
//        } catch (...) {
//
//        }
    }
    delete this->head;

    // reset the head
    this->head = new_head;

    this->length -= 1;

    return this->head;
}

template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::pop_back() {
    if (!this->head) return false;

    if (this->head->next == nullptr) {  // length is 1
        this->pop_front();
        return true;
    }

    // get to element before tail
    Node *cur = this->head;
    while (cur->next != this->tail) {
        cur = cur->next;
    }

    // release the tail
    if (this->destroy_data_func != nullptr) {
//        try {
            this->destroy_data_func(this->tail->data);
//        } catch (...) {
//
//        }
    }
    delete this->tail;

    // update the tail
    this->tail = cur;
    this->tail->next = nullptr;

    this->length -= 1;

    return true;
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::erase(const int idx)
{
    this->deleteLinkedListNode(this->getNodeByIdx(idx));
}

template<typename DTYPE>
int kernel::utils::LinkedList<DTYPE>::removeValues(const DTYPE &data) {
    int num_values_removed = 0;
    Node *prev = nullptr;  // first previous node that was not deleted
    for (Node *cur = this->head, *to_del; cur != nullptr; ) {
        if (cur->data == data) {
            ++num_values_removed;
            to_del = cur;
            cur = cur->next;

            if (this->destroy_data_func != nullptr) {
//                try {
                    this->destroy_data_func(to_del->data);
//                } catch (...) {
//
//                }
            }
            delete to_del;

            if (prev) {  // cur is not head
                // unlink the freed node
                prev->next = cur;
            } else {  // removed the head of the linked list
                this->head = cur;
            }
        } else {  // cur node should not be removed
            prev = cur;
            cur = cur->next;
        }
    }
    this->tail = prev;
    // update the length of the list
    this->length -= num_values_removed;
    return num_values_removed;
}

template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::deleteLinkedListNode
        (kernel::utils::LinkedList<DTYPE>::Node *to_delete)
{
    if (!to_delete) return nullptr;

    if (to_delete == this->head) {
        return this->pop_front();
    }

    if (to_delete->next == nullptr) {  // remove the tail
        if (!this->pop_back()) {  // error occurred
            // throw LinkedListCustomException("Failed to pop back");
            //std::cout << "deleteLinkedListNode: Failed to pop back\n";
            return nullptr;
        }
        return nullptr;  // next node after last is nullptr
    }

    // since there is no info on previous node then
    // swap the data with the one in front of node being deleted
    // no need to actually swap/copy the data which will be removed
    if (this->destroy_data_func != nullptr) {
//        try {
            this->destroy_data_func(to_delete->data);
//        } catch (...) {
//
//        }
    }

    // if the tail is the node to be deleted after swap then update it
    if (this->tail == to_delete->next) this->tail = to_delete;
    // swap next node's data
    // to_delete->data = std::move(to_delete->next->data);
    to_delete->data = static_cast<DTYPE &&>(to_delete->next->data);
    Node *tmp = to_delete->next;  // node to actually delete
    to_delete->next = to_delete->next->next;
    // tmp's data ptr has been reassigned so no data freeing should be done
    delete tmp;

    this->length -= 1;

    return to_delete;
}


// ============================
// Lookups:


template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::find
    (const DTYPE& data, kernel::utils::LinkedList<DTYPE>::Node*& prev_)
{
    for (const Node *cur = this->head, *prev = nullptr;
         cur != nullptr;
         cur = cur->next)
    {
        if (cur->data == data) {
            // node with searched data was found
            prev_ = prev;
            return cur;
        }
        prev = cur;
    }
    return nullptr;
}

template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::getNodeByValue(const DTYPE& data)
{
    Node *dummy;
    return this->find(data, dummy);
}

// returns data of the idx-th node
template<typename DTYPE>
DTYPE& kernel::utils::LinkedList<DTYPE>::getDataByIdx(const int idx) {
    return (*this)[idx];
}

// returns data of the idx-th node
template<typename DTYPE>
const DTYPE& kernel::utils::LinkedList<DTYPE>::getDataByIdx(const int idx) const {
    return (*this)[idx];
}

// returns ptr to the idx-th node
template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::getNodeByIdx(const int idx) const
{
    if (!this->isValidIndex(idx)) return nullptr;

    if (idx == this->length - 1) return this->tail;

    Node *cur = this->head;
    for (int i = 0; i < idx; ++i) {
        cur = cur->next;
    }
    return cur;
}


// ============================
// Node Extraction:


template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
    kernel::utils::LinkedList<DTYPE>::extractNode
        (kernel::utils::LinkedList<DTYPE>::Node* node,
         kernel::utils::LinkedList<DTYPE>::Node *prev)
{
    if (!node || !this->head) return nullptr;
    --this->length;

    // removing the head:
    if (node == this->head) {
        this->head = this->head->next;
        // if list had only 1 element set tail to new head
        if (this->tail == node) this->tail = this->head;
        node->next = nullptr;
        return node;
    }

    // removing tail:
    if (node->next == nullptr) {
        // go to new tail and unlink its end
        // if no previous node provided find what preceds the tail
        if (!prev || prev->next != node) {
            for (prev = this->head; prev->next->next != nullptr; prev = prev->next);
        }
        prev->next = nullptr;
        this->tail = prev;
        return node;
    }

    // removing middle node:
    if (prev && prev->next == node) {  // preceding node was provided
        prev->next = node->next;
        node->next = nullptr;
        return node;
    }

    // swap the data with the next node
    Node *swapped_one = node->next;
    //DTYPE tmp = node->data;
    //node->data = swapped_one->data;
    //swapped_one->data = tmp;
    kernel::detail::linked_list::swap(node->data, swapped_one->data);

    // unlink the swapped_one node and update the tail
    // if node to extract was preceding the tail
    if (swapped_one == this->tail) this->tail = node;

    node->next = swapped_one->next;
    swapped_one->next = nullptr;
    return swapped_one;
}

template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::extractByIdx(const int idx)
{
    if (!this->isValidIndex(idx)) return nullptr;

    if (idx == 0) return this->extractNode(this->head);
    Node *prev = this->getNodeByIdx(idx - 1);
    return this->extractNode(prev->next, prev);
}

template<typename DTYPE>
typename kernel::utils::LinkedList<DTYPE>::Node*
kernel::utils::LinkedList<DTYPE>::extractByValue(const DTYPE &data)
{
    Node *prev;
    Node *found = this->find(data, prev);
    if (!found) return nullptr;
    return this->extractNode(found, prev);
}


// ============================
// Getters:


template<typename DTYPE>
int kernel::utils::LinkedList<DTYPE>::size() const {
    return this->length;
}

template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::empty() const {
    return this->head == nullptr;
}


// ============================
// Setters:


template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::setDestroyDataFunction
        (void(* const destroy_data_func)(DTYPE& about_to_be_destroyed))
{
    this->destroy_data_func = destroy_data_func;
}


// ============================
// Misc. Methods


template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::isValidIndex(const int idx) const
{
    if (idx >= 0 && idx < (int) this->length) return true;

    // throw LinkedListExceptionIdxOutOfRange();
    //std::cout << "LinkedList[] idx out of range\n";
    return false;
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::reverse() {
    if (!this->head) return;  // empty list

    // update the tail and reverse the list
    Node *prev = this->tail = this->head;
    for (Node *cur = this->head->next; cur; ) {
        Node *next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    this->head = prev;
    this->tail->next = nullptr;
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::sort(const bool ascending) {
    if (this->length <= 1) return;

    // sort in given order
    kernel::utils::LinkedList<DTYPE>::mergeSort(*this, ascending);

    // update the tail if it is not well positioned
    if (!this->tail || this->tail->next != nullptr) {
        auto cur = this->head;
        for ( ; cur->next != nullptr; cur = cur->next);
        this->tail = cur;
    }
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::sort
    (bool (*cmp_func)(const DTYPE &x, const DTYPE &y))
{
    if (this->length <= 1) return;

    // sort in given order
    kernel::utils::LinkedList<DTYPE>::mergeSort(*this, cmp_func);

    // update the tail if it is not well positioned
    if (!this->tail || this->tail->next != nullptr) {
        auto cur = this->head;
        for ( ; cur->next != nullptr; cur = cur->next);
        this->tail = cur;
    }
}


// ========================================================
// ========================================================
// Iterator:


// ============================
// Moving the iterator:


template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::iter_begin() const
{
    this->iter = this->head;
    this->prev = nullptr;
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::iter_next() const
{
    if (this->iter) {
        this->prev = this->iter;
        this->iter = this->iter->next;
    }
}


// ============================
// Iterator comparisons:


template<typename DTYPE>
bool kernel::utils::LinkedList<DTYPE>::iter_not_end() const
{
    return this->iter != nullptr;
}


// ============================
// Iterator Data Manipulation:


// // returns reference to data in the node which the
// // iter points to, if no such node raises InvalidIterator error
// template<typename DTYPE>
// DTYPE& kernel::utils::LinkedList<DTYPE>::iter_data()
// {
//     if (this->iter) return iter->data;
//     // if iterator points to nullptr raise exception
//     throw LinkedListExceptionInvalidIterator();
// }

// //returns reference to const data in the node which the
// //iter points to, if no such node raises InvalidIterator error
// template<typename DTYPE>
// const DTYPE& kernel::utils::LinkedList<DTYPE>::iter_data() const
// {
//     if (this->iter) return iter->data;
//     // if iterator points to nullptr raise exception
//     throw LinkedListExceptionInvalidIterator();
// }

template<typename DTYPE>
DTYPE* kernel::utils::LinkedList<DTYPE>::iter_data()
{
    if (this->iter) return &iter->data;
    // if iterator points to nullptr raise exception
    // throw LinkedListExceptionInvalidIterator();
    return nullptr;
}

template<typename DTYPE>
const DTYPE* kernel::utils::LinkedList<DTYPE>::iter_data() const
{
    if (this->iter) return &iter->data;
    // if iterator points to nullptr raise exception
    // throw LinkedListExceptionInvalidIterator();
    return nullptr;
}


// ==========================================
// Helper functions of detail namespace:


template<typename DTYPE>
void kernel::detail::linked_list::swap(DTYPE &x, DTYPE &y) {
    DTYPE tmp( static_cast<DTYPE &&> (x) );
    x = static_cast<DTYPE &&> (y);
    y = static_cast<DTYPE &&> (tmp);
}


template<typename SinglyLinkedListNode>
void kernel::detail::linked_list::mergeStep(SinglyLinkedListNode* &new_tail,
                       const int left_size,
                       const int right_size,
                       const bool ascending)
{
    SinglyLinkedListNode *left = new_tail->next;
    SinglyLinkedListNode *right = left;

    // move right to point to first element of second half
    for (int i = 0; i < left_size; ++i) {
        right = right->next;
    }

    int left_added = 0;
    int right_added = 0;
    while (left_added < left_size && right_added < right_size) {
        if ( ascending ? left->data < right->data
                       : right->data < left->data )
        {
            new_tail->next = left;
            new_tail = left;
            left = left->next;
            left_added++;
        } else {
            new_tail->next = right;
            new_tail = right;
            right = right->next;
            right_added++;
        }
    }

    // add the leftovers of left half if any
    for ( ; left_added < left_size; ++left_added) {
        new_tail->next = left;
        new_tail = left;
        left = left->next;
    }

    // add the leftovers of right half if any
    for ( ; right_added < right_size; ++right_added) {
        new_tail->next = right;
        new_tail = right;
        right = right->next;
    }

    // connect to the unsorted rest of the list
    new_tail->next = right;
}

// Implemented in LinkedList.hpp
template<typename SinglyLinkedListNode, typename DTYPE>
void kernel::detail::linked_list::mergeStep
    (SinglyLinkedListNode* &new_tail,
     const int left_size,
     const int right_size,
     bool (*cmp_func)(const DTYPE &x, const DTYPE &y))
{
    SinglyLinkedListNode *left = new_tail->next;
    SinglyLinkedListNode *right = left;

    // move right to point to first element of second half
    for (int i = 0; i < left_size; ++i) {
        right = right->next;
    }

    int left_added = 0;
    int right_added = 0;
    while (left_added < left_size && right_added < right_size) {
        if (cmp_func(left->data, right->data)) {
            // left->data < right->data
            new_tail->next = left;
            new_tail = left;
            left = left->next;
            left_added++;
        } else {  // left->data >= right->data
            new_tail->next = right;
            new_tail = right;
            right = right->next;
            right_added++;
        }
    }

    // add the leftovers of left half if any
    for ( ; left_added < left_size; ++left_added) {
        new_tail->next = left;
        new_tail = left;
        left = left->next;
    }

    // add the leftovers of right half if any
    for ( ; right_added < right_size; ++right_added) {
        new_tail->next = right;
        new_tail = right;
        right = right->next;
    }

    // connect to the unsorted rest of the list
    new_tail->next = right;
}


// ==========================================
// Private Misc Helpers:


template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::mergeSort
    (kernel::utils::LinkedList<DTYPE> &list, const bool ascending)
{
    const int n = list.length;
    if (n <= 1) return;

    auto new_head = new typename
            kernel::utils::LinkedList<DTYPE>::Node(list.head->data);
    new_head->next = list.head;

    for (int halfs_size = 1; halfs_size <= n; halfs_size *= 2) {
        auto new_tail = new_head;

        for (int j = 0, limit = n / (2 * halfs_size); j < limit; ++j) {
            kernel::detail::linked_list::mergeStep(new_tail, halfs_size,
                              halfs_size, ascending);
        }

        // perform on the last segment which is smaller
        // than 2 * halfs_size
        int leftover_size = n % (2 * halfs_size);
        if (leftover_size < halfs_size) continue;
        kernel::detail::linked_list::mergeStep(new_tail, halfs_size,
                          leftover_size - halfs_size, ascending);
        list.tail = new_tail;
    }

    list.head = new_head->next;
    delete new_head;
}

template<typename DTYPE>
void kernel::utils::LinkedList<DTYPE>::mergeSort
    (kernel::utils::LinkedList<DTYPE> &list,
     bool (*cmp_func)(const DTYPE &x, const DTYPE &y))
{
    const int n = list.length;
    if (n <= 1) return;

    auto new_head = new typename kernel::utils::LinkedList<DTYPE>::Node(list.head->data);
    new_head->next = list.head;

    for (int halfs_size = 1; halfs_size <= n; halfs_size *= 2) {
        auto new_tail = new_head;

        for (int j = 0, limit = n / (2 * halfs_size); j < limit; ++j) {
            detail::linked_list::mergeStep(new_tail, halfs_size, halfs_size, cmp_func);
        }

        // perform on the last segment which is smaller
        // than 2 * halfs_size
        int leftover_size = n % (2 * halfs_size);
        if (leftover_size < halfs_size) continue;
        detail::linked_list::mergeStep(new_tail, halfs_size,
                          leftover_size - halfs_size, cmp_func);
        list.tail = new_tail;
    }

    list.head = new_head->next;
    delete new_head;
}


#endif  // LINKED_LIST_HPP
