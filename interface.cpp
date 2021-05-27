#include<iostream>
#include<vector> 
#include<cctype>

using namespace std; 

typedef struct variables {                                                                          //typedef struct for the variables 
    string name; 
    string value; 
    string type; 
}variables;

bool isOperator (char ch) {                                                                         //boolean function that checks if a character is an operator
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool containsNoChar (string input) {                                                                //boolean function that checks if a string contains alphabet charaters
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

void processInput(string input) {                                                                   //function that process that user command
    vector<string> user_command;                                                                      //vector of type string which stores the user command string into substrings
    string placeholder="";                                                                              
    int i = 0; 

    while (i < input.length()) {                                                                      //runs until the length of user command string

        if (isalnum(input[i]) || input[i] == '.') {                                                   //checks if the current character is alphanumeric or a dot
            if (isOperator(placeholder[placeholder.size()-1])) {                                        //if yes, it firstly checks if the last character on the placeholder string is an operator   
                if(isdigit(input[i])) {                                                                 //if the upper if-statement is evaluated, it checks if the current char is a digit
                    placeholder += input[i];                                                              //if the current char is digit, it concatenates the digit to the placeholder string
                }
                else {                                                                                  //if the current char is not a digit, it pushes the placeholder onto the vector user_command
                    user_command.push_back(placeholder); 
                    placeholder = input[i];                                                              //re-set the placeholder string to the current char
                }
            }
            else {                                                                                      //else if the last charater on the placeholder string is not an operator
                placeholder += input[i];                                                                  //we concatenate the current char to the placeholder string
            }
        }

        else if (isOperator(input[i])) {                                                               //checks if the current character is an operator
            if (containsNoChar(placeholder)) {                                                           //if yes, we check if the placeholder string contains no character
                placeholder += input[i];                                                                 //if it doesn't contains any character, we concatenate the current input to the placeholder string
            }                                                                                               
            else {                                                                                     //else,
                if (placeholder != "") {                                                                 //we check if the placeholder string is empty
                    user_command.push_back(placeholder);                                                 //if not, we push the placeholder to the user_command vector
                    user_command.push_back(string(1, input[i]));                                         //furthermore, we add the current operator to the user_command vector
                    placeholder = "";                                                                    //reset the placeholder string to an empty string
                }
            }
        }

        else if (input[i] == '=') {                                                                    //check if the current character is an equal sign
            if (placeholder != "") {                                                                     //checks if the placeholder string is not empty
                user_command.push_back(placeholder);                                                     //place whatever in the placeholder to the user_command vector
            }
            user_command.push_back("=");                                                                 //also, we push the equal sign to the vector
            placeholder = "";                                                                            //reset the placeholder string to empty string
        }

        else if (input[i] == ' ') {                                                                    //checks if the current character is a whitespace
            while (input[i] == ' ') {                                                                    //This loop runs until the character is no longer a white space (eliminating extra ws)
                i++;                                                                                     //increment i so we can access the next character in the input string
            }
            if (isOperator(input[i])) {                                                                //if the current character is an operator, same logic from line 47-58 applies here
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
            else {                                                                                     //if the current character is not an operator
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {               //we check if the last char in the placeholder string is not an operator and the placeholder string is not empty
                    user_command.push_back(placeholder);                                                 //if the condition are met, we push whatever the content of placeholder string to the vector 
                    placeholder = "";                                                                    //reset the placeholder string to empty 
                    //placeholder += input[i];                                                        
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {                              //else if the current character is an operator
                    // if (isdigit(input[i])) { //isalnum kanina pero may mali ata.                       //check if the current char is a digit
                    //     //placeholder += input[i];                                                         //if a digit, we concatenate it to the placeholder string
                    // }
                    if (!isdigit(input[i])) {                                                           //check if current input is not a digit, then we push placeholder string to the vector
                        user_command.push_back(placeholder);                                             
                        placeholder = "";
                        //placeholder += input[i]; 
                    }
                }                                                             
                placeholder += input[i]; 
            }
        }
        i++;
    }

    if (placeholder != "") {                                                                            //If the placeholder string is not empty after the loop, we add whatever's left to the vector
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