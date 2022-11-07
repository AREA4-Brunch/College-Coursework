/*
    Developed for MSVC and GPP compilers, set
    the IS_USING_GPP_COMPILER flag so the newline
    chars do not mess things up.

*/

const bool IS_USING_GPP_COMPILER = true;


// #define _CRT_SECURE_NO_WARNINGS

// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>

#include <iostream>
#include "BStarTree.hpp"


using std::cout;



// ============================
// Globals:

enum USER_OPTIONS : char {
    USER_LOAD_IN_TREE = 1, USER_ADD_TO_TREE,
    USER_DISPLAY_TREE, USER_DELETE_KEY, USER_DESTROY_TREE,
    USER_KTH_SMALLEST, USER_QUIT
};


// ============================
// Program Functions:

USER_OPTIONS displayMainMenu();

void amuseUser();


// ============================
// Helper Functions:

template<typename CONT>
void displayContainer(const CONT& cont);

template<typename MATR>
void displayMatrix(const MATR& matr);



// ============================
// Tests:

void test();
void newTest();


// ============================
// RUNNER


int main()
{
    //test();
    // newTest();

    amuseUser();

    return 0;
}


// ============================
// Program Functions:


// returns the option chosen by the user
USER_OPTIONS displayMainMenu() {
    printf("\n\nPlease choose one of the following\n(type in the option's number and press Enter):\n\n");
    printf("%d Load in the tree\n", (int) USER_LOAD_IN_TREE);
    printf("%d Add new key to the tree\n", (int) USER_ADD_TO_TREE);
    printf("%d Display tree\n", (int) USER_DISPLAY_TREE);
    printf("%d Delete the key and all corresponding values from the tree\n", (int) USER_DELETE_KEY);
    printf("%d Destroy the tree\n", (int) USER_DESTROY_TREE);
    printf("%d Get k-th smallest key\n", (int) USER_KTH_SMALLEST);
    printf("\nEnter any other integer to Quit\n\n");

    int user_cmd = -1;
    scanf("%d", &user_cmd);

    return static_cast<USER_OPTIONS> (user_cmd);
}


// main program
void amuseUser() {

    std::string tree_uninit_msg = "B* Tree object is uninitialised, please use option 1 to initialise it\n\n";
    BStarTree<std::string> *tree = nullptr;
    std::string file_path, cur_key, cur_val;
    std::string instream_end_flag = "__end";
    std::vector<std::string*> found_values;

    for (bool do_not_quit = true; do_not_quit; ) {
        switch (displayMainMenu())
        {
        case USER_LOAD_IN_TREE:
            printf("Do you want to load in from terminal or file (1/2): ");
            int user_cmd;
            scanf("%d", &user_cmd);
            if (tree) {
                delete tree;
                printf("Deleted the old tree.\n");
            }

            printf("Enter the max num of subtrees M: ");
            int m;
            scanf("%d", &m);

            if (user_cmd == 2) {  // from file
                printf("Enter the path to file:\n");

                if (IS_USING_GPP_COMPILER)
                    std::cin >> file_path;  // g++
                else {
                    std::cin.sync();
                    std::cin.ignore();
                    std::getline(std::cin, file_path);
                }

                try {
                    tree = new BStarTree<std::string>(m, file_path, false);
                }
                catch (std::ios_base::failure& e) {
                    std::cout << "Oops, an error occurred. Are you sure the given file exists\n";
                    break;
                }
            }
            else {
                tree = new BStarTree<std::string>(m);
                printf("Start entering the keys (each key is in separate line).\n");
                printf("To mark the end of adding keys type `%s` instead of expected key:\n",
                    instream_end_flag.c_str());
                std::cin.sync();
                std::cin.ignore();
                tree->BuildFromInputStream(std::cin, instream_end_flag, false);
                //std::cin >> *tree;
            }

            printf("\nSuccessfully created a new tree.\n");

            break;

        case USER_ADD_TO_TREE:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            if (!IS_USING_GPP_COMPILER) {
                std::cin.sync();
                std::cin.ignore();
            }
            printf("Enter the key:\n");

            if (IS_USING_GPP_COMPILER) std::cin >> cur_key;  // g++
            else std::getline(std::cin, cur_key);

            if (!tree->insertKey(cur_key)) {
                printf("The key already exists in the tree so insertion was ignored\n");
            }

            break;

        case USER_DISPLAY_TREE:
            if (tree) std::cout << "\nTree:\n\n" << *tree << "\n\n";
            else std::cout << tree_uninit_msg;
            break;

        case USER_DELETE_KEY:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            if (!IS_USING_GPP_COMPILER) {
                std::cin.sync();
                std::cin.ignore();
            }
            printf("Enter the key to delete:\n");

            if (IS_USING_GPP_COMPILER) std::cin >> cur_key;  // g++
            else std::getline(std::cin, cur_key);

            if (!tree->deleteKey(cur_key)) {
                printf("Failed to delete the key, are you sure it exists in the tree?\n");
                break;
            }

            break;

        case USER_DESTROY_TREE:
            if (tree) delete tree;
            tree = nullptr;
            break;

        case USER_KTH_SMALLEST:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            int k;
            printf("Enter k (0-indexed): ");
            if (IS_USING_GPP_COMPILER) std::cin >> k;  // g++
            else scanf("%d", &k);

            try {
                std::cout << k << "-th key is: " << tree->getKthSmallestKey(k) << "\n";
            }
            catch (const BStarTreeCustomException &e) {
                std::cout << std::endl
                    << "Oops, an error ocurred:"
                    << std::endl << e.what() << std::endl;
            }
            break;

        default:
            do_not_quit = false;
            break;
        }
    }

    // Free up the taken memory
    if (tree) delete tree;

}


// ============================
// Helper Functions:


template<typename CONT>
void displayContainer(const CONT& cont) {
    for (const auto& el : cont) {
        std::cout << el << ", ";
    }
    printf("\n");
}


template<typename MATR>
void displayMatrix(const MATR& matr) {
    for (const auto& row : matr) {
        displayContainer(row);
    }
    printf("\n");
}


template<typename DTYPE>
std::ostream& operator<< (std::ostream& os, const std::pair<DTYPE, DTYPE>& x) {
    std::cout << "(" << x.first << ", " << x.second << ")";
}


// ============================
// Testing


void test() {
    BStarTree<std::string> tree(3);
    int k = 0;

    tree.insertKey("08");
    cout << tree << "\n";
    tree.insertKey("23");
    cout << tree << "\n";
    tree.insertKey("18");
    cout << tree << "\n";

    //tree.insertKey("06");
    //cout << tree << "\n";
    //tree.insertKey("05");
    //cout << tree << "\n";

    //tree.insertKey("21");
    //cout << tree << "\n";

    tree.insertKey("16");
    cout << tree << "\n";
    tree.insertKey("33");
    cout << tree << "\n";
    tree.insertKey("31");
    cout << tree << "\n";
    tree.insertKey("28");
    cout << tree << "\n";
    tree.insertKey("38");
    cout << tree << "\n";
    tree.insertKey("48");
    cout << tree << "\n";
    tree.insertKey("40");
    cout << tree << "\n";
    tree.insertKey("57");
    cout << tree << "\n";

    tree.deleteKey("18");
    cout << tree << "\n";
    tree.deleteKey("31");
    cout << tree << "\n";
    tree.deleteKey("57");
    cout << tree << "\n";
    tree.deleteKey("40");
    cout << tree << "\n";


    k = 1;
    for (int k = 0; k < 11 - 4; ++k) {
        cout << k << "-th: "
            << tree.getKthSmallestKey(k) << "\n";
    }

}


void newTest() {
    BStarTree<std::string> tree(4, "new_test.txt");

    cout << tree << "\n";

    tree.insertKey("18");
    cout << tree << "\n";
    tree.insertKey("19");
    cout << tree << "\n";
    tree.insertKey("20");
    cout << tree << "\n";
    tree.deleteKey("03");
    cout << tree << "\n";

    tree.insertKey("08");
    cout << tree << "\n";
}
