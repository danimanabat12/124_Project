#include<iostream>
#include<vector> 
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>

using namespace std; 

/*
bugs created:
pressing enter on blank input results to an error
inputting the variable/keyword only results to an error

summary of functions made

book isKeyword - returns true if the string is a keyword(BEG, PRINT, MOD)
bool isValid - returns true if the string satisfies variable name req.
bool isInteger - returns true if the string is an integer
bool isFloat -returns true if the string is a float
bool isOperator - returns true if the char is an operator symbol
bool isOperand - returns true if the string is a valid operand
bool comparison - compares two values if they have the same data type. --> wala pa ni nagamit

bool containsNoChar - returns true if there is no alphabet in the string
vector<string> processInput - accepts string input from user, returns vector with the inputs separated according to their 'tokens'
bool processElements - evaluates the data type of the input
void updateDataType - updates the data type of a given variable
string extractDataType - returns the data type of a given variable

bool isUsed - returns true if the string is a variable name that does not exist
void variable_history - updates the masterlist of all variables inputted by the user
void semantics - processes the intent and works through the commands of the user
int string_size - accepts a string input, returns its size (for non const strings)
string evaluate - replaces all variables with their corresponding values for easier solving
string concatenate - given a starting point, combines all desparate strings from vector into 1
*/


typedef struct variables {                                                                          //typedef struct for the variables 
    string name; 
    string value; 
    string type; 
}variables;


bool isOperator (char ch) {                                                                         //boolean function that checks if a character is an operator
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool isKeyword(string input){
	if(input == "BEG" || input == "PRINT" || input == "MOD") return true;		//if the set variable is a keyword
	else return false;
}

bool containsNoChar (string input) {                                                                //boolean function that checks if a string contains alphabet charaters
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

bool isValid(string input){
	int i = 0;
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

bool isUsed(vector<variables> var_list, string input) {
	int i=0;
	int valid = true;
	
	while(i < var_list.size()){
		if(var_list[i].name == input){
			return false;
		}		
		i++;
	}
	return true;
}

bool isInteger(string input){
   	istringstream iss(input); 
    int f; 
    iss >> noskipws >> f; 
    return iss.eof() && !iss.fail(); 
}

bool isFloat(string input) {
    istringstream iss(input); 
    float f; 
    iss >> noskipws >> f; 
    return iss.eof() && !iss.fail(); 
}

bool isOperand(string input) {
	bool flag = true; 
	if(!isInteger(input) && !isFloat(input)) flag = false;
	return flag; 
}

void updateDataType (vector<variables> &var_list, string var_name, string data_type) {
	for (int i = 0; i < var_list.size(); i++) {
		if (var_list.at(i).name == var_name) {
			var_list.at(i).type = data_type;
			break;
		}
	}
} 

string extractDataType (vector<variables> &var_list, string var_name) {
	for (int i = 0; i < var_list.size(); i++) {
		if (var_list.at(i).name == var_name) {
			return var_list.at(i).type;
		}
	}
	return "";
}

bool comparison(string input1, string input2){
	if(isInteger(input1) == isInteger(input2)) return true;
	else if(isFloat(input1) == isFloat(input2)) return true;
	else return false;
}


bool processElements(vector<string> user_command, vector<variables> &var_list){
	
	int var_count = 0;
	int i = 0;
	string input;
	string bin = "";
	string dataType; 
	string var_name = ""; 
	bool encounteredEqual = false;
	bool containsFloat = false;
	// vector<string> index;
	
	for (i; i < user_command.size(); i++) {
        input = user_command.at(i);															//set input as the holder for the each value of the vector at specific position
        
    	if(isValid(input) && !isKeyword(user_command.at(i))){								//if the input is a string then it is a variable subjected for validation.
			if (isUsed(var_list, input) && var_count == 0) {
				variables input_var;
				input_var.name = input; 
				input_var.type = "NULL";
				input_var.value = "";
				var_list.push_back(input_var);
				var_count++;
			}
			else if (!isUsed(var_list,input) && var_count > 1 && encounteredEqual == true) {
				dataType = extractDataType(var_list, user_command.at(i)); 
				updateDataType(var_list, var_name, dataType);
			}
			else if (isUsed(var_list, input)) {
				cout << "SNOL> Error! [" << user_command.at(i) <<"] is not defined!" << endl; 
				return false;
			}
			var_name = input;
		}
		
		else if(user_command.at(i) == "="){													//else it is an assignment operator
			encounteredEqual = true;
		}
		
		else if(!isValid(user_command.at(i)) && containsNoChar(user_command.at(i))){		//else it is an expression
	        
			for(int j = 0; j < input.size(); j++){											//iterate the size of the value of the vector to check its element				
        		
        		if(isdigit(input.at(j))) { 
					bin += input.at(j);
				}
				else if (input.at(j) == '.') {
					bin+= input.at(j);
				}
				else {
					// index.push_back(bin);
					if (!isOperand(bin)) {
						cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 
						return false;
					} 
					if (!isInteger(bin)) {
						containsFloat = true;
					} 
					bin="";
				} 
			}
			// if (bin != "") index.push_back(bin);
			if (!isOperand(bin)) {
				cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 
				return false;
			} 
			if (!isInteger(bin)) {
				containsFloat = true;
			} 
			
			if  (containsFloat == true)updateDataType(var_list, var_name, "float");
			else updateDataType(var_list, var_name, "int");
		}
		
		else if(isKeyword(user_command.at(i))) {
		}
   
	}

	return true;
}

vector<string> processInput(string input) {                                                            //function that process that user command
    vector<string> user_command;                                                                      //vector of type string which stores the user command string into substrings
    string placeholder="";                                                                              
    int i = 0; 
    bool flag = true; 

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
            else if (input[i] == '=') {                                                                    //check if the current character is an equal sign
                if (placeholder != "") {                                                                     //checks if the placeholder string is not empty
                    user_command.push_back(placeholder);                                                     //place whatever in the placeholder to the user_command vector
                }
                user_command.push_back("=");                                                                 //also, we push the equal sign to the vector
                placeholder = "";                                                                            //reset the placeholder string to empty string
            }
            else {                                                                                     //if the current character is not an operator
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {               //we check if the last char in the placeholder string is not an operator and the placeholder string is not empty
                    user_command.push_back(placeholder);                                                 //if the condition are met, we push whatever the content of placeholder string to the vector 
                    placeholder = "";    
                    placeholder += input[i];                                                                 //reset the placeholder string to empty                                                       
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {                              //else if the current character is an operator
                    if (!isdigit(input[i])) {                                                           //check if current input is not a digit, then we push placeholder string to the vector
                        user_command.push_back(placeholder);                                             
                        placeholder = "";
                    }
                    placeholder += input[i]; 
                }  
                else if (isalnum(input[i])) {
                    placeholder += input[i];
                }                                                      
                else {
                    cout << "SNOL> Invalid input. Characters like [" << input[i] <<"] is not allowed." << endl; 
                    user_command.clear();
                    return user_command;
                }
            }
        }
        else {
            cout << "SNOL> Invalid input. Characters like [" << input[i] <<"] is not allowed." << endl; 
            user_command.clear();
            return user_command;
        }
        i++;
    }

    if (placeholder != "") {                                                                            //If the placeholder string is not empty after the loop, we add whatever's left to the vector
        user_command.push_back(placeholder);
        placeholder = "";
    }

    return user_command;
}

void variable_history(vector<variables> &var_list, vector <string> &user_command){
	if(user_command[0]== "BEG" && isUsed(var_list, user_command[1])){			//for cases of beg which is an elaborate assignment operator, exceptions are made
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(1);
	}
	
	if(!isUsed(var_list, user_command[0]) || isKeyword(user_command[0])) return;	//to prevent submitting keywords as variables as well as variable resubmission 
	

	if(user_command.at(0) != "" && isValid(user_command.at(0))){  					//if first element is variable, save it to the masterlist, assume its an assignment statement
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(0);
	}

}

int string_size(string input){														//string size
	int i;
	for(i=0; input[i] != '\0';) i++;	
	return i;
}

string evaluate(string &input, vector <variables> &var_list){						//replaces all variables with their corresponding values for easier solving
	int i=0,j=1;

	while(i < j){
		j=string_size(input);			
		if(!isdigit(input[i]) && !isOperator(input[i])){											//if it is not a digit and not an operator
			//check for Mod
			std::size_t found = input.find("MOD"); 													//made specifically for MOD keyword, replace it with %
			if(found != std::string::npos){
				input.replace(found, found+2, "%");
				continue;
			}
			int k=0;

			while(k < var_list.size()){																//find and replace all present variables in the string
				string check = var_list.at(k).name;	
				found = input.find(check);
				if(found != std::string::npos){
					input.replace(found, found + string_size(check), var_list.at(k).value);
					continue;
				}
				k++;
			}			
		}
		i++;
	}
	
	cout<<"post eval:"<<input<<endl;
	return input;
}

string concatenate(vector<string> user_command, int starting_point){							//	 given a starting point, combines all desparate strings from vector into 1 string
	int j = starting_point;
	string input ="";
		for(j; j< user_command.size(); j++){
				
				input = input + user_command[j];
			}
	return input;
}
	
void semantics(vector <variables> &var_list, vector <string> &user_command){					//due to nature of switch only accepting int and does not handle string well, if are used throughout

	if(!isUsed(var_list, user_command[0]) && user_command.at(1) == "="){						//check if assignment, variable in first element, '=' on the second
		int i=0;
	
		while(i < var_list.size()){																//search for the variable specified
			if(var_list[i].name == user_command[0]) {											//once found, concatenate the rest into 1 r-value, evaluates it and assigns it to var
				string input = concatenate(user_command, 2);
				var_list[i].value = evaluate(input, var_list);
				//place arithmetic function here
				
				if(!isInteger(var_list[i].value))var_list[i].type = "Float";					//determine if data type is float or integer
				else var_list[i].type = "Int";
				
				
				cout<<"datatype of "<<var_list[i].name<<" is "<<var_list[i].type<<endl;
				break;	
			}
			i++;
		}		
	}
	//check if keyword
	else if(isKeyword(user_command[0])){
		if(user_command[0] == "PRINT"){
			string input = concatenate(user_command, 1);
			input = evaluate(input, var_list);
			//evaluate arithmetic here
			cout<<input<<endl;
		}
		
		else if(user_command[0] == "BEG"){ 
			int i=0;
			string input;

			while(i < var_list.size()){
				if(var_list[i].name == user_command[1]) {
					cout<<"Please enter value for ["<<user_command[1]<<"]"<<endl<<"Input:";
					cin>>input;
					
					//place arithmetic function here
					var_list[i].value= evaluate(input, var_list);
					if(!isInteger(var_list[i].value))var_list[i].type = "Float";					//determine if data type is float or integer
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
		evaluate(user_command[0], var_list);
		//place arithmetic function here
	}
}

int main() {
    string input;
	vector<variables> var_list;
	
    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    while(input != "EXIT!") {
        cout << "\nCommand: ";
        getline(cin, input);
        vector<string> user_command = processInput(input);        
		//processElements(user_command, var_list);
        //assume input already error checked.
        //assume input data types are already identified
		if (processElements(user_command, var_list)) {
			 variable_history(var_list, user_command);
         	  semantics(var_list, user_command);  
		}
    }

    cout << "\nInterface is now terminated..." << endl;
}
