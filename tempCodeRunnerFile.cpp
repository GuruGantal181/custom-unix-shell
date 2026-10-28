#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>

using namespace std;

class UnixShell{
public:
    void run(){
        while(true){
            cout << "user@unix-shell_$ ";
            string command;
            getline(cin,command );
            
            if(command == "exit"){
                break;
            }

            if(command.empty()) continue;

            vector <string> tokens = parseCommand(command);

            for(const string &token: tokens){
                cout << token << " ";
            }
            cout << "\n";
        }
    }

private:
    vector<string> parseCommand(const string& command){
        vector <string> tokens;
        string word = "";
        for(int i= 0; i < command.size(); i++){
            if(command[i] !=' ' && command[i]  != '\n' && command[i] != '\t'){
                word.push_back(command[i]);
            }
            else if(word.size() >0){
                tokens.push_back(word);
                word = "";
            }
        }
        if(word.size() > 0) 
            tokens.push_back(word);
        return tokens;
    }
};



int main(){
    UnixShell shell;
    shell.run();
}