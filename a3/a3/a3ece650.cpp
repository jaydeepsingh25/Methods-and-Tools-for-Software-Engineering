#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <vector>

using namespace std;


int a3_input(void) {
    while (!std::cin.eof()) {
        string inputLine;
        getline(cin, inputLine);
        if (inputLine.size () > 0){
            cout << inputLine << endl;
        }
    }
    return 0;
}

void closeAllpipes(int *rgen_a1, int *a1_a2)
{
    close(rgen_a1[0]);
    close(rgen_a1[1]);
    close(a1_a2[0]);
    close(a1_a2[1]);
}
int main(int argc, char **argv)
{
    // std::cout << "Replace with your solution for the main "
    //           << "driver program of Assignment 3\n";
    // this program will set up IPC of all the programs
    char *a1_args[] = {(char *)"python3", (char *)"a1ece650.py", (char *)NULL};
    char *a2_args[] = {(char *)"./a2ece650", NULL};

    int rgen_a1[2];
    int a1_a2[2];

    pipe(rgen_a1);
    pipe(a1_a2);

    vector<pid_t> children;
    pid_t child_pid;
    // First child -- Rgen
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(rgen_a1[1], STDOUT_FILENO);
        closeAllpipes(rgen_a1, a1_a2);
        return execv("./rgen", argv);
    }
    children.push_back(child_pid);

    // Second Child -- A1
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(rgen_a1[0], STDIN_FILENO);
        dup2(a1_a2[1], STDOUT_FILENO);
        closeAllpipes(rgen_a1, a1_a2);
        return execvp(*a1_args, a1_args);
    }
    children.push_back(child_pid);
    child_pid = fork();
    // Third Child -- A2
    if (child_pid == 0)
    {
        dup2(a1_a2[0], STDIN_FILENO);
        closeAllpipes(rgen_a1, a1_a2);
        return execvp(*a2_args, a2_args);
    }
    children.push_back(child_pid);

    dup2(a1_a2[1], STDOUT_FILENO);
    closeAllpipes(rgen_a1, a1_a2);
    int res = a3_input();
    for (pid_t k : children)
    {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }
    return res;
}
