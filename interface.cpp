#include<iostream>
#include<vector> 
#include<cctype>

using namespace std; 

typedef struct variables {
    string name; 
    string value; 
    string type; 
}variables;

bool isOperator (char ch) {
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool containsNoChar (string input) {
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

void processInput(string input) {
    vector<string> user_command; 
    string placeholder="";
    int i = 0; 

    while (i < input.length()) {

        if (isalnum(input[i]) || input[i] == '.') {
            if (isOperator(placeholder[placeholder.size()-1])) {
                if(isdigit(input[i])) {
                    placeholder += input[i];
                }
                else {
                    user_command.push_back(placeholder); 
                    placeholder = input[i];
                }
            }
            else {
                placeholder += input[i];
            }
        }

        else if (isOperator(input[i])) {
            if (containsNoChar(placeholder)) {  //placeholder[placeholder.size()-1]
                placeholder += input[i];
            }
            else {
                if (placeholder != "") {
                    user_command.push_back(placeholder); 
                    user_command.push_back(string(1, input[i])); 
                    placeholder = ""; 
                }
            }
        }
        else if (input[i] == '=') {
            if (placeholder != "") {
                user_command.push_back(placeholder);
            }
            user_command.push_back("=");
            placeholder = "";  
        }
        else if (input[i] == ' ') {
            while (input[i] == ' ') {
                i++;
            }
            if (isOperator(input[i])) {
                if (containsNoChar(placeholder)) {
                    placeholder += input[i];
                } 
                else {
                    if (placeholder != "") {
                        user_command.push_back(placeholder); 
                        user_command.push_back(string(1, input[i])); 
                        placeholder = ""; 
                    }
                }
            }
            else {
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {
                    user_command.push_back(placeholder); 
                    placeholder = ""; 
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {
                    if (isalnum(input[i])) {
                        placeholder += input[i];
                    }
                    else {
                        user_command.push_back(placeholder); 
                        placeholder = "";
                    }
                }
                placeholder += input[i];
            }
        }
        i++;
    }

    if (placeholder != "") {
        user_command.push_back(placeholder);
        placeholder = "";
    }

    cout << "Debug: " << endl;
    for (i = 0; i < user_command.size(); i++) {
        cout << i << ": " << user_command.at(i) << endl;
    }

}

int main() {
    string input;

    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    while(input != "EXIT!") {
        cout << "\nCommand: ";
        getline(cin, input);
        processInput(input);
    }

    cout << "\nInterface is now terminated..." << endl;
}