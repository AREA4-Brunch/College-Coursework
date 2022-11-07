#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


// Helper Functions:


template<typename CONT>
void displayCont(const CONT &cont, std::ostream &os = std::cout) {
    for (auto const &el : cont) {
        os << el << ", ";
    }
    os << std::endl;
}


// Sorting Functions:

void insertionSort(std::vector<int> &arr);
void shellSort(std::vector<int> &arr);

void selectionSort(std::vector<int> &arr);

void bubbleSort(std::vector<int> &arr);
void bubbleSortBest(std::vector<int> &arr);
void bubbleSortSimplified(std::vector<int> &arr);

void heapSort(std::vector<int> &arr);

void Adjust(std::vector<int> &arr, const int idx);
void heapSortBest(std::vector<int> &arr);

void quickSort(std::vector<int> &arr, const int left, const int right);


template <typename DTYPE>
void merge(std::vector<DTYPE> &arr, const int start,
            const int left_size, const int right_size,
            DTYPE* &arr_B);

template <typename DTYPE>
void mergeSort(std::vector<DTYPE> &arr);


// RUNNER PROGRAM:

int main()
{
    vector<int> arr = { 5, 10, -4, 2, 7 };
    vector<int> sorted = arr;

    // sort here:
    // insertionSort(arr);
    // selectionSort(arr);
    // bubbleSort(arr);
    // shellSort(arr);
    // heapSort(arr);
    heapSortBest(arr);
    // bubbleSortBest(arr);
    // bubbleSortSimplified(arr);
    // quickSort(arr, 0, arr.size() - 1);

    printf("Sorted mine:\n");
    displayCont(arr);

    // display solution

    std::sort(sorted.begin(), sorted.end());

    printf("Sorted correctly:\n");
    displayCont(sorted);

    return 0;
}


void insertionSort(std::vector<int> &arr) {
    // for each el go back and insert it right after the first
    // el greater than it, that way left side is always sorted
    for (int i = 0, n = arr.size(); i < n; ++i) {
        int cur_key = arr[i];
        int j = i - 1;
        for ( ; j >= 0 && cur_key < arr[j]; --j) {
            arr[j + 1] = arr[j];
        }
        arr[j + 1]  = cur_key;
    }
}


void shellSort(std::vector<int> &arr) {
    for (int step = arr.size() / 2; step > 0; step /= 2) {
        // do an insertion sort on the els separated by step
        for (int i = step, n = arr.size(); i < n; ++i) {
            int cur_key = arr[i];
            int j = i - step;
            for ( ; j >= 0 && cur_key < arr[j]; j -= step) {
                arr[j + step] = arr[j];
            }
            arr[j + step] =  cur_key;
        }
    }
}


void selectionSort(std::vector<int> &arr) {
    // find the minimum and set it to beginning
    // narrow down the window by 1 and repeat,
    // i labels the beginning of the window
    for (int i = 0, n = arr.size(); i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[min_idx]) {
                // std::swap(arr[i], arr[j]);
                min_idx = j;
            }
        }
        std::swap(arr[i], arr[min_idx]);
    }
}


void bubbleSort(std::vector<int> &arr) {
    for (int i = 0, n = arr.size(); i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

void bubbleSortBest(std::vector<int> &arr) {
    for (int pos = arr.size() - 1; pos > 0; ) {
        int bound = pos;
        pos = 0;
        for (int i = 0; i < bound; ++i) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                pos = i;
            }
        }
    }
}

void bubbleSortSimplified(std::vector<int> &arr) {
    for (int i = 0, n = arr.size(); i < n - 1; ++i) {
        bool were_swaps = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                were_swaps = true;
            }
        }
        if (!were_swaps) return;
    }
}


// left, right inclusive
void quickSortPP2(std::vector<int> &arr, const int left, const int right) {
    if (left >= right) return;

    // put random el at the beginning of subarray
    const int rnd_idx = left + std::rand() % (right - left + 1);
    std::swap(arr[left], arr[rnd_idx]);

    int pivot = left;  // index of last el in subarr smaller than new `left`(rnd el)
    for (int i = left + 1; i <= right; ++i) {
        if (arr[i] <= arr[left]) {
            std::swap(arr[i], arr[++pivot]);
        }
    }

    std::swap(arr[left], arr[pivot]);
    quickSort(arr, left, pivot - 1);
    quickSort(arr, pivot + 1, right);
}

void quickSort(std::vector<int> &arr, const int left, const int right) {
    if (left >= right) return;

    int j = right;
    for (int i = left + 1; ; ) {
        while (i < j && arr[i] <= arr[left]) ++i;
        while (arr[j] > arr[left]) --j;
        if (i < j) std::swap(arr[i], arr[j]);
        else break;
    }
    std::swap(arr[left], arr[j]);

    quickSort(arr, left, j - 1);
    quickSort(arr, j + 1, right);
}


template <typename DTYPE>
void merge(std::vector<DTYPE> &arr, const int start,
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
void mergeSort(std::vector<DTYPE> &arr) {
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


void countingSort(std::vector<int> &arr) {
    int max_int = 0;
    for (auto el : arr) if (el > max_int) max_int = el;

    std::vector<int> counts(max_int + 1, 0);

    for (auto el : arr) {
        counts[el]++;
    }

    for (int i = 1; i <= max_int; ++i) {
        counts[i] += counts[i - 1];
    }

    std::vector<int> sorted(arr.size());
    for (int i = max_int; i >= 0; --i) {
        sorted[counts[arr[i]]--] = arr[i];
    }

    arr = sorted;
}


void heapSort(std::vector<int> &arr) {
    // If you put the root at 0, then the item at array[index] has
    // its children at array[index*2+1] and array[index*2+2].
    // The node at array[child] has its parent at array[(child-1)/2]
    //                     root at 0       root at 1
    // Left child        index*2 + 1     index*2
    // Right child       index*2 + 2     index*2 + 1
    // Parent            (index-1)/2     index/2
    constexpr int root_idx = 0;
    // Generate a MAX heap:
    for (int i = 1 + root_idx, n = arr.size(); i < n; ++i) {  // following is O(N * logN), ADJUST opt is O(N)
        int new_el = arr[i];
        int cur_idx = i;
        for (int parent_idx = (cur_idx - 1) / 2; cur_idx > root_idx && arr[parent_idx] < new_el; ) {
            // move paraent down
            arr[cur_idx] = arr[parent_idx];
            // update ptrs 1 lvl up
            cur_idx = parent_idx;
            parent_idx = (cur_idx - 1) / 2;
        }
        arr[cur_idx] = new_el;
    }

    // Sort using the MAX heap:
    for (int i = arr.size() - 1; i > root_idx; --i) {
        int last_el = arr[i];
        arr[i] = arr[root_idx];
        // select cur as child node of appropriate subtree
        int parent_idx = root_idx;
        int cur_idx = (i - 1 >= root_idx + 2 && arr[root_idx + 2] > arr[root_idx + 1]) ?
                     parent_idx + 2 : parent_idx + 1;
        // propagate the root down
        while (cur_idx <= i - 1 && arr[cur_idx] > last_el) {
            arr[parent_idx] = arr[cur_idx];
            // move ptrs one lvl down
            parent_idx = cur_idx;
            cur_idx = 2 * parent_idx + 1;  // inti as left child
            // set as right child if thats the appropriate direction:
            if (cur_idx + 1 <= i - 1 && arr[cur_idx + 1] > arr[cur_idx]) {
                cur_idx++;
            }
        }
        arr[parent_idx] = last_el;
    }
}


// end is inclusive
void Adjust(std::vector<int> &arr, const int idx, const int end) {
    int cur_key = arr[idx];
    int i = 2 * idx + 1;  // left child of idx-th
    for ( ; i <= end; i = 2 * i + 1) { // move left at each end of iter
        if (i < end && arr[i] < arr[i + 1]) {
            // left child is smaller, so move right
            i++;
        }
        if (cur_key >= arr[i]) {
            arr[(i - 1) / 2] = cur_key;
            return;
        }
        // move bigger child to replace parent
        arr[(i - 1) / 2] = arr[i];
    }
    arr[(i - 1) / 2] = cur_key;
}

void heapSortBest(std::vector<int> &arr) {
        // If you put the root at 0, then the item at array[index] has
    // its children at array[index*2+1] and array[index*2+2].
    // The node at array[child] has its parent at array[(child-1)/2]
    //                     root at 0       root at 1
    // Left child        index*2 + 1     index*2
    // Right child       index*2 + 2     index*2 + 1
    // Parent            (index-1)/2     index/2
    constexpr int root_idx = 0;

    for (int n = arr.size(), i = n / 2; i >= 0; --i) {
        Adjust(arr, i, n);
    }

    for (int i = arr.size() - 2; i >= root_idx; --i) {
        std::swap(arr[i + 1], arr[root_idx]);
        Adjust(arr, root_idx, i);
    }
}

/*

ADJUST(arr, i, n) {
    k = arr[i]
    j = 2*i
    while(j <= n) {
        if (j < n && arr[j] < arr[j + 1]) {
            // left child is smaller
            j++;
        }
        if (k >= arr[j]) {
            arr[j / 2] = k;
            return;
        }
        arr[j / 2] = arr[j];
        j = 2 * j;
    }  // end of while
    arr[j / 2] = k;
}


HEAPSORT(arr) {
    for (int i = n / 2; i >= 1; --1) {
        ADJUST(arr, i, n);
    }
    for (int i = n - 1; i >= 1; --i) {
        swap(arr[i + 1], arr[1]);
        ADJUST(arr, 1, i);
    }
}

*/
/*

    RADIX EXCHANGE:
        2 ptrs, left one moves until it hits el with bit 1
                right decreases until it hits el with bit 0
                those 2 els swap, algo keeps going until j >= i
        repeat for all bits, just on mini segments
*/

/*

    POREDAK CUVAJU:

        Bubble sort
        Insertion sort
        Counting sort
        BST Sort
        Both types of Merge Sort

*/
