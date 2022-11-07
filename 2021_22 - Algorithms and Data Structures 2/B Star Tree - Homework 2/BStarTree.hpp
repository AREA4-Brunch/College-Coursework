/*
    B* Tree Template Class Implementation, only keys
    are stored as requested in homework, to store
    associated values then the template param can
    be a key-value pair, e.g. (TKey, TData*) with custom
    comparison operators and then use fetch(TKey&) method to
    retrieve the pair that matched the comparisons.
*/

#ifndef B_STAR_TREE_HPP
#define B_STAR_TREE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <queue>  // for level order


/*
    B* Tree storing only unique keys, no corresponding values.
    Given DTYPE has to have:
        operator==
        operator<
        ostream operator<<
        operator=
        copy/move constructor for std::move

    - Every node but root has at most m subtrees
    - Root has min. 2 and max. 1 + 2*floor((2m - 2) / 3) subtrees
    - Every node but the root and leaves has min. ceil((2m - 1) / 3) subtrees
    - All leaves are on the same level
    - Non leaf node with k subtrees has exactly k keys
    - When deleting leaves have to remain with at least
      ceil((2m - 1) / 3) subtrees to keep existing
*/


// ============================
// B* Tree Exception:


class BStarTreeCustomException : public std::exception {
 private:
    std::string msg = "B* Tree Error Ocurred";
 public:
    BStarTreeCustomException() = default;
    BStarTreeCustomException(const std::string &msg) : msg(msg) {}
    const char* what() const noexcept override { return this->msg.c_str(); }
};



// ============================
// B* Tree and Friend declarations:


template<typename KEY_DTYPE> class BStarTree;

template<typename KEY_DTYPE>
std::ostream& operator<<
(std::ostream& os, const BStarTree<KEY_DTYPE>& self);

template<typename KEY_DTYPE>
std::istream& operator>>
(std::istream& is, BStarTree<KEY_DTYPE>& self);


// ============================
// B* Tree Definition:


template<typename KEY_DTYPE>
class BStarTree
{

 public:

    // ============================
    // Constructors:

    BStarTree(const int m);

    BStarTree(const int m, const std::vector<KEY_DTYPE>& arr_,
              const bool is_sorted=false);

    BStarTree(const int m,
              const std::string& file_path,
              const bool is_sorted=false,
              KEY_DTYPE(*str_to_key_func) (std::string&) = nullptr);

    // ============================
    // Destructor

    ~BStarTree();

    // ============================
    // Constructor helpers:

    void BuildFromInputStream
        (std::istream& in_file,
         const std::string& file_end_flag,
         const bool is_sorted,
         KEY_DTYPE(*str_to_key_func) (std::string&) = nullptr);

    // ============================
    // Destructor helpers:

    void clear();

    // ============================
    // Getters

    int getHeight() const { return this->tree_height; }

    // ============================
    // Lookups:

    bool exists(const KEY_DTYPE &key) const;

    /// @brief Finds the key inside the tree that equals
    ///        the given key and returns reference to it,
    ///        if such exists, else throws an exception
    /// @param key 
    /// @return reference to key in the tree
    KEY_DTYPE& fetch(const KEY_DTYPE &key);

    /// @brief Finds the key inside the tree that equals
    ///        the given key and returns reference to it,
    ///        if such exists, else throws an exception
    /// @param key 
    /// @return const reference to key in the tree
    const KEY_DTYPE& fetch(const KEY_DTYPE &key) const;

    // ============================
    // Modifiers:

    bool insertKey(const KEY_DTYPE &key);
    bool deleteKey(const KEY_DTYPE &key);

    // ============================
    // Misc. methods:

    KEY_DTYPE& getKthSmallestKey(int k) const;


 private:

    class Node {
     public:
        Node* parent = nullptr;
        std::vector<typename BStarTree<KEY_DTYPE>::Node*> children;
        std::vector<KEY_DTYPE> keys;  // sorted

        // Constructors:
        Node() {};
        Node(const KEY_DTYPE& key);

        // Destructor
        ~Node() {}

        // Insertion:
        void insertKeyToLeaf(const int idx, const KEY_DTYPE& key);

        void insertKey(const int idx, const KEY_DTYPE& key,
                       typename BStarTree<KEY_DTYPE>::Node* subtree=nullptr);

        void addKeyToLeaf(const KEY_DTYPE& key);
        void removeKeyFromLeaf(const int key_idx);
        void removeKey(const int key_idx);

        // Misc.
        bool isLeaf() const;

        // Retrieving siblings:
        int getSubtreeIdx();

        Node* getRightSibling();
        Node* getLeftSibling();
        Node* getRightSibling(const int idx);
        Node* getLeftSibling(const int idx);

     private:

        // Constructor and insertion helper
        void constructFromKey(const KEY_DTYPE &key); 
        void displayKeys(std::ostream &os) const;
        void displayKeysDebug(std::ostream &os) const;

        friend std::ostream& operator<<
            (std::ostream& os, const BStarTree<KEY_DTYPE>::Node& self)
        {
            self.displayKeys(os);
            //self.displayKeysDebug(os);
            return os;
        }
    };


    // ============================
    // Private Attributes:

    Node *root = nullptr;
    int m;  // as in m-ary tree

    int tree_height = 0;  // num of lvls in the tree

    // max chars used to display a node
    const int max_node_chars = 12;

    // flags used in further implementation
    static const Node lvl_end_node_sentinel, empty_node_sentinel;


    // ============================
    // Constructor helpers:

    void setM(const int m);

    void BuildFromArray(const std::vector<KEY_DTYPE>& arr_,
        const bool is_sorted=false);


    // ============================
    // Traversals:

    std::queue<typename BStarTree<KEY_DTYPE>::Node*>
        getLevelOrderQueue(int &height) const;


    // ============================
    // Lookups:

    typename BStarTree<KEY_DTYPE>::Node*
        find(const KEY_DTYPE &key,
             typename BStarTree<KEY_DTYPE>::Node* &prev,
             int &subtree_idx) const;


    // ============================
    // Insertion

    void insertKeyIntoLeafNode(typename BStarTree<KEY_DTYPE>::Node* node,
                               const int subtree_idx,
                               const KEY_DTYPE &key);

    // ============================
    // Deletion

    void removeKeyFromNode( typename BStarTree<KEY_DTYPE>::Node* node,
                            int subtree_idx, int key_idx);

    // ============================
    // Splitting:

    void splitRoot();
    void twoThreeSplit(typename BStarTree<KEY_DTYPE>::Node* left,
                       typename BStarTree<KEY_DTYPE>::Node* right,
                       const int sep_key_idx);

    // ============================
    // Merging:

    typename BStarTree<KEY_DTYPE>::Node*
        threeTwoMerge(typename BStarTree<KEY_DTYPE>::Node* middle,
                      const int middle_subtree_idx);
    
    typename BStarTree<KEY_DTYPE>::Node*
        twoOneMerge(typename BStarTree<KEY_DTYPE>::Node* source,
                    typename BStarTree<KEY_DTYPE>::Node* destination,
                    const int sep_key_idx, const bool is_dest_right);


    // ============================
    // Redistributing (prelivanje)

    void redistribute(typename BStarTree<KEY_DTYPE>::Node* left,
                      typename BStarTree<KEY_DTYPE>::Node* right,
                      const int sep_key_idx);


    // ============================
    // Misc.

    int calcTreeHeight() const;
    bool isNodeFull(typename BStarTree<KEY_DTYPE>::Node* node) const;
    bool exceedsCapacity(typename BStarTree<KEY_DTYPE>::Node* node) const;
    bool isNodeTooSmall(typename BStarTree<KEY_DTYPE>::Node* node) const;
    bool isEnoughKeysNonRoot(const int num_keys) const;


    // ============================
    // Friends and family ;)

    friend std::ostream& operator<< <KEY_DTYPE>
        (std::ostream& os, const BStarTree<KEY_DTYPE> &self);

    friend std::istream& operator>> <KEY_DTYPE>
        (std::istream& is, BStarTree<KEY_DTYPE> &self);

};



// ============================
// ============================
// Implementation:


#include <fstream>  // loading data in tree from file
#include <stack>  // for in order
#include <algorithm>  // std::sort
#include <typeinfo>  // typeid
#include <unordered_map>
#include <math.h>  // for ceil


// #include "BStarTree.h"


// ============================
// String comparison:


namespace detail {
    
    // moves everything from first vector into second, does not
    // resize the first vector
    template<typename T>
    void embedVector(std::vector<T> &source, std::vector<T> &destination) {
        destination.insert(destination.end(),
                           std::make_move_iterator(source.begin()),
                           std::make_move_iterator(source.end()) );
    }

    template<typename CONT>
    void displayContainer(const CONT &cont) {
        for (auto const& el : cont) {
            std::cout << el << ", ";
        }
        std::cout << std::endl;
    }

    // inline bool operator< (const std::string& x, const std::string& y) {
    //     return (x.compare(y) < 0);
    // }

    // inline bool operator== (const std::string& x, const std::string& y) {
    //     return (x.compare(y) == 0);
    // }

    // inline bool operator!= (const std::string& x, const std::string& y) {
    //     return (x.compare(y) != 0);
    // }

    // inline bool operator> (const std::string& x, const std::string& y) {
    //     return (x.compare(y) > 0);
    // }

    // inline bool operator<= (const std::string& x, const std::string& y) {
    //     return !(x > y);
    // }

    // inline bool operator>= (const std::string& x, const std::string& y) {
    //     return !(x < y);
    // }

}  // end of detail namespace


// ============================
// Initialize static flags:


template<typename KEY_DTYPE>
const typename BStarTree<KEY_DTYPE>::Node
BStarTree<KEY_DTYPE>::lvl_end_node_sentinel;

template<typename KEY_DTYPE>
const typename BStarTree<KEY_DTYPE>::Node
BStarTree<KEY_DTYPE>::empty_node_sentinel;


// ============================
// Macros for smoother coding:


#define LVL_END_NODE_PTR ((Node*) &this->lvl_end_node_sentinel)
#define EMPTY_NODE_PTR ((Node*) &this->empty_node_sentinel)


// ============================
// ============================
// Constructors:


template<typename KEY_DTYPE>
BStarTree<KEY_DTYPE>::BStarTree(const int max_num_subtrees)
    : root(nullptr)
{
    this->setM(max_num_subtrees);
}

// Constructor that builds complete tree out of the given array
// of key value pairs, copies given array in case it is unsorted and then
// sorts it
template<typename KEY_DTYPE>
BStarTree<KEY_DTYPE>::BStarTree
(const int max_num_subtrees, const std::vector<KEY_DTYPE>& arr,
 const bool is_sorted)
    : root(nullptr)
{
    this->setM(max_num_subtrees);
    this->BuildFromArray(arr, is_sorted);
}

// Builds BST from a file, if no key/value conversion functions are provided
// key and value will be of type std::string, accept for int, float, double
// where stoi, stof and stod will be used
// Assumes < operand is defined for keys and values
// File should have keys and values line after the other
template<typename KEY_DTYPE>
BStarTree<KEY_DTYPE>::BStarTree
(const int max_num_subtrees, const std::string& file_path, const bool is_sorted,
    KEY_DTYPE(*str_to_key_func) (std::string&))
    : root(nullptr)
{
    std::ifstream in_file(file_path.c_str());
    if (!in_file.is_open()) {
        throw std::ios_base::failure("BST constructor was unable to read the file");
    }

    this->setM(max_num_subtrees);

    std::string file_end_flag = "__end";
    this->BuildFromInputStream(in_file, file_end_flag, is_sorted,
        str_to_key_func);

    in_file.close();
}



// ============================
// Destructor:


// Destructor of the tree, deletes all the keys and data with it
template<typename KEY_DTYPE>
BStarTree<KEY_DTYPE>::~BStarTree() {
    this->clear();
}

template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::clear() {
    int num_levels = -1;
    std::queue<typename BStarTree<KEY_DTYPE>::Node*> nodes
        = this->getLevelOrderQueue(num_levels);

    for (Node* to_del; !nodes.empty(); ) {
        to_del = nodes.front();
        nodes.pop();
        // children of to_del have already been deleted
        if (to_del != LVL_END_NODE_PTR) delete to_del;
        // else to_del was just a level end indicator
    }

    this->tree_height = 0;
    this->root = nullptr;
}


// ============================
// Constructor Helpers:


template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::setM(const int max_num_subtrees) {
    if (max_num_subtrees < 3 || max_num_subtrees > 10) {
        throw BStarTreeCustomException("m has to be in range: [3, 10]");
    }
    this->m = max_num_subtrees;
}

// does not close given ifstream
// Builds BST from a file, if no key/value conversion functions are provided
// key and value will be of type std::string, accept for int, float, double
// where stoi, stof and stod will be used
// Assumes operator< is defined for keys and values
// File should have keys and values line after the other
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::BuildFromInputStream
(std::istream& in_file, const std::string& file_end_flag,
 const bool is_sorted, KEY_DTYPE(*str_to_key_func) (std::string&))
{
    std::vector<KEY_DTYPE> arr;

    // load in line by line
    // std::pair<KEY_DTYPE, VAL_DTYPE> cur;
    KEY_DTYPE cur;

    for (std::string cur_line;
         std::getline(in_file, cur_line) && cur_line.compare(file_end_flag) != 0;
        )
    {
        // convert and add key
        if (str_to_key_func) cur = str_to_key_func(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(std::string)) cur = cur_line;
        else if (typeid(KEY_DTYPE) == typeid(int)) cur = std::stoi(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(float)) cur = std::stof(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(double)) cur = std::stod(cur_line);
        arr.emplace_back(cur);
    }

    // sort here in-place to avoid using a copy in BuildFromArray
    // if (!is_sorted) std::sort(arr.begin(), arr.end());
    // this->BuildFromSortedArray(arr);
    this->BuildFromArray(arr, is_sorted);
}

// just adds elements one by one, does not take advantage of array being sorted
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::BuildFromArray
    (const std::vector<KEY_DTYPE>& arr, const bool is_sorted)
{
    for (const auto& key : arr) {
        this->insertKey(key);
    }
}


// ============================
// Lookups:


// returns ptr to the node with the given key and modifies
// given ptr to point to prev node than the one returned and
// if no node with given key exists returns nullptr and
// updates given idx to the idx-th subtree
template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node*
BStarTree<KEY_DTYPE>::find(const KEY_DTYPE &key,
                           typename BStarTree<KEY_DTYPE>::Node* &prev,
                           int &subtree_idx) const
{
    prev = nullptr;
    for (Node *cur = this->root; cur != nullptr; ) {
        auto it = std::lower_bound(cur->keys.begin(), cur->keys.end(), key);
        // cur node contains given key
        if (it != cur->keys.end() && *it == key) return cur;
        subtree_idx = it - cur->keys.begin();
        prev = cur;
        cur = cur->children[subtree_idx];
    }

    return nullptr;
}

// returns true if there is a node with a given key, false otherwise
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::exists(const KEY_DTYPE &key) const
{
    Node *dummy;
    int dummy2;
    return nullptr != this->find(key, dummy, dummy2);
}

template<typename KEY_DTYPE>
KEY_DTYPE& BStarTree<KEY_DTYPE>::fetch(const KEY_DTYPE &key) {
    for (Node *cur = this->root; cur != nullptr; ) {
        auto it = std::lower_bound(cur->keys.begin(), cur->keys.end(), key);
        // cur node contains given key
        if (it != cur->keys.end() && *it == key) return *it;
        int subtree_idx = it - cur->keys.begin();
        cur = cur->children[subtree_idx];
    }

    throw BStarTreeCustomException("Given key does not exist in the tree");
}

template<typename KEY_DTYPE>
const KEY_DTYPE& BStarTree<KEY_DTYPE>::fetch(const KEY_DTYPE &key) const {
    for (Node *cur = this->root; cur != nullptr; ) {
        auto it = std::lower_bound(cur->keys.begin(), cur->keys.end(), key);
        // cur node contains given key
        if (it != cur->keys.end() && *it == key) return *it;
        int subtree_idx = it - cur->keys.begin();
        cur = cur->children[subtree_idx];
    }

    throw BStarTreeCustomException("Given key does not exist in the tree");
}


// ============================
// Modifiers:


// inserts given key if it does not exist and returns true,
// just returns false otherwise
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::insertKey(const KEY_DTYPE& key) {
    if (!this->root) {  // add to the empty tree
        this->root = new Node(key);
        this->tree_height = 1;
        return true;
    }

    // find where to insert given key
    Node *node;
    int subtree_idx = -1;
    Node *found = this->find(key, node, subtree_idx);

    // if node with given key already exists just return false
    if (found != nullptr) return false;

    this->insertKeyIntoLeafNode(node, subtree_idx, key);
    return true;
}

// inserts given key into given node at given position
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::
 insertKeyIntoLeafNode(typename BStarTree<KEY_DTYPE>::Node* node,
                       const int subtree_idx,
                       const KEY_DTYPE &key)
{
    if (!this->isNodeFull(node)) {  // there is space so just add it
        node->insertKeyToLeaf(subtree_idx, key);
        return;
    }
    
    node->insertKey(subtree_idx, key, nullptr);

    do {

        if (node == this->root) {
            this->splitRoot();
            return;
        }

        // try to redistribute rather than split if there
        // is space in right or left sibling
        int node_idx = node->getSubtreeIdx();

        Node *right = node->getRightSibling(node_idx);
        if (!this->isNodeFull(right)) {  // redistribute with right sibling
            this->redistribute(node, right, node_idx);
            return;
        }

        // check the left sibling for redistributing
        Node *left = node->getLeftSibling(node_idx);
        if (!this->isNodeFull(left)) {  // redistribute with left sibling
            this->redistribute(left, node, node_idx - 1);
            return;
        }

        // splitting is unavoidable, so do it with right sibling
        // if any, otherwise with the left one
        if (right != nullptr) {  // split with right sibling
            this->twoThreeSplit(node, right, node_idx);
        }
        else {  // split with left sibling
            this->twoThreeSplit(left, node, node_idx - 1);
        }

        // with every split the tree grows in height by 1
        ++this->tree_height;

        // in case after splitting the parent is greater than it
        // is allowed to be then retry one of prev techniques
        // until it is not
        node = node->parent;

    } while(this->exceedsCapacity(node));

    return;
}

// removes the key from the tree if it exists and returns true
// otherwise returns false
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::deleteKey(const KEY_DTYPE& key) {
    Node *found, *prev;
    int subtree_idx = -1;

    found = this->find(key, prev, subtree_idx);
    
    // if the node with given key does not exist just return false
    if (!found) return false;

    auto it = std::lower_bound(found->keys.begin(),
                               found->keys.end(), key);
    int key_idx = it - found->keys.begin();
    this->removeKeyFromNode(found, subtree_idx, key_idx);

    return true;
}

// removes the key at given position from the given node,
// subtree_idx is node's idx among subtrees of its parent
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::
 removeKeyFromNode(typename BStarTree<KEY_DTYPE>::Node* node,
                   int subtree_idx, int key_idx)
{
    if (!node->isLeaf()) {  // has subtrees
        Node *successor = node->children[key_idx + 1];
        subtree_idx = key_idx + 1;
        while (successor->children[0] != nullptr) {
            successor = successor->children[0];
            subtree_idx = 0;
        }
        // swap the keys with the leaf node
        //std::swap(node->keys[key_idx], successor->keys[0]);
        // since data is being delete no need for actual swap
        node->keys[key_idx] = std::move(successor->keys[0]);

        // update the node and the key to delete
        key_idx = 0;
        node = successor;
    }

    node->removeKeyFromLeaf(key_idx);

    // if the leaf has enough keys keep it as is
    if (!this->isNodeTooSmall(node)) return;

    do {
        if (node == this->root) {
            // the root does not have enough keys (it is empty)
            // so delete it and update the root ptr
            Node *to_del = this->root;
            this->root = this->root->children[0];
            delete to_del;
            --this->tree_height;
            return;
        }

        // recalculate node's idx in parent's subtrees
        subtree_idx = node->getSubtreeIdx();

        // otherwise try to redistribute keys from siblings to it
        // first try with right sibling then with left, then
        // right's right and then left's left

        Node *right = node->getRightSibling(subtree_idx);
        if (right && this->isEnoughKeysNonRoot(right->keys.size() - 1)) {
            // can redistribute through right sibling

            // move from parent to end of this node
            node->keys.emplace_back(std::move(node->parent->keys[subtree_idx]));
            // move left subtree of the key that is about to go
            // up as a separator
            node->children.emplace_back(right->children[0]);
            if (right->children[0]) right->children[0]->parent = node;

            // move first el from right sibling into the parent
            node->parent->keys[subtree_idx] = std::move(right->keys[0]);
            
            right->removeKey(0);

            return;
        }

        Node *left = node->getLeftSibling(subtree_idx);
        if (left && this->isEnoughKeysNonRoot(left->keys.size() - 1)) {
            // can redistribute through left sibling

            // move from parent to end of this node
            node->keys.insert(node->keys.begin(),
                              std::move(node->parent->keys[subtree_idx - 1]));
            // move left subtree of the key that is about to go
            // up as a separator
            node->children.insert(node->children.begin(), left->children.back());
            if (left->children.back()) (left->children.back())->parent = node;

            // move first el from left sibling into the parent
            node->parent->keys[subtree_idx - 1] = std::move(left->keys.back());
            left->keys.pop_back();
            left->children.pop_back();

            return;
        }

        // check the second right sibling from the same parent
        Node *right_right = node->getRightSibling(subtree_idx + 1);
        if (right_right && this->isEnoughKeysNonRoot(right_right->keys.size() - 1)) {
            // can redistribute through right's right sibling

            // move from parent to end of this node
            node->keys.emplace_back(std::move(node->parent->keys[subtree_idx]));
            node->children.emplace_back(right->children[0]);
            if (right->children[0]) right->children[0]->parent = node;

            // move first el from right sibling into the parent
            node->parent->keys[subtree_idx] = std::move(right->keys[0]);
            right->removeKey(0);

            right->insertKey(right->keys.size(),
                             std::move(node->parent->keys[subtree_idx + 1]),
                             right_right->children[0]);
            
            node->parent->keys[subtree_idx + 1] = std::move(right_right->keys[0]);

            right_right->removeKey(0);

            return;
        }

        // check the second left sibling from the same parent
        Node *left_left = node->getRightSibling(subtree_idx + 1);
        if (left_left && this->isEnoughKeysNonRoot(left_left->keys.size() - 1)) {
            // can redistribute through left's left sibling

            // move from parent to end of this node
            node->keys.insert(node->keys.begin(),
                              std::move(node->parent->keys[subtree_idx - 1]));
            node->children.insert(node->children.begin(), left->children.back());
            if (left->children.back()) (left->children.back())->parent = node;

            // move first el from left sibling into the parent
            node->parent->keys[subtree_idx - 1] = std::move(left->keys.back());
            left->keys.pop_back();
            left->children.pop_back();

            // add second separator to the left node
            left->insertKey(0,
                            std::move(node->parent->keys[subtree_idx - 2]),
                            left_left->children.back());
            
            node->parent->keys[subtree_idx - 2] = std::move(left_left->keys.back());

            left_left->removeKey(left_left->keys.size() - 1);

            return;
        }

        // Merging is unavoidable:

        // in case there exists just 1 sibling and
        // then there is just 1 key in the parent to move:
        if (!left && !right_right) {  // there only exists right sibling
            // merge into right sibling
            node = twoOneMerge(node, right, subtree_idx, true);
        }
        else if (!right && !left_left) {  // there only exists left sibling
            // merge into left sibling
            node = twoOneMerge(node, left, subtree_idx - 1, false);
        }
        // there exist 2 siblings:
        else if (!left) {  // node, right, right_right
            node = this->threeTwoMerge(right, subtree_idx + 1);
        }
        else if (!right) {  // left_left, left, node
            node = this->threeTwoMerge(left, subtree_idx - 1);
        }
        else {  // left, node, right
            node = this->threeTwoMerge(node, subtree_idx);
        }

    } while (this->isNodeTooSmall(node));

    // update the tree height after merging
    // height decreases only when root gets deleted
    //this->tree_height = this->calcTreeHeight();
    
    return;
}


// ============================
// Splitting:


// splits the root which blew its capacity
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::splitRoot()
{
    // middle which goes up as a new root
    int middle = (this->root->keys.size() - 1) / 2;

    Node *new_root = new Node(std::move(this->root->keys[middle]));
    Node *left = this->root;
    Node *right = new Node();
    // move right half from left to right
    right->keys.reserve(left->keys.size() - middle - 1);
    right->keys.insert(right->keys.end(),
                       std::make_move_iterator(left->keys.begin() + middle + 1),
                       std::make_move_iterator(left->keys.end()) );

    // remove what went into right
    left->keys.resize(middle);

    // move the right half of subtrees 
    right->children.reserve(left->keys.size() - middle);
    right->children.insert(
        right->children.end(),
        std::make_move_iterator(left->children.begin() + middle + 1),
        std::make_move_iterator(left->children.end())
    );

    // remove what went into right
    left->children.resize(middle + 1);

    for (auto& child : right->children) {
        if (child) child->parent = right;
    }

    for (auto& child : left->children) {
        if (child) child->parent = left;
    }

    this->root = new_root;
    this->root->parent = nullptr;
    this->root->children[0] = left;
    this->root->children[1] = right;
    left->parent = this->root;
    right->parent = this->root;
}

// assumes given nodes are not empty
// builds 3rd node and rearranges all the keys and subtrees
// adds additional key to the parent at position 1 + sep_key_idx
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::
    twoThreeSplit(typename BStarTree<KEY_DTYPE>::Node* left,
                  typename BStarTree<KEY_DTYPE>::Node* right,
                  const int sep_key_idx)
{
    // left + right + separator
    int num_keys = left->keys.size() + right->keys.size() + 1;
    // num of keys per node
    int left_size = (2 * this->m - 2) / 3;
    int middle_size = (2 * this->m - 1) / 3;
    int right_size = (2 * this->m) / 3;

    std::vector<KEY_DTYPE> merged;
    merged.reserve(num_keys);
    // add the left
    detail::embedVector(left->keys, merged);
    // add the current middle
    merged.emplace_back(left->parent->keys[sep_key_idx]);
    // add the right
    detail::embedVector(right->keys, merged);

    // resize each node:
    left->keys.resize(left_size);
    right->keys.resize(right_size);

    std::vector<typename BStarTree<KEY_DTYPE>::Node*> merged_subtrees;
    merged_subtrees.reserve(num_keys + 1);
    // add the left
    detail::embedVector(left->children, merged_subtrees);
    // add the right
    detail::embedVector(right->children, merged_subtrees);
    
    // resize each node:
    left->children.resize(left_size + 1);
    right->children.resize(right_size + 1);

    // Distribute the keys over all 3 nodes:

    // Move all the elements into left
    int merged_ptr = 0;
    int merged_sbtr_ptr = 0;

    for (int i = 0; i < left_size; ++i) {
        left->keys[i] = std::move(merged[merged_ptr++]);
        left->children[i] = merged_subtrees[merged_sbtr_ptr++];
        // update parent of the child if child exists
        if (left->children[i]) left->children[i]->parent = left;
    }

    // send up one key
    left->parent->keys[sep_key_idx] = std::move(merged[merged_ptr++]);
    // add the last subtree of left
    left->children[left_size] = merged_subtrees[merged_sbtr_ptr++];
    if (left->children[left_size]) left->children[left_size]->parent = left;

    // create the middle node
    Node *middle = new Node();
    middle->parent = left->parent;

    middle->keys.resize(middle_size);
    middle->children.resize(middle_size + 1);

    // add to middle node
    for (int i = 0; i < middle_size; ++i) {
        middle->keys[i] = std::move(merged[merged_ptr++]);
        middle->children[i] = merged_subtrees[merged_sbtr_ptr++];
        if (middle->children[i]) middle->children[i]->parent = middle;
    }

    // add the last subtree of middle
    middle->children[middle_size] = merged_subtrees[merged_sbtr_ptr++];
    if (middle->children[middle_size])
        middle->children[middle_size]->parent = middle;

    // key to add to the parent that might cause a split of the parent
    left->parent->insertKey(sep_key_idx + 1, merged[merged_ptr++], middle);

    // add to right node
    for (int i = 0; i < right_size; ++i) {
        right->keys[i] = std::move(merged[merged_ptr++]);
        right->children[i] = merged_subtrees[merged_sbtr_ptr++];
        if (right->children[i]) right->children[i]->parent = right;
    }

    // add the last subtree of right
    right->children[right_size] = merged_subtrees[merged_sbtr_ptr++];
    if (right->children[right_size])
        right->children[right_size]->parent = right;

    //return this->isNodeFull(left->parent);
}


// ============================
// Merging:


// redistributes the keys and subtrees of given nodes so their parent
// deletes the middle node and returns the parent node
template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node* BStarTree<KEY_DTYPE>::
    threeTwoMerge(typename BStarTree<KEY_DTYPE>::Node* middle,
                  const int middle_subtree_idx)
{
    Node *left = middle->parent->children[middle_subtree_idx - 1];
    Node *right = middle->parent->children[middle_subtree_idx + 1];

    const int num_keys = left->keys.size()
                       + middle->keys.size()
                       + right->keys.size()
                       + 2;  // for 2 separator keys in parent
    
    const int middle_idx = (num_keys - 1) / 2;

    std::vector<KEY_DTYPE> merged;
    merged.reserve(num_keys);

    // add the left
    detail::embedVector(left->keys, merged);
    // add separator between left and middle
    merged.emplace_back(left->parent->keys[middle_subtree_idx - 1]);
    // add the middle
    detail::embedVector(middle->keys, merged);
    // add separator between middle and right
    merged.emplace_back(left->parent->keys[middle_subtree_idx]);
    // add the right
    detail::embedVector(right->keys, merged);
    
    // resize left and right
    left->keys.resize(middle_idx);
    right->keys.resize(num_keys - middle_idx - 1);
    middle->keys.clear();

    std::vector<typename BStarTree<KEY_DTYPE>::Node*> merged_subtrees;
    merged_subtrees.reserve(num_keys + 1);
    // add the left
    detail::embedVector(left->children, merged_subtrees);
    // add the middle
    detail::embedVector(middle->children, merged_subtrees);
    // add the right
    detail::embedVector(right->children, merged_subtrees);

    // resize left and right and destroy middle
    left->children.resize(middle_idx + 1);
    right->children.resize(num_keys - middle_idx);
    delete middle;

    // set the middle el as a separator
    left->parent->keys[middle_subtree_idx - 1] = std::move(merged[middle_idx]);
    // remove the second separator from the parent and
    // subtree on its right does not need an update afterwards
    left->parent->removeKey(middle_subtree_idx);

    // Distribute all the keys over the 2 nodes
    int merged_ptr = 0;
    int merged_sbtr_ptr = 0;

    // add to left node
    for (int i = 0; i < middle_idx; ++i) {
        left->keys[i] = std::move(merged[merged_ptr++]);
        left->children[i] = merged_subtrees[merged_sbtr_ptr++];
        // update parent pointer of the subtree
        if (left->children[i]) left->children[i]->parent = left;
    }

    // add the last subtree of left
    left->children[middle_idx] = merged_subtrees[merged_sbtr_ptr++];
    if (left->children[middle_idx])
        left->children[middle_idx]->parent = left;

    merged_ptr++;  // skip the separator in the parent

    // add to right node
    for (int i = 0, n = num_keys - middle_idx - 1; i < n; ++i) {
        right->keys[i] = std::move(merged[merged_ptr++]);
        right->children[i] = merged_subtrees[merged_sbtr_ptr++];
        // update parent pointer of the subtree
        if (right->children[i]) right->children[i]->parent = right;
    }

    // add the last subtree of right
    right->children[middle_idx] = merged_subtrees[merged_sbtr_ptr++];
    if (right->children[num_keys - middle_idx - 1])
        right->children[num_keys - middle_idx - 1]->parent = right;

    return left->parent;
}

// redistributes the keys from source, separator key from parent
// into the destination node, deletes source and returns their parent
// given bool arg indicates whether keys and subtrees will be
// prepended or appended
template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node* BStarTree<KEY_DTYPE>::
    twoOneMerge(typename BStarTree<KEY_DTYPE>::Node* source,
                typename BStarTree<KEY_DTYPE>::Node* dest,
                const int sep_key_idx,
                const bool is_dest_right)
{
    const int num_keys = source->keys.size()
                       + dest->keys.size()
                       + 1;  // 1 for separator key
    // add all the keys into destination node
    dest->keys.reserve(num_keys);
    dest->children.reserve(num_keys + 1);

    if (is_dest_right) {
        // move the separator to end of keys to be prepended
        source->keys.emplace_back(std::move(dest->parent->keys[sep_key_idx]));
    }
    else {
        // add the separator before you add the keys of right node
        dest->keys.emplace_back(std::move(dest->parent->keys[sep_key_idx]));
    }
    
    auto insert_keys_it = is_dest_right ?
                          dest->keys.begin() : dest->keys.end();
    int insert_sbtrs_idx = is_dest_right ?
                            0 : dest->children.size();

    dest->keys.insert(insert_keys_it,
                      std::make_move_iterator(source->keys.begin()),
                      std::make_move_iterator(source->keys.end()) );
    
    dest->children.insert(dest->children.begin() + insert_sbtrs_idx,
                          std::make_move_iterator(source->children.begin()),
                          std::make_move_iterator(source->children.end()) );

    // update added children's parents:
    for (int i = 0, n = source->children.size(); i < n; ++i) {
        if (dest->children[insert_sbtrs_idx]) {
            dest->children[insert_sbtrs_idx]->parent = dest;
        }
        ++insert_sbtrs_idx;
    }

    // remove the source now that is has been emptied out
    delete source;

    // remove the separator from the parent
    dest->parent->removeKey(sep_key_idx);

    // if the left node is what remains update the subtree pointer
    if (!is_dest_right) {
        dest->parent->children[sep_key_idx] = dest;
    }


    return dest->parent;
}


// ============================
// Redistributing:


// assumes given nodes are not empty
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::
    redistribute(typename BStarTree<KEY_DTYPE>::Node* left,
                 typename BStarTree<KEY_DTYPE>::Node* right,
                 const int sep_key_idx)
{
    // left + right + separator
    const int num_keys = left->keys.size() + right->keys.size() + 1;
    const int middle = num_keys / 2;  // idx of the key to go up

    std::vector<KEY_DTYPE> merged;
    merged.reserve(num_keys);
    // add the left
    detail::embedVector(left->keys, merged);
    // add the current middle
    merged.emplace_back(left->parent->keys[sep_key_idx]);
    // add the right
    detail::embedVector(right->keys, merged);

    std::vector<typename BStarTree<KEY_DTYPE>::Node*> merged_subtrees;
    merged_subtrees.reserve(num_keys + 1);
    // add the left
    detail::embedVector(left->children, merged_subtrees);
    // add the right
    detail::embedVector(right->children, merged_subtrees);

    left->keys.resize(middle);
    right->keys.resize(num_keys - middle - 1);
    left->children.resize(middle + 1);
    right->children.resize(num_keys - middle);

    int merged_ptr = 0;
    int merged_sbtrs_ptr = 0;
    // reconstruct left
    for (int i = 0; merged_ptr < middle; ++i) {
        left->keys[i] = std::move(merged[merged_ptr++]);
        left->children[i] = merged_subtrees[merged_sbtrs_ptr++];
        if (left->children[i]) left->children[i]->parent = left;
    }

    // set the middle
    left->parent->keys[sep_key_idx] = std::move(merged[merged_ptr++]);
    // add the last subtree of left node
    left->children[middle] = merged_subtrees[merged_sbtrs_ptr++];
    if (left->children[middle]) left->children[middle]->parent = left;

    // reconstruct right
    for (int i = 0; merged_ptr < num_keys; ++i) {
        right->keys[i] = std::move(merged[merged_ptr++]);
        right->children[i] = merged_subtrees[merged_sbtrs_ptr++];
        if (right->children[i]) right->children[i]->parent = right;
    }
    // add the last subtree of right node
    right->children.back() = merged_subtrees[merged_sbtrs_ptr++];
    if (right->children.back()) right->children.back()->parent = right;
}


// ============================
// Tree traversals:


// returns queue of ptrs to nodes in the tree in lvl order and
// modifies given argument to the height of the tree
// queue contains LVL_END_NODE_PTR flags for the end of each lvl
// if tree is empty returns empty queue
template<typename KEY_DTYPE>
std::queue<typename BStarTree<KEY_DTYPE>::Node*>
BStarTree<KEY_DTYPE>::getLevelOrderQueue(int &height) const
{
    std::queue<typename BStarTree<KEY_DTYPE>::Node*>
        to_process, lvl_order;

    height = 0;

    if (!this->root) {
        return lvl_order;
    }

    to_process.push(LVL_END_NODE_PTR);
    to_process.push(this->root);

    for (Node* cur; ; ) {
        cur = to_process.front();
        to_process.pop();
        lvl_order.push(cur);

        if (cur != LVL_END_NODE_PTR) {
            // add all the existing children
            for (auto &child : cur->children) {
                if (child) to_process.push(child);
            }
        }
        else if (to_process.empty()) {
            // there are no nodes left to process but
            // the end lvl flag
            return lvl_order;
        }
        else {
            // end of lvl was reached and there are
            // more nodes left (therefore in next lvl)
            height++;
            to_process.push(LVL_END_NODE_PTR);
        }
    }

    // this should never be reached
    return lvl_order;
}


// ============================
// Misc. methods:


// 0-indexed
template<typename KEY_DTYPE>
KEY_DTYPE& BStarTree<KEY_DTYPE>::getKthSmallestKey(int k) const {
    if (k < 0) {
        std::string msg = "Can't get Kth Smallest Key. k < 0 ???";
        throw BStarTreeCustomException(msg);
    }

    // to_process.front() = (idx of last processed subtree in cur, cur)
    std::stack<std::pair<int, typename BStarTree<KEY_DTYPE>::Node*>> to_process;
    to_process.push({-1, this->root});

    while (!to_process.empty()) {
        int key_to_process = to_process.top().first;
        Node *cur = to_process.top().second;

        to_process.pop();

        if (key_to_process >= 0) {
            if (key_to_process >= cur->keys.size()) continue;

            if (k == 0) return cur->keys[key_to_process];
            k--;  // this key has been processed
            if (k < 0) break;  // k > num of keys in this tree

            // set to revisit the node but on next key
            to_process.push({1 + key_to_process, cur});
            // but firstly visit in order the subtree
            if (cur->children[1 + key_to_process] != nullptr) {
                to_process.push({-1, cur->children[1 + key_to_process]});
            }
        }
        else {  // delay the processing of cur and go left if exists
            to_process.push({1 + key_to_process, cur});
            if (cur->children.size() && cur->children[0] != nullptr) {
                to_process.push({-1, cur->children[0]});
            }
        }
    }

    std::string msg = "Can't get Kth Smallest Key. k >= num of keys in the tree.";
    throw BStarTreeCustomException(msg);
}

// returns cur height of the tree (1-indexed)
template<typename KEY_DTYPE>
int BStarTree<KEY_DTYPE>::calcTreeHeight() const {
    std::queue<typename BStarTree<KEY_DTYPE>::Node*> to_process;

    int height = 0;

    if (!this->root) {
        return 0;
    }

    to_process.push(LVL_END_NODE_PTR);
    to_process.push(this->root);

    for (Node* cur; ; ) {
        cur = to_process.front();
        to_process.pop();

        if (cur != LVL_END_NODE_PTR) {
            // add all the existing children
            for (auto &child : cur->children) {
                if (child) to_process.push(child);
            }
        }
        else if (to_process.empty()) {
            // there are no nodes left to process but
            // the end lvl flag
            return height;
        }
        else {
            // end of lvl was reached and there are
            // more nodes left (therefore in next lvl)
            height++;
            to_process.push(LVL_END_NODE_PTR);
        }
    }

    // this should never be reached
    return height;
}

// returns if given node does not have more room for keys,
// in case given nodes is nullptr, returns true
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::
    isNodeFull(typename BStarTree<KEY_DTYPE>::Node* node) const
{
    if (!node) return true;
    if (node == this->root) {
        return node->keys.size() >= 2 * ((2 * this->m - 2) / 3);
    }

    return node->keys.size() >= this->m - 1;
}

// returns if given node does not have more room for keys,
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::
    exceedsCapacity(typename BStarTree<KEY_DTYPE>::Node* node) const
{
    if (node == this->root) {
        return node->keys.size() > 2 * ((2 * this->m - 2) / 3);
    }

    return node->keys.size() > this->m - 1;
}

// returns if given has less keys than it has to,
// in case given nodes is nullptr, returns true
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::
    isNodeTooSmall(typename BStarTree<KEY_DTYPE>::Node* node) const
{
    if (!node) return true;
    if (node == this->root) {
        return node->keys.size() < 1;
    }
    return false == this->isEnoughKeysNonRoot(node->keys.size());
}

// returns whether given num of keys is enough for the the
// non root node to exist
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::
    isEnoughKeysNonRoot(const int num_keys) const
{
    return 1. * (num_keys + 1) >= std::ceil((2 * this->m - 1) / 3.);
}


// ============================
// Friends and operator overloads:


// Display tree, level order, each node prefixed with `Xp/Y: `,
// where X is the idx of the parent from prev layer and Y is the
// index of the node in the current layer
template<typename KEY_DTYPE>
std::ostream& operator<< (std::ostream& os, const BStarTree<KEY_DTYPE>& self)
{
    if (!self.root) return os << "Empty tree\n";

    std::queue<std::pair<int, typename BStarTree<KEY_DTYPE>::Node*>> to_process;

    int height = 0;

    to_process.push({-1, nullptr});
    to_process.push({0, self.root});

    for (int node_idx = 0; ; ) {
        auto cur = to_process.front();
        to_process.pop();

        if (cur.first >= 0) {  // processing a valid node
                               // log self
            os << cur.first << "p/" << node_idx << ": " << *(cur.second) << " ";

            // add all the existing children
            for (auto &child : (cur.second)->children) {
                if (child) to_process.push({node_idx, child});
            }
            node_idx++;
        }
        else if (to_process.empty()) {
            // there are no nodes left to process but
            // the end lvl flag
            return os << std::endl;
        }
        else {
            // end of lvl was reached and there are
            // more nodes left (therefore in next lvl)
            to_process.push({-1, nullptr});
            os << std::endl << "Lvl " << height++ << ":   ";
            node_idx = 0;
        }
    }

    // should not be reached
    return os;
}

template<typename KEY_DTYPE>
std::istream& operator>> (std::istream& is, BStarTree<KEY_DTYPE>& self) {
    self.BuildFromInputStream(is, "", false);
    return is;
}


// ============================
// ============================
// Node definitions:


// ============================
// Constructors:

template<typename KEY_DTYPE>
BStarTree<KEY_DTYPE>::Node::Node(const KEY_DTYPE& key)
    : parent(nullptr)
{
    this->constructFromKey(key);
}

template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::constructFromKey(const KEY_DTYPE &key) {
    this->keys.emplace_back(key);
    this->children.emplace_back(nullptr);
    this->children.emplace_back(nullptr);
}


// ============================
// Insertion of keys:


template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::
    insertKeyToLeaf(const int idx, const KEY_DTYPE& key)
{
    if (!this->keys.size()) {  // adding to an empty leaf
        this->constructFromKey(key);
        return;
    }

    this->keys.insert(this->keys.begin() + idx, key);
    this->children.emplace_back(nullptr);
}

// inserts given key into given position as a given subtree
// as its right subtree
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::
    insertKey(const int idx, const KEY_DTYPE& key,
              typename BStarTree<KEY_DTYPE>::Node* subtree)
{
    if (!this->keys.size()) {  // adding to an empty leaf
        this->constructFromKey(key);
        this->children.back() = subtree;
        return;
    }

    this->keys.insert(this->keys.begin() + idx, key);
    this->children.insert(this->children.begin() + idx, subtree);
    if (subtree) subtree->parent = this;
}

template<typename KEY_DTYPE>
// inserts a given key, maintains the non-decreasing sorted order
void BStarTree<KEY_DTYPE>::Node::addKeyToLeaf(const KEY_DTYPE& key) {
    if (!this->keys.size()) {  // adding to an empty leaf
        this->constructFromKey(key);
        return;
    }

    auto it = std::upper_bound(this->keys.begin(), this->keys.end(), key);
    int idx = it - this->keys.begin();
    this->keys.insert(it, key);
    // this->children.insert(idx + this->children.begin(), nullptr);
    this->children.emplace_back(nullptr);
}

template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::removeKeyFromLeaf(const int key_idx)
{
    this->keys.erase(this->keys.begin() + key_idx);
    this->children.pop_back();
}

// removes idx-th key and removes left subtree of the deleted key 
template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::removeKey(const int key_idx) {
    this->keys.erase(this->keys.begin() + key_idx);
    this->children.erase(this->children.begin() + key_idx);
}

// returns whether it has any children but does not
// differentiate between leaf and root with no children
template<typename KEY_DTYPE>
bool BStarTree<KEY_DTYPE>::Node::isLeaf() const {
    for (auto &child : this->children) {
        if (child != nullptr) return false;
    }
    return true;
}


// ============================
// Retrieving siblings:


template<typename KEY_DTYPE>
int BStarTree<KEY_DTYPE>::Node::getSubtreeIdx() {
    for (int i = 0, n = this->parent->children.size();
        i < n;
        ++i)
    {
        if (this == this->parent->children[i]) return i;
    }
}

template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node*
    BStarTree<KEY_DTYPE>::Node::getRightSibling()
{
    for (int i  = 0, n = this->parent->children.size();
        i < n - 1;
        ++i)
    {
        if (this == this->parent->children[i]) {
            return this->parent->children[i + 1];
        }
    }
    return nullptr;
}

template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node*
    BStarTree<KEY_DTYPE>::Node::getLeftSibling()
{
    Node *prev = nullptr;
    for (auto &child : this->parent->children) {
        if (this == child) return prev;
        prev = child;
    }
    return prev;
}

template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node*
    BStarTree<KEY_DTYPE>::Node::getRightSibling(const int idx)
{
    if (idx < 0 || idx >= this->parent->children.size() - 1)
        return nullptr;
    return this->parent->children[idx + 1];
}

template<typename KEY_DTYPE>
typename BStarTree<KEY_DTYPE>::Node*
    BStarTree<KEY_DTYPE>::Node::getLeftSibling(const int idx)
{
    if (idx <= 0) return nullptr;
    return this->parent->children[idx - 1];
}


// ============================
// Logging the node:


template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::displayKeys(std::ostream &os) const {
    os << "|";
    // log all but last element
    for (int i = 0, n = this->keys.size() - 1; i < n; ++i) {
        os << this->keys[i] << " | ";
    }
    // add the last element and close off with the `|`
    os << this->keys.back() << "|";
}

template<typename KEY_DTYPE>
void BStarTree<KEY_DTYPE>::Node::displayKeysDebug(std::ostream &os) const {
    os << "(" << this->parent << "p/" << this << ") ";
    os << this->children[0] << "*|";
    // log all but last element
    for (int i = 0, n = this->keys.size() - 1; i < n; ++i) {
        os << this->keys[i] << " |*" << this->children[i + 1] << "*|";
    }
    // add the last element and close off with the `|`
    os << this->keys.back() << "|*" << this->children.back();
}



// ============================
// Remove macros used just for the code:


#ifdef LVL_END_NODE_PTR
#undef LVL_END_NODE_PTR
#endif

#ifdef EMPTY_NODE_PTR
#undef EMPTY_NODE_PTR
#endif


#endif // !"B_STAR_TREE_HPP"
