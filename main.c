#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/*handler function for child termination
to write in file if child terminate*/
void childSignal(int childSig){
    FILE *childFile;
    //open file childs.txt or create it if doesn't exits & make mode append
    childFile = fopen("childs.txt","a");
    if(childFile == NULL){
        printf("Error in file\n");
    }else{
        fprintf(childFile,"Child process terminated\n");
    }
    fclose(childFile);
}
int main()
{
    //command input that user will write
    char input[50];
    //flag to check &
    int flag = 0;
    while(1){
        flag = 0;
        printf("Enter the command :\n");
        //get the command from user
        gets(input);
        //check if the input is exit to exit program
        if(strcasecmp(input,"exit") == 0){
            exit(0);
        }
        //check empty input
        if(strcmp(input,"")==0){
            continue;
        }
        //sperate input into arguments
        char *token = strtok(input," ");
        //array of tokenized arguments
        char *args[8];
        //counter for numbers of arguments
        int count = 0;
        //loop along token not equal null
        while(token){
            args[count] = token;
            token = strtok(NULL," ");
            count ++;
        }
        //check for & to replace it with null
        if(strcmp(args[count-1],"&") == 0){
            args[count-1] = NULL;
            //& exits set flag=1
            flag = 1;
        }else{
            //add last element in array = null to be send in execvp correctly
            args[count] = NULL;
            count++;
        }
        //handle if user enter "exit  "
        if(strcasecmp(args[0],"exit") == 0){
            exit(0);
        }
        //check if commmand is cd
        if(strcasecmp(args[0],"cd") == 0){
            //call function chdir by passing the path to change the directory
            //check if the function returns -1 (error in path)
            if(chdir(args[1]) == -1){
                printf("cd Error in command\n");
            }
        }else{
            //signal handle function that invokes when child terminates
            signal(SIGCHLD,childSignal);
            //create child process
            pid_t pid = fork();
            if(pid == 0){
                //child
                /*execute command by passing first element in arguments and other arguments
                check if execvp return negative -> invalid command*/
                if(execvp(args[0],args) < 0){
                    printf("Erorr Invalid Command\n");
                    exit(0);
                }
            }else if(pid > 0){
                //parent
                //check if & exists
                if(flag == 0){
                    waitpid(pid,NULL,0);
                }
            }else{
                //failure in fork
                printf("error in calling function fork()\n");
                exit(0);
            }
        }
    }
    return 0;
}
