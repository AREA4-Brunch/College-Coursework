#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>  // for level order
#include <stack>  // for level order
#include <algorithm>  // std::sort
#include <stdexcept>  // raising errors
#include <tuple>
#include <typeinfo>  // typeid
#include <unordered_map>


// ============================
// Macros for smoother coding:


#define LVL_END_NODE_PTR ((Node*) &this->lvl_end_node_sentinel)
#define EMPTY_NODE_PTR ((Node*) &this->empty_node_sentinel)


// ============================
// String comparison:


// namespace {
//     inline bool operator< (const std::string& x, const std::string& y) {
//         return (x.compare(y) < 0);
//     }

//     inline bool operator== (const std::string& x, const std::string& y) {
//         return (x.compare(y) == 0);
//     }

//     inline bool operator!= (const std::string& x, const std::string& y) {
//         return (x.compare(y) != 0);
//     }

//     inline bool operator> (const std::string& x, const std::string& y) {
//         return (x.compare(y) > 0);
//     }

//     inline bool operator<= (const std::string& x, const std::string& y) {
//         return !(x > y);
//     }

//     inline bool operator>= (const std::string& x, const std::string& y) {
//         return !(x < y);
//     }
// }


// ============================
// Binary Search Tree:


template<typename KEY_DTYPE, typename VAL_DTYPE>
class BinarySearchTree;

template<typename KEY_DTYPE, typename VAL_DTYPE>
std::ostream& operator<<
(std::ostream& os, const BinarySearchTree<KEY_DTYPE, VAL_DTYPE>& self);


template<typename KEY_DTYPE, typename VAL_DTYPE>
class BinarySearchTree {

 private:


    // Binary search tree node
    class Node {

     public:
        Node* parent;
        Node* left;
        Node* right;

        KEY_DTYPE key;
        VAL_DTYPE data;

        // Constructors:
        Node() : parent(nullptr), left(nullptr), right(nullptr) {}

        // sets key, adds value to array of values
        // sets parent, left, right to nullptr
        Node(const std::pair<KEY_DTYPE, VAL_DTYPE>& key_val) {
            this->key = key_val.first;
            this->addValue(key_val.second);
            this->parent = this->left = this->right = nullptr;
        }

        // Destructor
        ~Node() {}

        // inline void addValue(const VAL_DTYPE &data) {
        //     this->data.emplace_back(data);
        // }
        inline void addValue(const VAL_DTYPE& data) {
            this->data = data;
        }

     private:

        void displayData(std::ostream& os) const {
            os << "(" << this->data << ")";
        }

        friend std::ostream& operator<<
            (std::ostream& os, const BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node& self)
        {
            os << self.key;
            self.displayData(os);
            return os;
        }
    };


    // ============================
    // Attributes:

    // Root Node of the trees
    Node* root = nullptr;

    // flags used in further implementation
    static const Node lvl_end_node_sentinel, empty_node_sentinel;

    const int max_node_chars = 12;  // max chars used to display a node


    // ============================
    // Level Orders:

    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>
        getLevelOrderQueue() const;

    std::vector<std::vector<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>>
        getLevelsMatrixFullTree() const;

    int calcTreeHeight() const;


    // ============================
    // Displaying:

    friend std::ostream& operator<< <KEY_DTYPE, VAL_DTYPE>
        (std::ostream& os, const BinarySearchTree<KEY_DTYPE, VAL_DTYPE>& self);


 public:

    int tree_height;  // 1-indexed


    // ============================
    // Constructors:

    BinarySearchTree() : tree_height(0), root(nullptr) {}

    BinarySearchTree(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr_,
        const bool is_sorted);

    BinarySearchTree(const std::string& file_path, const bool is_sorted,
        KEY_DTYPE(*str_to_key_func) (std::string&) = nullptr,
        VAL_DTYPE(*str_to_val_func) (std::string&) = nullptr);

    // ============================
    // constructor helpers

    void BuildFromSortedArray(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr);

    void BuildFromArray(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr_,
        const bool is_sorted);

    void BuildFromInputStream
    (std::istream& in_file, const std::string& file_end_flag, const bool is_sorted,
        KEY_DTYPE(*str_to_key_func) (std::string&) = nullptr,
        VAL_DTYPE(*str_to_val_func) (std::string&) = nullptr);

    // ============================
    // Destructor

    ~BinarySearchTree();

    // ============================
    // Lookups:

    int findKey(const KEY_DTYPE& key_to_del,
                Node*& found_node,
                Node*& found_nodes_parent) const;

    // bool getValue(const KEY_DTYPE &key,
    //         decltype(BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node::data)* &found_values) const;
    bool getValue(const KEY_DTYPE& key, std::vector<VAL_DTYPE*>& found_values) const;

    void logAllKeysPrefixNodes(const std::string& prefix) const;
    KEY_DTYPE getMostFrequentKey() const;

    // ============================
    // Modifiers:

    void insertKeyValuePair(const std::pair<KEY_DTYPE, VAL_DTYPE>& key_val);

    bool deleteNode
        (BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* cur,
         BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* cur_parent,
         BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* &replacement);
    
    bool deleteAllKeys(const KEY_DTYPE& key_to_del);

};


// ============================
// Initialize static flags:


template<typename KEY_DTYPE, typename VAL_DTYPE>
const typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::lvl_end_node_sentinel;

template<typename KEY_DTYPE, typename VAL_DTYPE>
const typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::empty_node_sentinel;



// ============================
// BST Implementation:



// Constructor that builds complete tree out of the given array
// of key value pairs, copies given array in case it is unsorted and then
// sorts it
template<typename KEY_DTYPE, typename VAL_DTYPE>
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BinarySearchTree
(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr, const bool is_sorted) {
    this->BuildFromArray(arr, is_sorted);
}


// Builds BST from a file, if no key/value conversion functions are provided
// key and value will be of type std::string, accept for int, float, double
// where stoi, stof and stod will be used
// Assumes < operand is defined for keys and values
// File should have keys and values line after the other
template<typename KEY_DTYPE, typename VAL_DTYPE>
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BinarySearchTree
(const std::string& file_path, const bool is_sorted,
    KEY_DTYPE(*str_to_key_func) (std::string&),
    VAL_DTYPE(*str_to_val_func) (std::string&))
{
    std::ifstream in_file(file_path.c_str());
    if (!in_file.is_open()) {
        throw std::ios_base::failure("BST constructor was unable to read the file");
    }

    std::string file_end_flag = "__end";
    this->BuildFromInputStream(in_file, file_end_flag, is_sorted,
        str_to_key_func, str_to_val_func);

    in_file.close();
}


// does not close given ifstream
// Builds BST from a file, if no key/value conversion functions are provided
// key and value will be of type std::string, accept for int, float, double
// where stoi, stof and stod will be used
// Assumes < operand is defined for keys and values
// File should have keys and values line after the other
template<typename KEY_DTYPE, typename VAL_DTYPE>
void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BuildFromInputStream
(std::istream& in_file, const std::string& file_end_flag, const bool is_sorted,
    KEY_DTYPE(*str_to_key_func) (std::string&),
    VAL_DTYPE(*str_to_val_func) (std::string&))
{
    std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>> arr;

    // load in line by line
    std::pair<KEY_DTYPE, VAL_DTYPE> cur;

    for (std::string cur_line; std::getline(in_file, cur_line) && cur_line.compare(file_end_flag) != 0; ) {
        // convert and add key
        if (str_to_key_func) cur.first = str_to_key_func(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(std::string)) cur.first = cur_line;
        else if (typeid(KEY_DTYPE) == typeid(int)) cur.first = std::stoi(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(float)) cur.first = std::stof(cur_line);
        else if (typeid(KEY_DTYPE) == typeid(double)) cur.first = std::stod(cur_line);
        // else cur.first = cur_line;  // keep it string
        // load in the value from the next line if it's not EOF
        if (std::getline(in_file, cur_line) && cur_line.compare(file_end_flag) != 0) {
            // convert and add value
            if (str_to_val_func) cur.second = str_to_val_func(cur_line);
            else if (typeid(VAL_DTYPE) == typeid(std::string)) cur.second = cur_line;
            else if (typeid(VAL_DTYPE) == typeid(int)) cur.second = std::stoi(cur_line);
            else if (typeid(VAL_DTYPE) == typeid(float)) cur.second = std::stof(cur_line);
            else if (typeid(VAL_DTYPE) == typeid(double)) cur.second = std::stod(cur_line);
            // else cur.second = cur_line;
            arr.emplace_back(cur);
        }
        else {
            printf("Last key did not have a value pair. Dismissing that key..\n\n");
            break;
        }
    }

    // sort here in-place to avoid using a copy in BuildFromArray
    // if (!is_sorted) std::sort(arr.begin(), arr.end());

    // this->BuildFromSortedArray(arr);
    this->BuildFromArray(arr, is_sorted);
}


// Destructor of the tree, deletes all the keys and data with it
template<typename KEY_DTYPE, typename VAL_DTYPE>
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::~BinarySearchTree() {
    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> nodes = this->getLevelOrderQueue();
    for (Node* to_del; !nodes.empty(); ) {
        to_del = nodes.front();
        nodes.pop();
        if (to_del != LVL_END_NODE_PTR) delete to_del;  // its children have already been deleted
        // else to_del==nullptr and was just a level end indicator
    }
}


// artificial constructor from key-value pair std::vector which
// is sorted in non-decreasing order, values sharing a key will
// share a same node in the tree. Both keys and values are deep copies.
template<typename KEY_DTYPE, typename VAL_DTYPE>
void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BuildFromSortedArray
(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr)
{
    if (!arr.size()) {
        this->tree_height = 0;
        this->root = nullptr;
        return;
    }

    // keep track of all keys added append values from pairs
    // sharing the same key
    // std::unordered_map<KEY_DTYPE, typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>
    //     exists;

    // keep track of local left roots
    std::stack<std::tuple<int, int, typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>>
        left_roots;

    int middle = arr.size() / 2;
    this->root = new Node(arr[middle]);
    left_roots.push(std::make_tuple(middle + 1, arr.size() - 1, this->root));
    // exists[arr[middle].first] = this->root;


    // add all leftmost nodes left of main root
    Node* cur = this->root;
    for (int start = 0, end = arr.size() / 2 - 1; start <= end; ) {
        middle = (start + end) / 2;
        // if (!exists.count(arr[middle].first)) {  // new key
        //     cur->left = new Node(arr[middle]);
        //     cur->left->parent = cur;
        //     cur = cur->left;
        //     exists[arr[middle].first] = cur;
        // } else {  // this key already exists so just append value
        //     exists[arr[middle].first]->addValue(arr[middle].second);
        // }
        cur->left = new Node(arr[middle]);
        cur->left->parent = cur;
        cur = cur->left;
        left_roots.push(std::make_tuple(middle + 1, end, cur));
        end = middle - 1;
    }

    // add all right nodes to previosuly added ones and
    // perform the same processing as above to those until
    // there are no more right nodes to create
    for (int start, end; !left_roots.empty(); ) {
        std::tie(start, end, cur) = left_roots.top();
        left_roots.pop();

        if (start > end) continue;

        // add the right node to the local root
        middle = (start + end) / 2;
        // if (!exists.count(arr[middle].first)) {  // new key
        //     cur->right = new Node(arr[middle]);
        //     cur->right->parent = cur;
        //     cur = cur->right;
        //     exists[arr[middle].first] = cur;
        // } else {  // this key already exists so just append value
        //     exists[arr[middle].first]->addValue(arr[middle].second);
        // }
        cur->right = new Node(arr[middle]);
        cur->right->parent = cur;
        cur = cur->right;
        left_roots.push(std::make_tuple(middle + 1, end, cur));

        // go through all leftmost nodes of cur's right subtree
        for (end = middle - 1; start <= end; ) {
            middle = (start + end) / 2;
            // if (!exists.count(arr[middle].first)) {  // new key
            //     cur->left = new Node(arr[middle]);
            //     cur->left->parent = cur;
            //     cur = cur->left;
            //     exists[arr[middle].first] = cur;
            // } else {  // this key already exists so just append value
            //     exists[arr[middle].first]->addValue(arr[middle].second);
            // }
            cur->left = new Node(arr[middle]);
            cur->left->parent = cur;
            cur = cur->left;
            left_roots.push(std::make_tuple(middle + 1, end, cur));
            end = middle - 1;
        }
    }

    this->tree_height = this->calcTreeHeight();
}


// creates the binary tree using BuildFromSortedArray and
// if the given array is not labeled as sorted it makes a copy and
// sorts it first using std::sort from <algorithms>
// template<typename KEY_DTYPE, typename VAL_DTYPE>
// void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BuildFromArray
// (const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>> &arr_, const bool is_sorted) {
//     std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>> *arr = &arr_;
//     if (!is_sorted) {
//         // make a copy of an array and sort it
//         arr = new std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>(arr_);
//         std::sort(arr->begin(), arr->end());
//     }

//     this->BuildFromSortedArray(*arr);

//     // free allocated memory for the copy of original array in case
//     // it was not sorted
//     if (!is_sorted) {
//         delete arr;
//     }
// }


template<typename KEY_DTYPE, typename VAL_DTYPE>
void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::BuildFromArray
(const std::vector<std::pair<KEY_DTYPE, VAL_DTYPE>>& arr, const bool is_sorted) {
    for (const auto& kv : arr) {
        this->insertKeyValuePair(kv);
    }
}


// returns level(0-indexed) at which the node with given key should be in,
// sets given found_node ptr to found node (nullptr in case there is no key)
// and sets found_nodes_parent to its parent
template<typename KEY_DTYPE, typename VAL_DTYPE>
int BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::findKey(const KEY_DTYPE& key,
    Node*& found_node,
    Node*& found_nodes_parent) const
{
    int level = 0;
    found_nodes_parent = nullptr;
    found_node = this->root;

    for (; found_node != nullptr && found_node->key != key; ++level) {
        found_nodes_parent = found_node;
        if (key < found_node->key) {
            found_node = found_node->left;
        }
        else {
            found_node = found_node->right;
        }
    }

    return level;
}


// returns true if the given key exists, false otherwise
// modifies given reference ptr in case of successful to point to found value/data
// search, in current implementation found_values is a vector of values
// corresponding to given key
// template<typename KEY_DTYPE, typename VAL_DTYPE>
// bool BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::getValue
// (const KEY_DTYPE &key,
// decltype(BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node::data)* &found_values) const
// {
//     Node *found_node, *dummy;
//     this->findKey(key, found_node, dummy);
//     if (!found_node) {
//         // given key was not found in the tree
//         return false;
//     }

//     found_values = &(found_node->data);
//     return true;
// }
template<typename KEY_DTYPE, typename VAL_DTYPE>
bool BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::getValue
(const KEY_DTYPE& key, std::vector<VAL_DTYPE*>& found_values) const
{
    Node* found_node, * dummy;
    this->findKey(key, found_node, dummy);
    if (!found_node) {
        // given key was not found in the tree
        return false;
    }

    found_values.emplace_back(&found_node->data);

    // keep going left until done with all copies of
    // this key:
    for (found_node = found_node->left;
        found_node != nullptr && found_node->key == key;
        found_node = found_node->left)
    {
        found_values.emplace_back(&found_node->data);
    }

    return true;
}


// inserts given value into the tree based on given key and updates tree height
// builds copies of key(if it's its 1st occurance) and a copy of the value
template<typename KEY_DTYPE, typename VAL_DTYPE>
void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::insertKeyValuePair
(const std::pair<KEY_DTYPE, VAL_DTYPE>& key_val)
{
    if (!this->root) {
        this->root = new Node(key_val);
        this->tree_height = 1;
        return;
    }

    Node* found_node;
    Node* found_nodes_parent;
    int level = this->findKey(key_val.first, found_node, found_nodes_parent);
    // if (found_node) {
    //     // key already exists so just add the value to it
    //     found_node->addValue(key_val.second);
    //     return;
    // }

    if (found_node) {  // key already exists
        if (!found_node->left) {
            found_node->left = new Node(key_val);
            found_node->left->parent = found_node;
        }
        else {
            Node* new_node = new Node(key_val);
            new_node->parent = found_node;
            new_node->left = found_node->left;
            found_node->left->parent = new_node;
            found_node->left = new_node;
        }
        // make new node a successor fo an old one by swapping their values
        // not keys since they are equivalent:
        VAL_DTYPE tmp = found_node->data;
        found_node->data = found_node->left->data;
        found_node->left->data = tmp;

        // update the height of the tree
        this->tree_height = this->calcTreeHeight();
        return;
    }

    // construct a new key
    Node* new_node = new Node(key_val);
    new_node->parent = found_nodes_parent;
    // set new_node as its parents left/right child
    if (found_nodes_parent != nullptr) {
        if (key_val.first < found_nodes_parent->key) {
            found_nodes_parent->left = new_node;
        }
        else {
            found_nodes_parent->right = new_node;
        }
    }
    // update the height of the tree
    this->tree_height = std::max(this->tree_height, 1 + level);
}


// deletes `cur` from the tree and
// sets `replacement` node to node that replaced the current one, nullptr if node
// did not have any children
// returns true on success, false otherwise and then replacement is not modified
template<typename KEY_DTYPE, typename VAL_DTYPE>
bool BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::deleteNode
(BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* cur,
 BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* cur_parent,
 BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node* &replacement)
{
    if (!this->root) return false;

    if (!cur) {
        // KEY WAS NOT FOUND
        return false;
    }

    // replace the node to delete with the next in-order value
    // in case it has just 1 child then just replace it with that child

    replacement = nullptr;
    if (!cur->left) {
        replacement = cur->right;
    }
    else if (!cur->right) {
        replacement = cur->left;
    }
    else {
        // find next in-order value in subtree
        // (leftest node in right subtree)
        Node* replacement_parent = cur;
        replacement = cur->right;

        while (replacement->left != nullptr) {
            replacement_parent = replacement;
            replacement = replacement->left;
        }

        if (cur != replacement_parent) {
            // replacement != cur->right and
            // instead there was at least one node left of cur->right
            replacement_parent->left = replacement->right;
            replacement->right = cur->right;
        }

        replacement->left = cur->left;
    }

    if (!cur_parent) {  // deleted the root
        this->root = replacement;
    }
    else if (cur_parent->left == cur) {
        cur_parent->left = replacement;
    }
    else {
        cur_parent->right = replacement;
    }
    if (replacement) replacement->parent = cur_parent;
    //this->tree_height = this->calcTreeHeight();

    delete cur;
    return true;
}


// returns true on success, false otherwise
template<typename KEY_DTYPE, typename VAL_DTYPE>
bool BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::deleteAllKeys(const KEY_DTYPE& key_to_del) {
    if (!this->root) return false;
    //if (!this->deleteNode(key_to_del)) return false;
    //while (this->deleteNode(key_to_del));
    //return true;

    Node* cur = this->root;
    Node* cur_parent = nullptr;

    // cur is the node to delete
    int level = this->findKey(key_to_del, cur, cur_parent);
    if (!cur) {
        // Key does not exist
        return false;
    }

    for (Node* replacement = nullptr; cur != nullptr && cur->key == key_to_del; ) {
        if (!this->deleteNode(cur, cur_parent, replacement)) break;
        cur = replacement;
        if (!cur) break;

        // in case the deleted node had a right subtree then a node
        // from that subtree took its place so the possible duplicate
        // key will be left of that node that took its place
        if (cur->key != key_to_del) {
            cur_parent = cur;
            cur = cur->left;
        }
    }

    this->tree_height = this->calcTreeHeight();
    return true;
}


// returns std::queue of pointers sorted by Level order
template<typename KEY_DTYPE, typename VAL_DTYPE>
std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::getLevelOrderQueue() const
{
    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> level_order, to_process;
    if (!this->root) return level_order;

    to_process.push(this->root);
    for (Node* cur; !to_process.empty(); ) {
        cur = to_process.front();
        to_process.pop();

        level_order.push(cur);
        if (cur->left) to_process.push(cur->left);
        if (cur->right) to_process.push(cur->right);
    }

    return level_order;
}


// returns cur height of the tree (1-indexed)
template<typename KEY_DTYPE, typename VAL_DTYPE>
int BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::calcTreeHeight() const {
    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> to_process;

    if (!this->root) return 0;

    to_process.push(LVL_END_NODE_PTR);
    to_process.push(this->root);
    int height = 0;
    for (Node* cur; ; ) {
        cur = to_process.front();
        to_process.pop();

        if (cur != LVL_END_NODE_PTR) {
            if (cur->left) to_process.push(cur->left);
            if (cur->right) to_process.push(cur->right);
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


// returns vector<vector<Node*>> where each row is a full level
// (EMPTY_NODE_PTRs for empty space)
template<typename KEY_DTYPE, typename VAL_DTYPE>
std::vector<std::vector<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>>
BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::getLevelsMatrixFullTree() const
{
    std::vector<std::vector<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>> levels = {};

    if (!this->root) return levels;

    std::vector<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> empty_vct = {};
    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> to_process;

    to_process.push(LVL_END_NODE_PTR);
    to_process.push(this->root);

    Node* cur;
    for (int max_lvls = this->tree_height; ; ) {
        cur = to_process.front();
        to_process.pop();

        if (cur == LVL_END_NODE_PTR) {  // end of level reached
            if (levels.size() >= max_lvls) break;
            levels.emplace_back(empty_vct);
            to_process.push(LVL_END_NODE_PTR);
            continue;
        }

        levels.back().emplace_back(cur);

        if (cur != EMPTY_NODE_PTR) {
            if (cur->left) to_process.push(cur->left);
            else to_process.push(EMPTY_NODE_PTR);

            if (cur->right) to_process.push(cur->right);
            else to_process.push(EMPTY_NODE_PTR);
        }
        else {  // add empty space
            to_process.push(EMPTY_NODE_PTR);
            to_process.push(EMPTY_NODE_PTR);
        }
    }

    return levels;
}


// Display tree, level order with trimmed output
// (max self.max_node_chars chars per node display)
template<typename KEY_DTYPE, typename VAL_DTYPE>
std::ostream& operator<<
(std::ostream& os, const BinarySearchTree<KEY_DTYPE, VAL_DTYPE>& self)
{
    if (!self.root) return os << "nullptr\n";

    std::vector<std::vector<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>>
        levels = self.getLevelsMatrixFullTree();

    const int max_lvls = levels.size();
    const int node_offset = 4;  // in chars
    const int max_nodes = 1 << (max_lvls - 1);
    // in chars:
    const int buffer_space_needed = max_nodes * self.max_node_chars
        + (max_nodes - 1) * node_offset;
    for (int lvl = 0, lvl_start = buffer_space_needed >> 1,
        center_offset = (self.max_node_chars + node_offset) << (max_lvls - 1);
        lvl < max_lvls;
        ++lvl, center_offset >>= 1, lvl_start >>= 1)
    {  // log current level:
        os << "lvl " << lvl << ": ";

        // lvl_start is center of first node of cur lvl
        // log the space upto current start
        for (int i = lvl_start - self.max_node_chars / 2; i > 0; --i) os << " ";

        // log each node and the space after it
        for (int cur_node_start_idx = lvl_start - self.max_node_chars / 2, cur_node_idx = 0;
            cur_node_start_idx < buffer_space_needed;
            cur_node_start_idx += center_offset, ++cur_node_idx)
        {  // log node by node:
            // log the current node in exaclty self.max_node_chars:
            std::stringstream ss;
            // log the node if it was not a dummy one, otherwise just
            // replace it with whitespace
            if (levels[lvl][cur_node_idx] != &(self.empty_node_sentinel))
                ss << *levels[lvl][cur_node_idx];
            std::string node_string = ss.str().substr(0, self.max_node_chars);
            // fill up to max_node_chars from both sides almost equally
            std::stringstream final_ss;
            int left_padding = (self.max_node_chars - node_string.length()) / 2;
            int right_padding = (self.max_node_chars - node_string.length()) - left_padding;
            for (int i = left_padding; i > 0; --i) final_ss << " ";
            // while (node_string.length() < self.max_node_chars) node_string += " ";
            final_ss << node_string;
            for (int i = right_padding; i > 0; --i) final_ss << " ";
            // else: in case originally there were more then trim them
            node_string = final_ss.str();
            os << node_string;

            // add space upto next node's start
            for (int i = center_offset - self.max_node_chars; i > 0; --i) os << " ";
        }
        // start next level:
        os << "\n\n";
    }

    return os;
}


template<typename KEY_DTYPE, typename VAL_DTYPE>
void BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::logAllKeysPrefixNodes
(const std::string& prefix) const
{
    if (typeid(KEY_DTYPE) != typeid(std::string)) {
        printf("This option is unavailable for key types other than std::string\n");
        return;
    }

    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*> to_process;
    if (!this->root) {
        printf("Tree is empty\n");
        return;
    }

    printf("Found prefix nodes:\n");

    to_process.push(this->root);
    for (Node* cur; !to_process.empty(); ) {
        cur = to_process.front();
        to_process.pop();

        // if cur->key contains a given prefix
        if (cur->key.rfind(prefix, 0) == 0) {
            // cur node should be logged:
            std::cout << *cur << "\n";
            // add cur's children since they both can have a prefix
            if (cur->left) to_process.push(cur->left);
            if (cur->right) to_process.push(cur->right);
        }
        else if (prefix < cur->key) {  // prefix is on the left
            if (cur->left) to_process.push(cur->left);
        }
        else {  // prefix is on the right
            if (cur->right) to_process.push(cur->right);
        }
    }

    printf("End of found prefix nodes\n");
}


template<typename KEY_DTYPE, typename VAL_DTYPE>
KEY_DTYPE BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::getMostFrequentKey() const {
    KEY_DTYPE max_key;

    if (!this->root) {
        // printf("Tree does not contain any keys.\n");
        return max_key;
    }

    std::unordered_map<KEY_DTYPE, int> freqs;
    std::queue<typename BinarySearchTree<KEY_DTYPE, VAL_DTYPE>::Node*>
        nodes = this->getLevelOrderQueue();

    for (Node* cur; !nodes.empty(); ) {
        cur = nodes.front();
        nodes.pop();
        if (cur != LVL_END_NODE_PTR) {
            if (freqs.count(cur->key)) {
                ++freqs[cur->key];
            }
            else {
                freqs[cur->key] = 1;
            }
        }
    }

    int max_freq = 0;
    for (const auto& kv : freqs) {
        if (kv.second > max_freq) {
            max_freq = kv.second;
            max_key = kv.first;
        }
    }
    return max_key;
}


// ============================
// Remove macros used just for the code:


#ifdef LVL_END_NODE_PTR
#undef LVL_END_NODE_PTR
#endif

#ifdef EMPTY_NODE_PTR
#undef EMPTY_NODE_PTR
#endif


#endif // !"BINARY_SEARCH_TREE_HPP"
