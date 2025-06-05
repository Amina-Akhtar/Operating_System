#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) 
{
    if (argc < 4) 
    {
        cout << "Command line arguments are missing\n";
        return 0;
    }
    const char *inputFile = argv[1];
    const char *outputFile = argv[2];
    const char *errorFile = argv[3];
    int fd0 = open(inputFile, O_RDONLY);
    if (fd0 == -1) 
    {
        cout << "Error in opening input file\n";
        return 1;
    }
    int fd1 = open(outputFile, O_WRONLY | O_CREAT, 0666);
    if (fd1 == -1) 
    {
        cout << "Error in opening output file\n";
        close(fd0);
        return 1;
    }

    int fd2 = open(errorFile, O_WRONLY | O_CREAT, 0666);
    if (fd2 == -1) 
    {
        cout << "Error in opening error file\n";
        close(fd0);
        close(fd1);
        return 1;
    }
    cout<<"File descriptors are redirected\n";
    int outfd = dup(STDOUT_FILENO);
    int errorfd = dup(STDERR_FILENO);
    dup2(fd1, STDOUT_FILENO);
    dup2(fd2, STDERR_FILENO);
    cout << "Processing input file...\n";
    char BUFF[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd0, BUFF, sizeof(BUFF))) > 0) 
    {
       BUFF[bytesRead] = '\0'; 
       char *token = strtok(BUFF, " \n");
       while (token != NULL) 
       {
        if (isdigit(*token) )
        { 
            cout << "Read number: " << token << "\n"; 
            int num = atoi(token);
            if (num == 0) 
            {
                cout << "Error: Division by zero encountered\n";
            } 
            else 
            {
                num = 100 / num;
                cout << "Processed number: " << num << "\n";
            }
        } 
        else 
        {
            cerr << "Error: Invalid number format for '" << token << "'\n";
             cerr << "Error: Division by zero encountered\n";
        }
        token = strtok(NULL, " \n"); 
    }
}
fflush(stdout);
fflush(stderr);

dup2(outfd, STDOUT_FILENO);
dup2(errorfd, STDERR_FILENO);
close(outfd);
close(errorfd);
cout << "File descriptors are restored\n";
}
