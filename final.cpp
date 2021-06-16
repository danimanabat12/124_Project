/*
	AMDG
	
	Program title: Simple Number-Only Language
	
	Program description: This is a simplied custom programming language that utilizes C++ as its interpreter.
	This custom programming language could perform basic programming languages concepts such as variable assignment, 
	data type identification, arithmetic operations, etc. 
	
	Program creators: Cabuga, Van Joseph		2019 - 08453
					  Gador, Ferdinand II		2019 - 08264
					  Manabat, Daniel Gabriel	2019 - 60012
					  Redelosa, Beau Mary		2019 - 60013
					  Vertudes, Ron Bryan		2019 - 08419
*/


//Standard Library Used
#include<iostream>
#include<vector> 
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>
#include<stack>
#include<algorithm>
#include<cmath>
#include<cstdlib>
#include<string>
using namespace std; 

//typedef struct for the masterlist of variables
typedef struct variables {                                                                          
    string name; 
    string value; 
    string type; 
}variables;

//helper functions
void helper();
int string_size(string input);
bool isOperator(char ch);
bool isKeyword(string input);
bool containsNoChar (string input);
bool isValid(string input);
bool isUsed(vector<variables> var_list, string input);
bool isFloat(string input);
bool isInteger(string input);
bool isOperand(string input);
string extractDataType (vector<variables> &var_list, string var_name);

//tokenizing function
vector<string> processInput(string input);

//error-checking function
bool errorChecking(vector<string> user_command); 
bool processElements(vector<string> user_command, vector<variables> &var_list);

//processing functions
void variable_history(vector<variables> &var_list, vector <string> &user_command);
string evaluate(string &input, vector <variables> &var_list);
string concatenate(vector<string> user_command, int starting_point);
void semantics(vector <variables> &var_list, vector <string> &user_command);

//arithmetic functions
string arithmetic(string); 
string infixToPostfix(string);
int getPrecedence(char);
vector<string> tokenize(string);
int getState(char&);
float solvePostfix(const vector<string>&);
float performOp(char, float, float);

enum states{REJECT = 0, INTEGER, REAL, NEGATIVE, OPERATOR, UNKNOWN, SPACE};

int main() {
	
	//variable declaration
    string input;
	vector<variables> var_list;
	//

	//interface message
    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    while(input != "EXIT!") {																	//While loop that runs until EXIT! has been entered.
        cout << "\nCommand: ";																	//Prompt the user to enter a command
        getline(cin, input);																	//Store the command to the string input
       	if (input == "HELP") {
       		helper(); 	
		}
		else if (input == "EXIT!") {;}
		else if (input.size() > 1 || isValid(input)){
			vector<string> user_command = processInput(input);  								//process the user input and store them to a vector of string
			if (!user_command.empty()) {														//if user command is not empty, we start processing the vector
				if (errorChecking(user_command)) {												//first, we error check the user command. We call the errorChecking function
					if (processElements(user_command, var_list)) {								//If the first errro checking did not encounter any problem, we call the processElements function
						if(user_command.size() > 1 || !isValid(user_command.at(0))) {			//If user_command vector contains more than one element or if the input is an arithmetic expression at this point, we process the user command
							variable_history(var_list, user_command); 							//store variables on the variable history vector
			         		semantics(var_list, user_command);  								//process the semantics and the command in general of the user
						}
					}
				}		
			}      
		}
		else if (!isValid(input)) {
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
		}
    }

    cout << "\nInterface is now terminated..." << endl;											//interface statement when the loop stopped working
}

void helper() {
	cout << "=============================================" << endl; 
	cout << "SNOL General rules, guidelines, and How to's!" << endl; 
	cout << "=============================================" << endl;
	
	cout << "\n\nRULES and GUIDELINES" << endl;
	cout << "\t1. FORMATTING: > Use spaces as a separator. Extra spaces, however, will be ignored." << endl;
	cout << "\t   Example: var = 6 -> Contains one space between every word" << endl;  
	cout << "\t            var     =   6 -> Contains more than one space for every word. This will still be evaluated like above." << endl;  
	cout << "\t2. CASE-SENSITIVE: > All words are case sensitive" << endl;
	cout << "\t   Example: var is different from VAR, and so is vAr. They will be treated as different entities" << endl;
	cout << "\t3. DATA TYPE: > This programming language only has two data types, namely: INTEGER and FLOATING point" << endl;
	cout << "\t              > The operands for arithmetic operations SHOULD have a similar data type" << endl;
	cout << "\t   Example: var = 1 + 5     -> Correct, since both 1 and 5 are INTEGERS" << endl;   
	cout << "\t            var = 1.5 + 3.5 -> Correct, since both 1.5 and 3.5 are FLOATING-POINT" << endl;  
	cout << "\t            var = 1.5 + 3   -> ERROR. 1.5 and 3 are conflicting data types" << endl;
	cout << "\t4. VARIABLES: > Variables always be alphanumeric. Also, it should start with letters, not numbers." << endl;
	cout << "\t   Example: var    -> Correct, since this variable is alphanumeric" << endl; 
	cout << "\t            var01  -> Also correct, this variable is alphanumeric" << endl; 
	cout << "\t           !var01  -> ERROR, this contains a special character" << endl; 
	cout << "\t           1var01  -> ERROR, variables starting with numbers is not allowed" << endl; 
	cout << "\n\t              > NOTE: Variables SHOULD BE DEFINED FIRST before they can be used." << endl;
	cout << "\t5. INVALID SYNTAX: > Your command should be logical and acceptable. Here are some cases where command will result to an error: " << endl;
	cout << "\t   Example: PRINT -> Calling the PRINt keyword (or BEG) without a targeted literal or variable" << endl; 
	cout << "\t            PRINT var -> When var is not yet declared, this will result to an error" << endl; 
	cout << "\t            PRINT BEG -> Using two or more special keyword on a specific command" << endl; 
	cout << "\t            num PRINT xd = 1 -> Unordered command. Special keywords found in between words" << endl;
	cout << "\t            num = var1 var+1 -> Illogical arithmetic operations. Some variables or literals are not used on an operation" << endl;  
	
	cout << "\n\nHOW TOs" << endl;
	cout << "\t1. Declaring a variable: Use either the assigment operator ('=') or the special keyword BEG" << endl;
	cout << "\t   Example: var = 6 or BEG var" << endl; 
	cout << "\t2. Arithmetic operations: These are the list of available operations: " << endl;
	cout << "\t   + -> Addition. Example: opr_1 + opr_2 + opr_3, where opr could be a variable or a literal" << endl; 
	cout << "\t   - -> Subtraction. Example: opr_1 - opr_2 - opr_3, where opr could be a variable or a literal" << endl; 
 	cout << "\t   * -> Multiplication. Example: opr_1 * opr_2 * opr_3, where opr could be a variable or a literal" << endl; 
 	cout << "\t   + -> Division. Example: opr_1 / opr_2, where opr could be a variable or a literal" << endl; 
 	cout << "\t   % -> Modulo. Example: opr_1 % opr_2, where opr could be a variable or a literal. NOTE: INTEGERS types are only allowed for this operation" << endl; 
 	cout << "\n\t   NOTE: The precedence of arithmetic operations follows PEMDAS. You can also use parentheses as long as they are balanced" << endl; 
 	cout << "\t3. SPECIAL KEYWORDS: > This program has only 4 special keywords, namely: PRINT, BEG, HELP, and EXIT!" << endl;
	cout << "\t                     > These keywords cannot be used as variable names." << endl;
	cout << "\t   Function of every special keywords: " << endl;
	cout << "\t       > PRINT *variable_name or literal* -> Prints the targeted variable or literal." << endl;
	cout << "\t       > BEG *variable_name* -> Asks for a value for the targeted variable." << endl;
	cout << "\t       > HELP -> prompts the general rules and guidelines of this program." << endl;
	cout << "\t       > EXIT! -> Exits this program" << endl;
	
	cout << "\n\n=============================================" << endl; 
	cout << "THANK YOU!" << endl; 
	system("pause");
}

//Pre-processing function that tokenizes the user command
vector<string> processInput(string input) {                                                            		
    
	//==========variable declaration===========//
	vector<string> user_command;                                                                      		//--> Vector of type string which stores the user command string into substrings
    string placeholder="";                                                                   				//--> Placeholder string           
    int i = 0; 																								//--> Counter variable
    bool flag = true; 																						//--> Flag  variable
    bool characterError = false;																			//--> Boolean variable that evaluates if there is a character error in the user input
	bool encounteredOperator = false;																		//--> Boolean variable that evaluates if we already met an operator in the user command string
	//=========================================//

	//Evaluates every character in the user command string
    while (i < input.length()) {                                                                      		//runs until the length of user command string

        if (isalnum(input[i]) || input[i] == '.') {                                                   		//checks if the current character is alphanumeric or a dot
            if (isOperator(placeholder[placeholder.size()-1])) {                                        	//if yes, it firstly checks if the last character on the placeholder string is an operator   
				if(isdigit(input[i]) ) {                              										 //if the upper if-statement is evaluated, it checks if the current char is a digi
					if(input[i-1] == ' ' && isValid(user_command.back()) && placeholder.at(placeholder.size()-1) == '-') {
                    	cout << "Unknown command! Does not match any valid command of the language." << endl; 
						user_command.clear();
                		return user_command;
					}
					placeholder += input[i];                                                              	//if the condition is met, it concatenates the digit to the placeholder string
                }
                else {																						//else, if the current character is not a digit
					if (characterError == true) {															//check first if characterError has been set to true. If yes,
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 						//print error statement.
                    	user_command.clear();
                    	return user_command;																//return an empty vector			
					}                                                                                  			
                    user_command.push_back(placeholder); 													//if not, then we push the placeholder to the user_command vector. This is another token
                    placeholder = input[i];                                                            		//re-set the placeholder string to the current char
                }
            }
            else {                                   														//if the last operator character on the placeholder is not an operator                                                   	
				placeholder += input[i];                 													//concatenate the current element on the placeholder string                                               	
            }
        }
		
		else if (input[i] == '(' || input[i] == ')') {														//if the current character is an opening or closing parenthesis
			if (input[i] == ')' && input[i-1] == '(' && i > 0) {
				cout << "Unknown command! Does not match any valid command of the language." << endl; 
				user_command.clear();
                return user_command;
			}
			if (isValid(placeholder)) {																		//check if the placeholder is a valid variable name
				user_command.push_back(placeholder); 															//pushes the placeholder as another token in the user_command vector
				if (isalpha(input[i+1])) {																	//if the next character in the input string is a letter,
						user_command.push_back(string(1, input[i]));											//we also push the parenthesis in the vector as a separate token
						placeholder = "";																		//reset placeholder
					}
				else placeholder = input[i];																//if not, then concatenate the parenthesis in the placeholder
			}
			else if (isdigit(input[i+1]) || input[i+1] == '(' || input[i+1] == ')' || isOperator(input[i+1])) {	//if not valid, then check the next character in the user input string
				placeholder += input[i];																			//if this if statement is met, then we concatenate the current character to the placeholder string
			}
			else { 																							//for other cases, 
				placeholder += input[i]; 																		//concatenate current input into the placeholder
				user_command.push_back(placeholder);															//push the placeholder string as a separate token
				placeholder = ""; 																				//reset placeholder
				
			}
		}
	
        else if (isOperator(input[i])) {                                                             		//checks if the current character is an operator
			if (containsNoChar(placeholder)) {                                                          	//if yes, we check if the placeholder string contains no character 
				if (placeholder != "") {
					//error checking if statement which evaluates the syntax of the input. Brute force to avoid any complications
					if ((input[i] != '-' || (input[i] == '-' && !isdigit(input[i+1]))) && placeholder.size()>1 && isOperator(placeholder[placeholder.size()-1])) {  
						cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
						user_command.clear();
	                   	return user_command;
					}           
					if(input[i] == '-' && !isdigit(input[i+1]) && (placeholder[placeholder.size()-1] == '(' || placeholder[placeholder.size()-1] == ')')) {
						cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
						user_command.clear();
		                return user_command;
					}
				}
//				if ((input[i] == '-' && !isdigit(input[i+1])  && isOperator(user_command.back()[user_command.back().size()-1]))) {
//					cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
//					user_command.clear();
//	                return user_command;
//				}
                placeholder += input[i];                                                                 	//if it doesn't contains any character, we concatenate the current input to the placeholder string
		    }                                                                                               
            else {                                                                                     		//else,
                if (placeholder != "") {                                                                 		//we check if the placeholder string is not empty
					if (characterError == true) {																//check if characterError is set to true
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					if (isdigit(input[i+1]) && input[i] == '-' && isValid(placeholder) && input[i-1] == ' ') {
						cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
						user_command.clear();
	                	return user_command;
					}
                    user_command.push_back(placeholder); 
//                        user_command.push_back(string(1, input[i])); 
//                        placeholder = "";
					placeholder = input[i];                                                                    	//reset placeholder
                }
            }
            encounteredOperator = true;
        }

        else if (input[i] == '=') {                                                                    	 	//check if the current character is an equal sign
            if (encounteredOperator == true) {
            	cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
            	user_command.clear();
            	return user_command;
			}
			if (placeholder != "") {                                                                     	//checks if the placeholder string is not empty
                if (characterError == true) {																	//check if characterError is set to true
					cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                   	user_command.clear();
                   	return user_command;
				}
				user_command.push_back(placeholder);                                                     	//place whatever in the placeholder to the user_command vector
            }
            user_command.push_back("=");                                                                 	//also, we push the equal sign to the vector
            placeholder = "";                                                                            	//reset the placeholder string to empty string
        }

        else if (input[i] == ' ') {                                                                    		//checks if the current character is a whitespace
            while (input[i] == ' ') {                                                                    	//This loop runs until the character is no longer a white space (eliminating extra ws)
                i++;                                                                                     	//increment i so we can access the next character in the input string
			}
			if (input[i] == '\0') {																			//if the current element now is a string terminator, then we break from the loop
				break;
			}
			//----------the lines until the end of this function just copies the same procedures when the input != ' ' since we're iterating the command here until the end of the excess spaces
            if (isOperator(input[i])) {                    	
				if (containsNoChar(placeholder)) {
					if (placeholder != "") {
						//brute force error checking for the syntax
	                    if (!isOperator(placeholder[placeholder.size()-1]) && input[i] == '-' && isdigit(input[i+1])) { 
							cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
	                    	user_command.clear();
	                    	return user_command;	
						}
						if (input[i] != '-' && placeholder.size() > 1 && isOperator(placeholder[placeholder.size()-1])) {
							cout << "SNOL> Unknown command! Does not match any valid command of the language.";
							user_command.clear();
	                   		return user_command;
						} 
						if ((input[i] != '-' || (input[i] == '-' && !isdigit(input[i+1]))) && placeholder.size() > 1 && isOperator(placeholder[placeholder.size()-1])) {
							cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
							user_command.clear();
                   			return user_command;
						}
						if(input[i] == '-' && !isdigit(input[i+1]) && (placeholder[placeholder.size()-1] == '(' || placeholder[placeholder.size()-1] == ')')) {
							cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
							user_command.clear();
		                	return user_command;
						}   
					}
					if ((input[i] == '-' && !isdigit(input[i+1])  && isOperator(user_command.back()[user_command.back().size()-1]))) {
						cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
						user_command.clear();
	                	return user_command;
					}
					placeholder += input[i];
                }
				 
                else {
                	//need og processing input 
                    if (placeholder != "") {
                    	if (characterError == true) {
							cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    		user_command.clear();
                    		return user_command;
						}
						if (isdigit(input[i+1]) && input[i] == '-' && isValid(placeholder)) {
							cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
							user_command.clear();
	                		return user_command;
						}
                        user_command.push_back(placeholder); 
						placeholder = input[i]; 
                    }
                }
                encounteredOperator = true;
            }
            
            else if (isalnum(input[i])) {																//if the current character is alphanumeric, then we concatenate it to the placeholder string													 
				if (isdigit(placeholder[placeholder.size()-1]) && input[i-1] == ' ') {
                	cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
                	user_command.clear();
                    return user_command;
				}
				if (isKeyword(placeholder) || isalpha(input[i])) {
					if (placeholder != "") {
						user_command.push_back(placeholder);
						placeholder = "";
					}
				} 
				if (isValid(placeholder) && input[i-1] == ' ') {
					cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
                	user_command.clear();
                    return user_command;
				}
//				if(isdigit(input[i]) && isValid(user_command.back()) && placeholder.at(placeholder.size()-1) == '-') {
//                    	cout << "SNOL> cdUnknown command! Does not match any valid command of the language." << endl; 
//						user_command.clear();
//                		return user_command;
//				}
				placeholder += input[i]; 
            }  

            else if (input[i] == '(' || input[i] == ')') { 
				if (input[i] == ')' && input[i-1] == '(' && i > 0) {
					cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
					user_command.clear();
                	return user_command;
				}
				
				if (isValid(placeholder)) {
					user_command.push_back(placeholder); 
					if (isalpha(input[i+1])) {
						user_command.push_back(string(1, input[i]));
						placeholder = "";
					}
					else placeholder = input[i];
				}
				else if (isdigit(input[i+1]) || input[i+1] == '(' || input[i+1] == ')' || isOperator(input[i+1])) {
					placeholder += input[i];
				}
				else {
					placeholder += input[i]; 
					user_command.push_back(placeholder);
					placeholder = ""; 
				}
			}

            else if (input[i] == '=') {                                                                  
				if (placeholder != "") {  
					if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}                                                                   				 
                    user_command.push_back(placeholder);                                                
                }
                user_command.push_back("=");                                                           
                placeholder = "";   
				                                                                    
            }
            
			//===============================================================//

            else {                                                                                     	 	//other cases
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {               	//we check if the last char in the placeholder string is not an operator and the placeholder string is not empty
                    if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					user_command.push_back(placeholder);                                                	//if the condition are met, we push whatever the content of placeholder string to the vector 
                    placeholder = "";    
                    placeholder += input[i];                                                                                                                      
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {                              		//else if the current character is an operator
                    if (!isdigit(input[i])) {                                                           		//check if current input is not a digit, then we push placeholder string to the vector
                        if (characterError == true) {	
							cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
	                    	user_command.clear();
	                    	return user_command;
						}
						if (isOperator(placeholder[placeholder.size()-1])) {
							cout << "yuh" << endl; 
							cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
							user_command.clear(); 
							return user_command; 
						}
						user_command.push_back(placeholder);                                             
                        placeholder = "";
                    }
                    placeholder += input[i]; 
                }                                              
                else {																						//else, an invalid character has been entered by the user. 
        			placeholder += input[i];																//add the invalid character to the placeholder string
		        	if (placeholder != "EXIT!") {															//If placeholder is not equal to EXIT! command, we set characterError bool to false
		        		characterError = true; 
					}
                }
            }
        }

       else {																								//else, an invalid character has been entered by the user. 
			placeholder += input[i];																		//add the invalid character to the placeholder string
        	if (placeholder != "EXIT!") {																	//If placeholder is not equal to EXIT! command, we set characterError bool to false
        		characterError = true; 
			}
    	}
    	
    	i++;
    }

    if (placeholder != "") {                                                                            	//If the placeholder string is not empty after the loop, we add whatever's left to the vector
        user_command.push_back(placeholder);
        placeholder = "";
    }
    
	//=====================================error checking==========================================//
    if (user_command.at(0) == "HELP" || user_command.at(0) == "EXIT!") {
    	cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
    	user_command.clear(); 
    	return user_command; 
	}
	
	if ((user_command.at(0) == "PRINT" || user_command.at(0) == "BEG") && user_command.size() > 2) {
		cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
    	user_command.clear(); 
    	return user_command; 
	}
	
	if (user_command.size() ==  2 && (isKeyword(user_command.at(0)))) {
		if (!isValid(user_command.at(1))) {
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
			user_command.clear(); 
			return user_command;
		}
	}
	
	if (user_command.size() == 1 && (isKeyword(user_command.at(0)))) {
		cout << "SNOL> Error! There are no target variable! Usage should be: " << user_command.at(0) << " [variable name]" << endl;
		user_command.clear(); 
    	return user_command;
	}
	
    
    for (int i = 0; i < user_command.size(); i++) {
    	if (isKeyword(user_command.at(i)) && i > 0) {
    		cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
    		user_command.clear(); 
    		return user_command; 
		}
		if(!containsNoChar(user_command.at(i)) && !isValid(user_command.at(i))) {
			cout << "SNOL> Error! Invalid variable name [" << user_command.at(i) <<"]" << endl;
			user_command.clear();
			return user_command; 
		}
		if (i == user_command.size() - 1) {
			string temp = user_command.at(i);
			if(isOperator(temp[temp.size()-1]) || temp[temp.size()-1] == '=') {
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
    		user_command.clear(); 
    		return user_command;
			}
		}
	}
//	
//	cout << "debug: " << endl; 
//	for (i = 0; i < user_command.size(); i++) {
//		cout << "TOKEN: " << user_command.at(i) << endl;
//	} 
//	
	
	//returns the error-checked user_command vector to the calling function. 
    return user_command;
}

bool errorChecking(vector<string> user_command){																		//bool errorChecking() function created by: Ferdinand A. Gador II
	
	bool stringChecker=false;
	bool invalidCharacter = false;																							//stringChecker returns a bool value to indicate if the user-input is valid or not
	int parenthesisChecker=0;																							//checks if the parentheses are equal and follows the proper formatting
	int openParenthesis=0;
	int closeParenthesis=0;
	bool isMismatch = false;																							//checks if parentheses are mismatched or not equal in an arithmetic equation
	
	if(user_command.size()==1){																							//if the size of the vector is 1, 
		
		if(user_command.at(0)!="PRINT" && user_command.at(0)!="BEG"){													//user input an expression with vector of size 1
			
			string tempString = user_command.at(0);																		//assign the value of the vector on a temporary string

			if(isdigit(tempString[0])||tempString[0]=='('||tempString[0]=='-'){																//if the first character of the string is an open parenthesis or digit, execute this code snippet

				for(int j=0;j<tempString.size();j++){																	//for loop scans every character of the string , checks for operators, parenthesis and integers	
																					
					if(isdigit(tempString[j])||tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%'||tempString[j]=='('||tempString[j]==')'||tempString[j]=='.'){
						if(tempString[j]=='('){
							parenthesisChecker++;
							openParenthesis++;
						}
						else if(tempString[j]==')'){
							if(parenthesisChecker!=0){
								parenthesisChecker--;
								closeParenthesis++;
							}
								else if(parenthesisChecker==0){
									closeParenthesis++;
									continue;
								}
						}
						stringChecker=true;																				//return true if string contains valid characters
					}
					else{
						stringChecker=false;																			//return false if the string contain invalid characters
						break;
					}
				}
			}
			
			else if(isalpha(tempString[0])||tempString[0]=='_'){														//if the string starts with letters or underscore

				for(int j=0;j<tempString.size();j++){																	//scan every character of the string
					
					if(isalnum(tempString[j])||tempString[j]=='_'){														//checks for alphanumeric characters and underscore
						stringChecker=true;																				//return true if valid characters are met
					}
					else{
						stringChecker=false; 																			// return false if invalid
						break;
					}
				}
			}else{;}
						
			if(parenthesisChecker==0&&(openParenthesis==closeParenthesis))stringChecker=true;
				else stringChecker=false;	
				
			if(stringChecker==false) cout << "SNOL> Unknown Word [" << tempString << "]" << endl; 						//if stringchecker is false, print the unknown word
			
		}else if(user_command.at(0)=="EXIT!")exit(0);																//if user input is EXIT, terminate the function immediately
		
	}else if(user_command.size()==2&&(user_command.at(0)=="PRINT"||user_command.at(0)=="BEG")){							//if the vectors has a size of two
		
		if(user_command.at(0)=="BEG"){																					//user calls the BEG function
			
			string tempString=user_command.at(1);																		//initialize the second string on a temporary string
			
			for(int j=0; j<tempString.length(); j++){																	//for loop that scans each character for variable initialization anomalies
				
				if(isdigit(tempString[0])){																				//checks if the first character of the variable is an integer										
					stringChecker = false;																				//return a false value, indicating invalid variable					
					break;																								//break the loop
				}
				else if(isalnum(tempString[j]) || tempString[j]=='_'){													//checks the string if it contains alphanumeric characters
						stringChecker = true;																			//return a true value, indicating valid variable
				}
				else{
					stringChecker = false;																				//the loop encountered and invalid character, returns a false value
					break;																								//break out of the loop
				}
			}
			
			if(stringChecker==false) cout << "SNOL> Unknown Word [" << tempString << "]" << endl; 						//if stringchecker is false, print the unknown word	
			
		}
		
		else if(user_command.at(0)=="PRINT"){ 																			//user calls the PRINT function
			
			string tempString=user_command.at(1); 																		//initialize the second string on a temporary string
			
				if(isdigit(tempString[0])||tempString[0]=='-'){		 													//if the first character of the string is an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						
						if(isdigit(tempString[j])||tempString[j]=='.'){													//if the loop encounters an integer or floating-point, it returns a true value
							stringChecker = true;
						}
						else{
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
				}
				else{																									//if the first character of the string is not an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						
						if(isalnum(tempString[j]) || tempString[j]=='_'){												//if the loop encounters an alphanumeric character, it returns a true value
							stringChecker = true;
						}
						else{																							
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
				}
				if(stringChecker==false) cout << "SNOL> Unknown Word [" << tempString << "]" << endl;					//if stringchecker is false, print the unknown word
			
		}else cout << "SNOL> Unknown Command! Does not match any valid command of the language" << endl; 				//if the user inserted an invalid command, print this statement
				
	}
	
	else{ 																												//assignment operation
		
		int equalCount=0; 																								//counts instances the 'equal' sign is used
		int operatorCount=0;																							//counts instances operator signs are used
		
		for(int i=0;i<user_command.size();i++){																			//for loop that scans every string inside the vector
			
			string tempString = user_command.at(i); 																	//assign the current vector inside a temporary string
			
			if(user_command.at(0)=="+"||user_command.at(0)=="-"||user_command.at(0)=="*"||user_command.at(0)=="/"||user_command.at(0)=="%"||user_command.at(0)=="="||user_command.at(0)=="("||user_command.at(0)==")"){
				stringChecker=false;
				break;
			}
			
			else{
				if(isalpha(tempString[0])){																				//if the string starts with a letter
					for(int j=0;j<tempString.size();j++){																//for loop scans for alphanumeric characters and underscore
						
						if(isalnum(tempString[j])||tempString[j]=='_'){
							stringChecker=true; 																		// return true if valid characters are met
						}
						else{
							stringChecker=false; 																		//return false if invalid characters are encountered
							break;																						//break out of the loop
						}
					}
				}
				
				else if(isdigit(tempString[0])||tempString[0]=='('||tempString[0]==')'||tempString[0]=='-'){ 								//if the string starts with an integer, and parenthesis
					for(int j=0;j<tempString.size();j++){ 																//for loop scans the string for valid characters
						
						if(isdigit(tempString[j])||tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%'||tempString[j]=='('||tempString[j]==')'||tempString[j]=='.'){
							if(tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%')operatorCount++;//if an operator is encountered, increment
							if(tempString[j]=='('){
								parenthesisChecker++;
								openParenthesis++;
							} 																						
							else if(tempString[j]==')'){
								if(parenthesisChecker!=0){
									parenthesisChecker--;
									closeParenthesis++;
								}else if(parenthesisChecker==0){
									closeParenthesis++;
									continue;
								}
							} 											
							stringChecker=true; 																		//return true value if valid character is met
						}
						else{
							stringChecker=false; 																		//return false if invalid character is encountered
							break;																						//break out of the loop
						}						
					}	 
				}

				else if(tempString[0]=='='){ 																			//if the vector contained an equal sign
					equalCount++; 																						//increment per equal signs encountered
				}
				
				else if(tempString[0]=='+'||tempString[0]=='+'||tempString[0]=='*'||tempString[0]=='/'||tempString[0]=='%'){ //if the vector contain operators
					operatorCount++;
					stringChecker=true; 																				//return true for every valid characters met
				}else{
					stringChecker=false;
					invalidCharacter=true;
				} 
			}	
		}
						
		if(parenthesisChecker==0&&(openParenthesis==closeParenthesis)&&invalidCharacter==false)stringChecker=true;
			else stringChecker=false;
		
		if(equalCount==0&&operatorCount==0) stringChecker=false;														//checks if the string contained an equal sign or an operator

		if(stringChecker==false) {
			if (isMismatch == true) cout <<"SNOL> Error! There are mismatched parentheses in the arithmetic operation!" << endl; 
			else cout << "SNOL> Unknown Command! Does not match any valid command of the language" << endl; 			//print this statement if the stringchecker is false
		}
	}
	
	return stringChecker;	
}

//This function serves as the second error checking function of this program. This validates the data types and some other error handling methods 
bool processElements(vector<string> user_command, vector<variables> &var_list) {				

	//===========variable declarations=============//													
	int i; 																				//--> Counter variable
	string input; 																		//--> Placeholder string 
	string bin = ""; 																	//--> Placeholder string																
	string toRemove = "()";																//--> Placeholder for the specific characters to be removed for error checking									 
	bool encounteredEqual = false; 														//--> Checks if we already encountered an equal sign
	bool encounteredOperator = false;													//--> Checks if we alreadt encountered an operator 
	bool undefinedVars = false; 														//--> Check if a specific variable name is undefined
	bool encounteredKeyword = false;													//--> Check if we already encoutered a special keyword (i.e., PRINT and BEG)
	bool encounteredModulo = false;														//--> Check if we already encountered modulo operator
	vector<string> stripedInput;														//--> Vector of type string that stores the cleaned command tokens					
	vector<string> undefined; 															//--> Vector of type string that stores undefined variables
	vector<string> varDataTypes; 														//--> Vector of type string that stores the data type of variables and numeric literals
	//=============================================//
	
	//=============cleaning of input===============//
	for (i = 0; i < user_command.size(); i++) {								
		input = user_command.at(i); 									
		
		//Removing parentheses on the user-input since we're only concerned about the data type checking on this function
		for (int j = 0; j < 2; j++) {									
			input.erase(remove(input.begin(), input.end(), toRemove[j]), input.end()); 		
		}
		if (input != "") {										
			stripedInput.push_back(input);												//stores the cleaned input to the vector stripedInput
		}
	}
	//===============================================//
	
//	for (int i = 0; i < stripedInput.size(); i++) {
//		cout << "INPUT: " << stripedInput.at(i) << endl; 
//	}
	
	//checks if the vector of cleaned input only contains one element
	if (stripedInput.size() == 1) { 								
		//If yes, it will check if the the element is a valid variable name and if it is not yet used. If yes, it will display an error	
		if (isValid(stripedInput.at(0)) && isUsed(var_list, stripedInput.at(0))) {			
			cout << "SNOL> Error! [" << user_command.at(0) << "] is not defined!" << endl;		
			return false;																//return false to the main function									
		}
		//else, return true
		else if (isValid(stripedInput.at(0)) && !isUsed(var_list, stripedInput.at(0))) return true;											
	}
	
	//Check if the first character on the first element on the input vector is an operator or an equal sign. If yes, it will result to an error
	if (isOperator(stripedInput.at(0)[0]) || stripedInput.at(0) == "=") {					
		cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
		return false; 										
	}
	
	//======================evaluating========================//
	for (i = 0; i < stripedInput.size(); i++) {							
		input = stripedInput.at(i);									
		
		//==================syntax checking=================//
		//These two if-statement checks if the input contains a variable or a literal not being used for an operation
		//Example: var1 = var2 var3*4. This is an invalid syntax. 

//		if (stripedInput.size() > 3 && input.size() == 1 && i > 1 && !isOperator(stripedInput.at(i)[0]) && !isOperator(stripedInput.at(i-1)[0])) {
//			cout << "ANYWAYS..." << endl; 
//			cout <<  stripedInput.at(i)[0] << endl; 
//			cout << stripedInput.at(i-1)[0] << endl; 
//			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
//			return false; 
//		}
		
		if (stripedInput.size() > 2 && isValid(input) && i > 1 && (!isOperator(stripedInput.at(i-1)[stripedInput.at(i-1).size()-1]) && stripedInput.at(i-1) != "=")) {
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
			return false; 
		}
		//==================================================//

		//checks if the current iteration on the vector is a valid variable name and not a special keyword
		if(isValid(input) && !isKeyword(input)) {								
			if(!isUsed(var_list, input)) {															//check if the variable is already used								
				varDataTypes.push_back(extractDataType(var_list, input));							//if yes, we store the data type of that variable to the varDataTypes vector
			}
			else {																					//else, 
				if (i > 0 || isOperator(stripedInput.at(i+1)[0])) {									//we check if the current iteration is greater than one OR if the next element in the command vector is an operator
					undefined.push_back(input);														//if yes, then we push that variable to undefined vector. This is an error. 
					undefinedVars = true;																	
				} 
			}
		}
		
		//else, checks if the current iteration on the vector is an equal sign. We set the encounteredEqual variable to true
		else if (input == "=") {										
			encounteredEqual = true;									
		}
		
		else if (input == "MOD") {
			encounteredModulo = true;
		}
		
		//else, checks if the current interation on the vector is an arithmetic operation involving literals
		else if (!isValid(input) && containsNoChar (input)) {						
			
			for (int j = 0; j < input.size(); j++) {												//this for loop runs the whole characters in the input string				
				if (isdigit(input.at(j)) || input.at(j) == '.') {									//checks if the current iteration is a digit or a period							
					bin += input.at(j);																//concatenate the character to the string bin
				}
				
				else {																				//else, check for the following classification
					if (isOperator(input.at(j))) {													//if the current character is an operator, set the encounteredOperator variable to true
						encounteredOperator = true;	
						if(input.at(j) == '%') {
							encounteredModulo = true;	
						}					
					}
					if(!isOperand(bin) && bin != "") {												//if not an operator and the bin contains something, this will result to an error
						cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 
						return false;								
					}
					if (!isInteger(bin) && bin !="") {												//if bin is not an integer, automatically assume that it is a floating variable				
						varDataTypes.push_back("Float"); 											//pushback Float to the vector of datatypes				
//						cout << bin << endl;
						bin = "";																	//reset bin							
					}	
					else if (isInteger(bin) && bin != "") {											//if bin is an integer, we pushback Int instead to the vector of datatypes
						varDataTypes.push_back("Int");						
//						cout << bin << endl; 
						bin = "";								
					}
				}
			}
			
			if(!isOperand(bin) && bin != "") {														//evaluate the bin if it contains some value after the loop. Same logic above					
				cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 		
					return false;									
				}
				if (!isInteger(bin) && bin !="") {							
					varDataTypes.push_back("Float"); 					
					bin = "";								
				}	
				else if (isInteger(bin) && bin != "") {						
					varDataTypes.push_back("Int");						
					bin = "";									
				}
			}
		
		else if (isKeyword(input)) encounteredKeyword = true;										//checks if the input is a keyword					
	}
	
	if (undefinedVars == true ) {																	//if there exist an undefined variable, we evaluate the test cases.									
		if (undefined.size() == 1 && stripedInput.at(0) == "BEG") {;								//1: if the undefined vector size is equal to 1 and the first element on the user command is the keyword BEG				
		}																							//we ignore the existence of this undefined variable.
		else {																						//2: other cases. They will result immediately to an error
			cout << "SNOL> Error! Undefined variables: "; 							
			for (int i = 0; i < undefined.size(); i++) {							
				cout << "[" << undefined.at(i) << "] ";			
			}
			cout << "\n"; 
			return false;											
		}
	}
	
	//if we encountered an equal sign and the first element in the user command vector is an existing variable, we remove that first variable
	//stored on the data varDataTypes vector
	if(encounteredEqual == true && !isUsed(var_list, stripedInput.at(0))) {						
		varDataTypes.erase(varDataTypes.begin());								
	}
	
	if (varDataTypes.size() > 1) {																	//evaluate varDataTypes vector only if it contains more than one element
		for (int i = 0; i < varDataTypes.size() - 1; i++) {											//trace the whole varDataTypes vector
			string str1 = varDataTypes.at(i);														//extract the data type of the element at i and store it to a string
			string str2 = varDataTypes.at(i+1);														//extract the data type of the element at i+1 and store it to a string
			if(str1 != str2) {																		//if two string is not equal, then the command contains data types that are not the same
				if (!(encounteredEqual == true && stripedInput.size() == 3 && encounteredOperator == false)) {
					cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
					return false;
				} 
			}
			if ((str1 == "Float" || str2 == "Float") && encounteredModulo == true) {
				cout << "SNOL> Error! Modulo operations can only be used by integer values!" << endl;
				return false;
			}
		}
	} 
	
	return true; 																					//if the function reached this point, then we return true.										
}

//function that stores a given variable in the masterlist of variables
void variable_history(vector<variables> &var_list, vector <string> &user_command){
	if(user_command[0]== "BEG" && isUsed(var_list, user_command[1])){								//for cases of beg which is an elaborate assignment operator, exceptions are made
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(1);
	}
	
	if(!isUsed(var_list, user_command[0]) || isKeyword(user_command[0])) return;					//to prevent submitting keywords as variables as well as variable resubmission 
	

	if(user_command.at(0) != "" && isValid(user_command.at(0))){  									//if first element is variable, save it to the masterlist, assume its an assignment statement
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(0);
	}

}

int string_size(string input){																		//string size
	int i;
	for(i=0; input[i] != '\0';) i++;	
	return i;
}


int search(string &input, string &varname ){
	int size = string_size(input);	
	for(int i=0; i < size;i++){ 
		string holder = "";
		
		while(1){																					//concatenate until an operator is found
			holder=holder + input[i];
			
			if(input[i]=='(' || input[i]=='-')break;
			
			i++;
			if(i == size|| isOperator(input[i]) || input[i] == '(' || input[i] == ')'){ 
				break;
			}
		
		}
		
		if(holder == varname){																		//if equal return the subscript (the adress of the first char of the variable to be replaced)
			return i-string_size(holder);
		}
	
	}	
	return -1; 																						//returns -1 if none found
}


string evaluate(string &input, vector <variables> &var_list){										//replaces all variables with their corresponding values for easier solving
	int i=0,j=1;
	
	while(i < j){

		j=string_size(input);			
		if(!isdigit(input[i]) && !isOperator(input[i])){											//if it is not a digit and not an operator
			//check for Mod
			string mod = "MOD";
			int found = search(input, mod);															//made specifically for MOD keyword, replace it with %
			if(found != -1){
				input.replace(found, found+2, "%");
				continue;
			}
			
			int k=0, l=0;
			while(k < var_list.size()){																//find and replace all present variables in the string	
				int found = search(input, var_list[k].name);										//check if current variable is present on the input string
				
				if(found != -1){	 
					input.replace(found, string_size(var_list[k].name), var_list[k].value);
				}
				else{
					k++;
				}
			}			
		}
		i++;
	}
	
	return input;
}


string concatenate(vector<string> user_command, int starting_point){						//given a starting point, combines all desparate strings from vector into 1 string
	int j = starting_point;
	string input ="";
		for(j; j< user_command.size(); j++){
				
				input = input + user_command[j];
			}
	return input;
}
	
void semantics(vector <variables> &var_list, vector <string> &user_command){				//due to nature of switch only accepting int and does not handle string well, if are used throughout

	if(!isUsed(var_list, user_command[0]) && user_command.at(1) == "="){					//check if assignment, variable in first element, '=' on the second
		int i=0;
	
		while(i < var_list.size()){															//search for the variable specified
			if(var_list[i].name == user_command[0]) {										//once found, concatenate the rest into 1 r-value, evaluates it and assigns it to var
				string input = concatenate(user_command, 2);
				var_list[i].value = arithmetic(evaluate(input, var_list));
				//place arithmetic function here
				
				if(!isInteger(var_list[i].value))var_list[i].type = "Float";				//determine if data type is float or integer
				else var_list[i].type = "Int";
				
				
				break;	
			}
			i++;
		}		
	}
	//check if keyword
	else if(isKeyword(user_command[0])){
		if(user_command[0] == "PRINT"){		//if first user command input in string is PRINT, assume print statement
			string input = concatenate(user_command, 1);
			input = evaluate(input, var_list);

			cout<<"SNOL> ["<<user_command[1]<<"]: " << input << endl;
		}
		
		else if(user_command[0] == "BEG"){ //if first user command input in string is BEG, assume beg statement
			int i=0;
			string input;

			while(i < var_list.size()){
				if(var_list[i].name == user_command[1]) {
					cout<<"SNOL> Please enter value for ["<<user_command[1]<<"]"<<endl<<"Input: ";
					getline(cin, input);
					
					//error checking: since the user needs to input valid integer or floating-point values
					if (!isInteger(input) && !isFloat(input)) {
						cout << "SNOL> Error! Please input INTEGER or FLOATING-POINT values only!" << endl; 
						fflush(stdin);
						break;
					}
					
					//place arithmetic function here
					var_list[i].value = arithmetic(evaluate(input, var_list));
					
					if(!isInteger(var_list[i].value))var_list[i].type = "Float";		//determine if data type is float or integer
					else var_list[i].type = "Int";
					
					
					fflush(stdin);
					break;
				}
				i++;
			}			
		}		
	}
	//if not an assignment nor a keyword, assume an arithmetic
	else{
		string temp = concatenate(user_command, 0);
		temp = evaluate(temp, var_list);
		temp = arithmetic(temp);
		
		//place arithmetic function here
	}
}

//main function to be called to manipulate then solve expression before converting answer to string
string arithmetic(string expression){
	bool floatOperands = false;
    for(unsigned i = 0; i < expression.length(); i++){									//loop to manipulate expression
    	//if close then open parentheses encountered without operators before or after, respectively
        if (expression[i] == '.') {														//if a point(.) is encountered, we assumed that the whole expression should yield to a floating-point value
			floatOperands = true;
		}
    	if(i>0 && ((expression[i] == '(' && (isdigit(expression[i-1]) || expression[i-1] == ')')) || (i>0 && isdigit(expression[i]) && expression[i-1] == ')')))
    		expression.insert(i, 1, '*');												//adds multiplication symbol in between
        else if(expression[i] != '-')
			continue;
		else if(i + 1 < expression.length() && isOperator(expression[i + 1]))
			continue;
        if (i == 0 || isOperator(expression[i - 1])|| expression[i - 1] == '(' )		//if negative number encountered
            expression[i] = '~';														//changes '-' before it to  '~' to avoid confusion
	} 
    
	try{																				//block of code to try
		float floatAns = solvePostfix(tokenize(infixToPostfix(expression)));			//solve for answer which is returned as float
		ostringstream ostr;																//convert float answer to string
		ostr<<floatAns;
		string stringAns = ostr.str();
		if(isInteger(stringAns) && floatOperands == true) { 							//if the converted answer yields to an integer value but the operands are floating point,
			stringAns += ".0";															// we concatenate '.0' to the stringAns in order to return a floating point value
		}
		if (isFloat(stringAns) && floatOperands == false) {
			//di pa ko sure ani
		}
		return stringAns;																//return string answer	
		throw (floatAns);
	}
	catch(...){																			//upon exception, print error message
		cout << "\nSNOL> Error! Invalid arithmetic equation!\n";
	}													
}//end of arithmetic function

//function that converts infix string to postfix string
string infixToPostfix(string infix){
    string postfix;
    stack<char> charStack;

    for (unsigned i = 0; i < infix.length(); i++){
        
        if ((isdigit(infix[i])) || (infix[i] == '.') || (infix[i] == '~'))				//numbers moved to postfix string
            postfix += infix[i];
        
        else if (isspace(infix[i]))														//ignore spaces
            continue;	
        
        else if (isOperator(infix[i])){													//if we get valid operators
            postfix += " ";
            //while stack isn't empty, check if operator in stack has higher precedence than the current one from infix. If so, move stack operator to postfix string
            while ((!charStack.empty()) &&(getPrecedence(charStack.top()) >= getPrecedence(infix[i]))){
                postfix += charStack.top();
                charStack.pop();
            }
            
            charStack.push(infix[i]);													//push the current infix operator to top of stack
        }
        
        else if (infix[i] == '(')														// push opening parentheses to stack
            charStack.push(infix[i]);
        
        else if (infix[i] == ')'){														//when closing parenthesis is encountered
            while ((!charStack.empty()) && (charStack.top() != '(')){					//pop operator from top of stack until encounter another opening parenthesis or stack is emptied
                postfix += charStack.top();    											//place the math operator onto the postfix string
                charStack.pop();
            }
            if (!charStack.empty()) 													//if stack isn't empty, it means opening parenthesis is on top so pop it
                charStack.pop();
            else
				return NULL;
        }

        else
            return NULL;
    }
 
    
    while (!charStack.empty()){															// move remaining math operators (if any) from the stack to postfix array
        postfix += charStack.top();
        charStack.pop();
    }
    return postfix;
}// end of function infixToPostfix

int getPrecedence(char operation){														//function that returns a designated precedence for valid operators
    int priority = 0;
    switch (operation){
		case '*': case '/': case '%':
	        priority = 3;
	        break;
	    case '-':
	        priority = 2;
	        break;
	    case '+':
	        priority = 1;
	        break;
    }
    return priority;
}//end of function getPrecedence

vector<string> tokenize(string postfix){												//function that tokenizes a postfix string
	
	//Finite State Machine(FSM)
	int stateTable[][7] = { {0, INTEGER,  REAL, NEGATIVE, OPERATOR,  UNKNOWN,  SPACE},
							{INTEGER,  INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT},
							{REAL,       REAL,  REJECT,  REJECT,  REJECT,   REJECT,  REJECT},
							{NEGATIVE, INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT},
							{OPERATOR,  REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT},
							{UNKNOWN,   REJECT, REJECT,  REJECT,  REJECT,   UNKNOWN, REJECT},
							{SPACE,     REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT} };
		
    vector<string> tokens;
    char currChar = ' ';
    int col = REJECT, currState = REJECT;
    string currToken = "";

    for (unsigned i = 0; i < postfix.length();){
        currChar = postfix[i];
        col = getState(currChar);														//get designated column of current character
        if ((currState == REAL) && (col == REAL)){
            cerr << "\nSNOL> Error! Invalid Number Format. Multiple decimal points found.\n";
            return vector<string>();													//return empty vector
        }
        currState = stateTable[currState][col];											//get current state according to FSM
        
        if (currState == REJECT){														//REJECT means successfully tokenized
            if (currToken != " ")
                tokens.push_back(currToken);
            currToken = "";
        }
        
        else{																			//otherwise, we keep building next token until successful
            currToken += currChar;
            i++;
        }
    }
    
    if (currToken != " ")																//double check to make sure last token is saved
        tokens.push_back(currToken);
    return tokens;
}// end of function tokenize

int getState(char &currChar){															//function that returns the state of a character being tokenized
    if(isspace(currChar))
        return SPACE;
    else if(isdigit(currChar))
        return INTEGER;
    else if(currChar == '.')
        return REAL;
    else if(currChar == '~'){
		currChar = '-';
        return NEGATIVE;
    }
    else if (isOperator(currChar))
		return OPERATOR;
    return UNKNOWN;
}// end of function getState

float solvePostfix(const vector<string>& postfix){										//function that solves the now postfix expression
    float op1 = 0, op2 = 0;
    float answer = 0;
    stack<float> floatStack;

    for (unsigned i = 0; i < postfix.size(); i++){
        //push positive numbers and decimal points to stack
        if ((isdigit(postfix[i][0])) || (postfix[i][0] == '.'))
            floatStack.push(atof(postfix[i].c_str()));
        //push negative numbers onto the stack
        else if ((postfix[i].length() > 1) && ((postfix[i][0] == '-') && (isdigit(postfix[i][1]) || (postfix[i][1] == '.'))))
            floatStack.push(atof(postfix[i].c_str()));
        
        else if (isOperator(postfix[i][0]) && (!floatStack.empty())){				//if expression is an operator, pop numbers from stack and send them to function performOp
            char token = postfix[i][0];
 
            if (floatStack.size() > 1){
                op2 = floatStack.top();
                floatStack.pop();
                op1 = floatStack.top();
                floatStack.pop();
                answer = performOp(token, op1, op2);
                floatStack.push(answer);
            }
        }
    }
    
    if (!floatStack.empty())															// pop the final answer from the stack, and return to main
        answer = floatStack.top();
    return answer;
}// end of solvePostfix

float performOp(char operation, float op1, float op2){									//helper function for solvePostfix to perform individual operations
    float result;
    switch (operation){
	    case '+':
	        result = op1 + op2;
	        break;
	    case '-':
	        result = op1 - op2;
	        break;
	    case '*':
	        result = op1 * op2;
	        break;
	    case '/':
	        result = op1 / op2;
	        break;
	    case '%':
	        result = ((int)op1 % (int)op2) + modf(op1, &op2);
	        break;
	    default:
	    	cout << "\nSNOL> Error! Invalid arithmetic equation!\n";					//upon checking, this function will never reach this part due to pre-error checking of the user command
	        break;
    }
    return result;
}// end of function performOp


//===============================helper functions===============================//
bool isOperator (char ch) {                                                            	//boolean function that checks if a character is an operator
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool isKeyword(string input){
	if(input == "BEG" || input == "PRINT" || input == "HELP") return true;				//if the set variable is a keyword
	else return false;
}

bool containsNoChar (string input) {                                                    //boolean function that checks if a string contains alphabet charaters
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

bool isValid(string input){
	int i = 0;
	int valid = true;																	//initialize valid as true	

		if(isalpha(input[i])) valid = true;												//if each element of the input has no unnecessary characters														//if the input is only alphanumeric	that start with a letter
		else valid = false;																//else invalid
		
		if(valid == true){																//while it is valid
		  	for(i = 1; i < input.size(); i++) {											//scan each element of the string
		  	   	if(!(isalpha(input[i]) || isdigit(input[i]) || input[i] == '_')){		//if there is unneccessary characters then invalid
					valid = false;
					break;
				}
		  	}
		  	   return true; 															//if the variable is valid, it will return true
		  }
	    else return false;																//else not
}

bool isUsed(vector<variables> var_list, string input) {									//boolean function that checks if the variable already exists in the list		
	int i=0;																	
	int valid = true;
	
	while(i < var_list.size()){															//Runs the whole list until the input is found in the list
		if(var_list[i].name == input){
			return false;																
		}		
		i++;
	}
	return true;
}

//Reference: Stackoverflow
bool isInteger(string input){															//boolean function to test if the input is an integer
   	istringstream iss(input); 															//istringstream means to copy the value of the input
    int f; 																				//declare an integer variable
    iss >> noskipws >> f; 																//produced an integer type
    return iss.eof() && !iss.fail(); 													//return true if the input is an integer
}

bool isFloat(string input) {															//boolean function to test id the input is a  floating-point
    istringstream iss(input); 															//istringstream means to copy the value of the input
    float f; 																			//declare a floating-point variable
    iss >> noskipws >> f; 																//produced a floating-point type
    return iss.eof() && !iss.fail(); 													//return true if the input is a floating-point
}

bool isOperand(string input) {															//boolean function that checks the input if it is an operand
	bool flag = true; 																	//initialize flag as true
	if(!isInteger(input) && !isFloat(input)) flag = false;								//if input is not an operand return false
	return flag; 																		//return true if the input is an operand
}

string extractDataType (vector<variables> &var_list, string var_name) {				 	 //This function extracts the data type of an existing variable in the list
	for (int i = 0; i < var_list.size(); i++) {											//Iterates until the var_name is equal to the current iteration
		if (var_list.at(i).name == var_name) {
			return var_list.at(i).type;													//returns the data type
		}
	}
	return "";																			//the function will never really reach this part.
}
