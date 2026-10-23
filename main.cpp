#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <functional>
#include <unordered_map>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

class UnixShell{
private:
unordered_map <string, function<void(const vector<string>&)>> commands;

    void setupCommands(){
        commands["mkdir"] = [this](const vector <string>& args){
            executeMkdir(args);
        };
        commands["cd"] = [this](const vector <string>& args){
            executeCd(args);
        };
        commands["ls"] = [this](const vector <string>& args){
            executeLs(args);
        }
    }

    void executeMkdir(const vector <string> &args){
        if(args.size() == 1){
            cout << args[0] << ": missing operand\n";
        }
        for(int i = 1; i <args.size(); i++){
            if(mkdir(args[i].c_str(),0777)){
                perror("mkdir");
                return;
            }
        }
    }

    void executeCd(const vector <string> & args){
        const char* target;
        if(args.size() == 1){
            target = getenv("HOME");
        }
        else{
            target = args[1].c_str();
        }
        if(chdir(target)){
            perror("cd");
        }
    }

    void executeLs(const vector <string> & args){
        const char* target;
        if(args.size() == 1){
            target = ".";
        }
        else{
            target= args[1].c_str();
        }
        DIR* dir = opendir(target);
        
        if (dir == NULL){
            perror("ls");
            return;
        }

        struct dirent* entry = readdir(dir);

        while(entry){
            cout << entry->d_name << " ";
            entry = readdir(dir);
        }

        closedir(dir);
        cout << '\n';
    }

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

            string mainCommand = tokens[0];
            // findind command in unordered map
            auto it = commands.find(mainCommand);

            if(it != commands.end()){
                //calling the mapped function of command
                it->second(tokens);
            }
            else{
                cout << mainCommand << ": command not found\n";
            }

        }
    }

};



int main(){
    UnixShell shell;
    shell.run();
}