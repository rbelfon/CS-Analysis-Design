// Project Two - Course Planner
// This program loads course data from a file and stores it in a binary search tree.
// It lets the user print the full course list or search for a specific course.
// I chose a BST so everything prints in order without extra sorting.
// The file path is entered by the user when the program runs.
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// store one course and its prerequisite course numbers
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// node for the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& newCourse) : course(newCourse), left(nullptr), right(nullptr) {
    }
};

class BinarySearchTree {
private:
    Node* root;

    // insert helper
    void insertNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                insertNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                insertNode(node->right, course);
            }
        }
    }

    // in-order traversal prints courses in alphanumeric order
    void printInOrder(Node* node) const {
        if (node != nullptr) {
            printInOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            printInOrder(node->right);
        }
    }

    // cleanup helper
    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() : root(nullptr) {
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    // clear tree so the user can reload another file if needed
    void clear() {
        destroyTree(root);
        root = nullptr;
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    void insert(const Course& course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            insertNode(root, course);
        }
    }

    Course* search(const string& courseNumber) const {
        Node* current = root;

        while (current != nullptr) {
            if (courseNumber == current->course.courseNumber) {
                return &(current->course);
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return nullptr;
    }

    void printCourseList() const {
        printInOrder(root);
    }
};

// remove spaces from both ends
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    size_t end = text.find_last_not_of(" \t\r\n");

    if (start == string::npos) {
        return "";
    }

    return text.substr(start, end - start + 1);
}

// make course numbers case-insensitive for searching
string toUpperCase(string text) {
    for (size_t i = 0; i < text.size(); ++i) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// split one CSV line by commas
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// read the file and load every course into the BST
bool loadCourseData(const string& fileName, BinarySearchTree& bst) {
    ifstream file(fileName.c_str());
    string line;

    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    // clear old data before loading new data
    bst.clear();

    while (getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        // every valid line must have at least course number and title
        if (tokens.size() < 2) {
            cout << "Error: Invalid file format." << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        // add any prerequisite course numbers
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpperCase(tokens[i]));
            }
        }

        bst.insert(course);
    }

    file.close();
    cout << "Data loaded successfully." << endl;
    return true;
}

// print one course with prerequisite course numbers and titles
void printCourseInfo(const BinarySearchTree& bst, string courseNumber) {
    courseNumber = toUpperCase(trim(courseNumber));

    Course* course = bst.search(courseNumber);

    if (course == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";

    for (size_t i = 0; i < course->prerequisites.size(); ++i) {
        Course* prereq = bst.search(course->prerequisites[i]);

        if (prereq != nullptr) {
            cout << prereq->courseNumber << ", " << prereq->courseTitle;
        }
        else {
            cout << course->prerequisites[i];
        }

        if (i < course->prerequisites.size() - 1) {
            cout << "; ";
        }
    }

    cout << endl;
}

int main() {
    BinarySearchTree bst;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "That is not a valid option." << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            string fileName;

            cout << "What is the name of the file? ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, fileName);

            if (loadCourseData(fileName, bst)) {
                dataLoaded = true;
            }
            else {
                dataLoaded = false;
            }
            break;
        }

        case 2:
            if (!dataLoaded || bst.isEmpty()) {
                cout << "No data loaded." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                bst.printCourseList();
            }
            break;

        case 3:
            if (!dataLoaded || bst.isEmpty()) {
                cout << "No data loaded." << endl;
            }
            else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                printCourseInfo(bst, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}
