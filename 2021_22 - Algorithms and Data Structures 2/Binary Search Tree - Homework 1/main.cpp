/*

    Demonstrates some BST functionalities, there are
    2 versions of main, for MSVC and GPP compilers, set
    the IS_USING_GPP_COMPILER flag so the newline chars do no mess things up, also
    use large console window as the tree gets displayed properly
    with trimmed outputs to keep the indentation so the last lvl
    can fit in single row.

*/

const bool IS_USING_GPP_COMPILER = true;



#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>  // catching errors


#include "BinarySearchTree.hpp"


// ============================
// Globals:
enum USER_OPTIONS : char {
    USER_LOAD_IN_TREE = 1, USER_GET_VAL_FROM_KEY, USER_ADD_TO_TREE,
    USER_DISPLAY_TREE, USER_DELETE_KEY, USER_DESTROY_TREE, USER_MOST_FREQUENT_KEY, USER_QUIT
};


// ============================
// Program Functions:

USER_OPTIONS displayMainMenu();


// ============================
// Helper Functions:

template<typename CONT>
void displayContainer(const CONT& cont);

template<typename MATR>
void displayMatrix(const MATR& matr);
int mainGPP();


// ============================


int main()
{
    // if using GPP and not MSVC
    if (IS_USING_GPP_COMPILER) return mainGPP();

    std::string tree_uninit_msg = "BST object is uninitialised, please use option 1 to initialise it\n\n";
    BinarySearchTree<std::string, std::string>* tree = NULL;
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

            if (user_cmd == 2) {  // from file
                printf("Enter the path to file:\n");
                std::cin.sync();
                std::cin.ignore();
                std::getline(std::cin, file_path);
                try {
                    tree = new BinarySearchTree<std::string, std::string>(file_path, false);
                } catch (std::ios_base::failure &e) {
                    std::cout << "Oops, an error occurred. Are you sure the given file exists\n";
                    break;
                }
            }
            else {
                tree = new BinarySearchTree<std::string, std::string>();
                printf("Start entering the key value pairs (each key/value is in separate line).\n");
                printf("To mark the end of adding key value pairs type `%s` instead of expected key:\n",
                    instream_end_flag.c_str());
                std::cin.sync();
                std::cin.ignore();
                tree->BuildFromInputStream(std::cin, instream_end_flag, false);
            }

            printf("\nSuccessfully created a new tree.\n");

            break;

        case USER_GET_VAL_FROM_KEY:
            found_values.clear();

            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }

            printf("Enter the key whose values are to be found:\n");
            std::cin.sync();
            std::cin.ignore();
            std::getline(std::cin, cur_key);
            if (!tree->getValue(cur_key, found_values)) {
                printf("Failed to find the key, are you sure it exists in the tree?\n");
                break;
            }

            if (found_values.size()) {
                printf("Found values:\n");
                // displayContainer(found_values);
                for (const auto& it : found_values) {
                    std::cout << *it << ", ";
                }
                printf("\nEnd of found values\n");
            }
            else printf("There were no found values ??\n");

            break;

        case USER_ADD_TO_TREE:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            std::cin.sync();
            std::cin.ignore();

            printf("Enter the key:\n");
            std::getline(std::cin, cur_key);

            printf("Enter the value:\n");
            std::getline(std::cin, cur_val);

            tree->insertKeyValuePair(std::make_pair(cur_key, cur_val));
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

            std::cin.sync();
            std::cin.ignore();

            printf("Enter the key to delete:\n");
            std::getline(std::cin, cur_key);

            if (!tree->deleteAllKeys(cur_key)) {
                printf("Failed to delete the key, are you sure it exists in the tree?\n");
                break;
            }

            break;

        case USER_DESTROY_TREE:
            if (tree) delete tree;
            tree = NULL;
            break;

        case USER_MOST_FREQUENT_KEY:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            std::cout << "Key with most occurrances is: " << tree->getMostFrequentKey() << "\n";
            break;

        default:
            do_not_quit = false;
            break;
        }
    }

    // Free up the taken memory
    if (tree) delete tree;

    return 0;
}


// ============================
// Program Functions:

// returns the option chosen by the user
USER_OPTIONS displayMainMenu() {
    printf("\n\nPlease choose one of the following\n(type in the option's number and press Enter):\n\n");
    printf("1 Load in the tree\n");
    printf("2 Search for values based on key\n");
    printf("3 Add new key value pair to the tree\n");
    printf("4 Display tree\n");
    printf("5 Delete the key and all corresponding values from the tree\n");
    printf("6 Destroy the tree\n");
    printf("7 Get key with most occurrances\n");
    printf("\nEnter any other integer to Quit\n\n");

    int user_cmd = -1;
    scanf("%d", &user_cmd);

    return static_cast<USER_OPTIONS> (user_cmd);
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


// Strings are read as single words instead of whole line
int mainGPP()
{
    std::string tree_uninit_msg = "BST object is uninitialised, please use option 1 to initialise it\n\n";
    BinarySearchTree<std::string, std::string>* tree = NULL;
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

            if (user_cmd == 2) {  // from file
                printf("Enter the path to file:\n");
                std::cin >> file_path;
                tree = new BinarySearchTree<std::string, std::string>(file_path, false);
            }
            else {
                tree = new BinarySearchTree<std::string, std::string>();
                printf("Start entering the key value pairs (each key/value is in separate line).\n");
                printf("To mark the end of adding key value pairs type `%s` instead of expected key:\n",
                    instream_end_flag.c_str());
                std::cin.sync();
                std::cin.ignore();
                tree->BuildFromInputStream(std::cin, instream_end_flag, false);
            }

            printf("\nSuccessfully created a new tree.\n");

            break;

        case USER_GET_VAL_FROM_KEY:
            found_values.clear();
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }

            printf("Enter the key whose values are to be found:\n");
            std::cin >> cur_key;
            if (!tree->getValue(cur_key, found_values)) {
                printf("Failed to find the key, are you sure it exists in the tree?\n");
                break;
            }

            if (found_values.size()) {
                printf("Found values:\n");
                // displayContainer(found_values);
                for (const auto& it : found_values) {
                    std::cout << *it << ", ";
                }
                printf("\nEnd of found values\n");
            }
            else printf("There were no found values ??\n");

            break;

        case USER_ADD_TO_TREE:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }

            printf("Enter the key:\n");
            std::cin >> cur_key;

            printf("Enter the value:\n");
            std::cin >> cur_val;

            tree->insertKeyValuePair(std::make_pair(cur_key, cur_val));
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


            printf("Enter the key to delete:\n");
            std::cin >> cur_key;

            if (!tree->deleteAllKeys(cur_key)) {
                printf("Failed to delete the key, are you sure it exists in the tree?\n");
                break;
            }

            break;

        case USER_DESTROY_TREE:
            if (tree) delete tree;
            tree = NULL;
            break;

        case USER_MOST_FREQUENT_KEY:
            if (!tree) {
                std::cout << tree_uninit_msg;
                break;
            }
            std::cout << "Key with most occurrances is: " << tree->getMostFrequentKey() << "\n";
            break;

        default:
            do_not_quit = false;
            break;
        }
    }

    // Free up the taken memory
    if (tree) delete tree;

    return 0;
}
