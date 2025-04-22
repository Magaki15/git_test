#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <ctime>
using namespace std;

const int CAPACITY = 100;
const int TYPE_CAPACITY = 100;

struct Question {
    string text;
    string answer;
    string type;
};

int split(string str, char delimiter, string arr[], int arr_size) {
    int count = 0;
    size_t start = 0, end;

    while ((end = str.find(delimiter, start)) != string::npos && count < arr_size) {
        arr[count++] = str.substr(start, end - start);
        start = end + 1;
    }
    if (start < str.size() && count < arr_size)
        arr[count++] = str.substr(start);

    return count;
}

string trim(string str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

int count_occurrence(string str, char ch) {
    int count = 0;
    for (char c : str)
        if (c == ch)
            count++;
    return count;
}

string* extract_type(string type, int& num_types_curr_item) {
    num_types_curr_item = count_occurrence(type, ';') + 1;
    string* result = new string[num_types_curr_item];

    size_t start = 0, end;
    int index = 0;

    while ((end = type.find(';', start)) != string::npos) {
        result[index++] = trim(type.substr(start, end - start));
        start = end + 1;
    }
    result[index] = trim(type.substr(start));

    return result;
}

bool contains_type(Question q, string type) {
    int count;
    string* types = extract_type(q.type, count);
    for (int i = 0; i < count; i++) {
        if (types[i] == type) {
            delete[] types;
            return true;
        }
    }
    delete[] types;
    return false;
}

void randomize(Question arr[], int size) {
    for (int i = 0; i < size; i++) {
        int r = rand() % size;
        Question temp = arr[i];
        arr[i] = arr[r];
        arr[r] = temp;
    }
}

void insert_order_unique(string types[], int type_capacity, int& type_count, string toAdd) {
    if (type_count >= type_capacity)
        return;

    int i = 0;
    while (i < type_count && types[i] < toAdd)
        i++;

    if (i < type_count && types[i] == toAdd)
        return;

    for (int j = type_count; j > i; j--)
        types[j] = types[j - 1];

    types[i] = toAdd;
    type_count++;
}

void insert_order_unique(string types[], int type_capacity, int& type_count, Question ques[], int ques_size) {
    for (int i = 0; i < ques_size; i++) {
        int num;
        string* extracted = extract_type(ques[i].type, num);
        for (int j = 0; j < num; j++) {
            insert_order_unique(types, type_capacity, type_count, extracted[j]);
        }
        delete[] extracted;
    }
}

string choose_type(string* types, int type_count) {
    int choice;
    cout << "0. ALL TYPES" << endl;
    for (int i = 0; i < type_count; i++)
        cout << i + 1 << ". " << types[i] << endl;

    do {
        cout << "Choose a type [0-" << type_count << "]: ";
        cin >> choice;
    } while (choice < 0 || choice > type_count);

    if (choice == 0)
        return "";
    return types[choice - 1];
}

void answer_by_type(Question ques[], int size, string type) {
    randomize(ques, size);
    int score = 0;
    int count = 0;
    string ans;

    for (int i = 0; i < size; i++) {
        if (type != "" && !contains_type(ques[i], type)) continue;

        count++;
        cout << "Q: " << ques[i].text << endl;
        cout << "Your answer: ";
        cin.ignore();
        getline(cin, ans);

        if (trim(ans) == trim(ques[i].answer)) {
            cout << "Correct!" << endl;
            score++;
        } else {
            cout << "Incorrect. The correct answer was: " << ques[i].answer << endl;
        }
    }

    cout << "\nYou scored " << score << " out of " << count << "!" << endl;
}

int read_questions(string filename, Question ques[], int capacity) {
    ifstream file(filename);
    if (!file) return 0;

    string line;
    int size = 0;

    while (getline(file, line) && size < capacity) {
        string fields[3];
        if (split(line, '|', fields, 3) == 3) {
            ques[size].text = trim(fields[0]);
            ques[size].answer = trim(fields[1]);
            ques[size].type = trim(fields[2]);
            size++;
        }
    }
    return size;
}

int main() {
    srand(time(0));
    Question ques[CAPACITY];
    string types[TYPE_CAPACITY];
    int typeCount = 0;

    string filename;
    cout << "Enter filename: ";
    cin >> filename;

    int size = read_questions(filename, ques, CAPACITY);
    if (size == 0) {
        cout << "Error reading file." << endl;
        return 1;
    }

    insert_order_unique(types, TYPE_CAPACITY, typeCount, ques, size);
    string chosenType = choose_type(types, typeCount);
    answer_by_type(ques, size, chosenType);

    return 0;
}