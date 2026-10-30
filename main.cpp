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
        };
        commands["pwd"] = [this](const vector <string>& args){
            executePwd(args);
        };
        commands["organize"] = [this](const vector <string>& args){
            executeOrganize(args);
        };
        commands["dirsize"] = [this](const vector <string>& args){
            executeDirsize(args);
        };
        commands["touch"] = [this](const vector <string>& args){
            executeTouch(args);
        };
        commands["rm"] = [this](const vector <string>& args){
            executeRm(args);
        };
        commands["rmdir"] = [this](const vector <string>& args){
            executeRmdir(args);
        };
        commands["mv"] = [this](const vector <string>& args){
            executeMv(args);
        };
        commands["cat"] = [this](const vector <string>& args){
            executeCat(args);
        };
        commands["safe-rm"] = [this](const vector <string>& args){
            executeSafeRm(args);
        };
        commands["hide"] = [this](const vector <string>& args){
            executeHide(args);
        };
        commands["Unhide"] = [this](const vector <string>& args){
            executeUnhide(args);
        };
        commands["prune"] = [this](const vector <string>& args){
            executePrune(args);
        };
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
    
    void executePwd(const vector<string> &args){
        const int size = 1024;
        char buffer[size];
        char *s = getcwd(buffer, size);
        if(s== nullptr){
            perror("pwd");
        }
        else cout << s << '\n';
    }

    void executeOrganize(const vector <string> &args){
        DIR* dir = opendir(".");
        if(dir == NULL){
            perror("Organize");
            return;
        }
        struct dirent *entry;
        while((entry = readdir(dir)) != nullptr){
            if(entry->d_type != DT_REG){
                continue;
            }

            string fileName = entry->d_name;

            size_t pos = fileName.rfind('.');

            if(pos == string::npos || pos == 0){
                continue;
            }

            string ext = fileName.substr(pos+1);
            mkdir(ext.c_str(), 0777);

            string newPath = ext + "/" + fileName;
            if(rename(fileName.c_str(),newPath.c_str())){// this if runs if it doesnt return 0
                perror("Organize Rename error");
            }
            else{
                cout << "Moved " << fileName << " -> " << newPath << '\n';
            }

        }
        
        closedir(dir);
    }

    long long calcDirSize(const string & path){
        long long totalSize = 0;
        //open directory
        DIR *dir = opendir(path.c_str());
        if(dir == nullptr){
            perror("dirsize opendir error");
            return 0;
        }

        struct dirent * entry;

        while((entry = readdir(dir)) != nullptr){
            string name = entry->d_name;

            if(name == "." || name == ".."){
                continue ;//skip current and parent directory
            }
            string fullPath = path + "/" + name;

            struct stat fileInfo;

            if(stat(fullPath.c_str(), &fileInfo)){
                perror("dirsize stat error");
                continue;
            }
            if(S_ISREG(fileInfo.st_mode)){// check if it is regular file
                totalSize += fileInfo.st_size;  // add file size
            }
            else if(S_ISDIR(fileInfo.st_mode)){
                totalSize+= calcDirSize(fullPath);//recursively find size of subdir
            }
        }
        closedir(dir);
        return totalSize;
    } 

    void executeDirsize(const vector <string> & args){
        string path = ".";
        if(args.size() > 1){
            path = args[1];
        }

        long long bytes = calcDirSize(path);
        cout << "Total Size : " << bytes << " bytes\n";
    }

    
    void executeTouch(const vector <string> & args){
        if (args.size() < 2) {
            cout << "touch: missing file operand\n";
            return; 
        }

        string filename = args[1];
        int fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0644);

        if (fd == -1) {
            perror("touch error");
            return;
        }

        close(fd);
    }
    
    void executeRm(const vector <string> & args){
        if (args.size() < 2) {
            cerr << "error: filename not provided" << endl;
            return;
        }

        string filename = args[1];

        if (unlink(filename.c_str()) == -1) {
            perror("rm error");
        }
    }
    
    void executeRmdir(const vector <string> & args){
        if (args.size() < 2) {
            cerr << "error: directory name not provided\n";
            return; 
        }

        string dirname = args[1];
        
        if (rmdir(dirname.c_str()) == -1) {
            perror("rmdir error");
        }
    }
    
    void executeMv(const vector <string> & args){
        
    }
    
    void executeSafeRm(const vector <string> & args){
        
    }
    
    void executeCat(const vector <string> & args){
        
    }
    
    void executeHide(const vector <string> & args){

    }
    
    void executeUnhide(const vector <string> & args){
        
    }
    
    void executePrune(const vector <string> & args){
        
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