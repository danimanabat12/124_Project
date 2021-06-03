#include<iostream>
#include<vector> 
#include<cctype>

using namespace std; 

/*
summary of functions made

book isKeyword - returns true if the string is a keyword(BEG, PRINT, MOD)
bool isValid - returns true if the string satisfies variable name req.
bool isInteger - returns true if the string is an integer
bool isFloat -returns true if the string is a float
bool isOperator - returns true if the char is an operator symbol

bool containsNoChar - returns true if there is no alphabet in the string
vector<string> processInput - accepts string input from user, returns vector with the inputs separated according to their 'tokens'


bool isUsed - returns true if the string is a variable name that does not exist
void variable_history - updates the masterlist of all variables inputted by the user
void semantics - processes the intent and works through the commands of the user

*/



typedef struct variables {                                                                          //typedef struct for the variables 
    string name; 
    string value; 
    string type; 
}variables;


/*Helper function that checks if the variable is a keyword*/
bool isKeyword(string input){
	if(input == "BEG" || input == "PRINT" || input == "MOD") return true;		//if the set variable is a keyword
	else return false;
}


/*Helper function that checks the variable's validity*/
bool isValid(string input){
	int i;
	int valid = true;															//initialize valid as true	

		if(isalpha(input[i])) valid = true;										//if the input is only alphanumeric	that start with a letter
		else valid = false;														//else invalid
		
		if(valid == true){														//while it is valid
		  	for(i = 1; i < input.size(); i++){									//scan each element of the string
		  	   	if(!(isalpha(input[i]) || isdigit(input[i]) || input[i] == '_')){	//if there is unneccessary characters then invalid
					valid = false;
					break;
				}
		  	   }
		  	   return true; 													//if the variable is valid, it will return true
		  }
		

	else return false;															//else not
}

bool isUsed(vector<variables> var_list, string input){
	int i=0;
	int valid = true;
	
	while(i < var_list.size()){
		if(var_list.at(i).name == input){
			return false;
		}		
		i++;
	}
	return valid;
}



/*Helper function for identifying the data type of the input is an integer*/
bool isInteger(string input){
    int minus = 0;																//set minus as zero
    int digits = 0;																//set digits as zero
    
    for(int i = 0; i < input.size(); i++){										//scan the each element of the input 
        if(isdigit(input[i]) != 0){ 											//if the element is not zero
        	digits++;															
		}
        else if(input[i] == '-'){												//if the element is negative
        	minus++;					
		}   
    }
    if(digits + minus == input.size()) return true;								//if both digits and minus are single digit and has no decimal then it is an integer
    else return false;															//else it is not an integer type
}

/*Helper function for identifying the data type of the input is an integer*/
bool isFloat(string input){
    int minus = 0;																//set minus as zero
    int digits = 0;																//set digits as zero
    int decimal = 0;															//set decimal as zero
    
    for(int i = 0; i < input.size(); i++){										//scan the input by checking one-by-one
        if(isdigit(input[i]) != 0){												//if the element is not zero
        	digits++;															//increment digits
		}
        else if(input[i] == '-'){												//if the element is negative
        	minus++;															//increment minus
		}
		else if(input[i] == '.'){												//if one the elements in the input contains a period
            decimal++;															//increment decimal
        }   
    }
    if (digits + decimal + minus == input.size())  return true; 				//if the input has a decimal point then it is a floating point        
	else return false;															//else it is not a floating point
}


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

vector<string> processInput(string input) {                                                                   //function that process that user command
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
	
	return user_command;
}




void variable_history(vector<variables> &var_list, vector <string> &user_command){

	//if first element is variable put into variable vector since there can only be 1 l-value in an assignment
	if(user_command.at(0) != "" && isValid(user_command.at(0))){
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(0);
	}

}

void semantics(vector <variables> &var_list, vector <string> &user_command){									//due to nature of switch only accepting int and does not handle string well, if are used throughout
	//check if assignment,
	if(!isUsed(var_list, user_command[0]) && user_command.at(1) == "="){							//if the 1st input is a variable and the second is an equal sign
		cout<<"Eh"<<endl;
	}
}


int main() {
    string input;
	//making a vector to contain all successful variables inputted by user
	vector<variables> var_list;
	

    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    while(input != "EXIT!") {
        cout << "\nCommand: ";
        getline(cin, input);
        vector<string> user_command = processInput(input);        
        
        //assume input already error checed.
        //assume input data types are already identified
        variable_history(var_list, user_command);
        
        semantics(var_list, user_command);       
        
        
        
    }

    cout << "\nInterface is now terminated..." << endl;
}
