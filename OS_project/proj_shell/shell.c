#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int i = 0; //count 용 변수
    int pid[50]; //pid 저장
    int argNum =0; //;로 나뉜 토큰 갯수+2(명령어 갯수)
    char readArgs[100]; //문자열 받는 배열
    char* command[50]; //;로 나눈 token을 저장하는 배열
    char* commandToken[20][20];  
    //인자가 전달되지 않고 실행되는 case
    if(argc == 1){
       while(1)
       {
           argNum = 0;
            printf("prompt> ");
            if(fgets(readArgs, 100, stdin) == NULL){printf("\n"); return 0;} //if ctrl+d input
            readArgs[strlen(readArgs)-1] = '\0';
            if(strcmp(readArgs,"quit") == 0){
                return 0;
            }
            //command[0]~command[argNum-2] : token
            //command[argNum-1] : NULL
            command[argNum++] = strtok(readArgs, ";");
            while(command[argNum++] = strtok(NULL, ";")){ }

            for(i = 0; i<argNum-1; i++){
                pid[i] = fork(); //create child process
                if(pid[i] == 0){ //if child process
                    int tokenNum=0;
                    //commandToken[i][0]~commandToken[i][tokenNum-2] : token
                    //commandToken[i][tokenNum-1] : NULL
                    commandToken[i][tokenNum++] = strtok(command[i], " ");
                    while(commandToken[i][tokenNum++] = strtok(NULL, " ")){ }
                    execvp(commandToken[i][0], commandToken[i]); //실행할 파일 호출
                    printf("this shouldn't print out\n"); 
                }
                else if(pid < 0){ //error when make child process
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                }
            }
            while(wait(NULL)>0){ }
       }
    }
    
    else if(argc ==2){
        FILE *fp = fopen(argv[1], "r");
        if(fp==NULL)
        {
            printf("there is no batch file\n");
            exit(1);
        }
        while(fgets(readArgs, 100, fp)){
           argNum = 0;
           //fgets는 개행문자도 포함시키므로
           if(readArgs[strlen(readArgs)-1] == '\n'){
                readArgs[strlen(readArgs)-1] = '\0';
           }
            if(strcmp(readArgs,"quit") == 0){
                return 0;
            }
            printf("%s\n",readArgs);
            //command[0]~command[argNum-2] : token
            //command[argNum-1] : NULL
            command[argNum++] = strtok(readArgs, ";");
            while(command[argNum++] = strtok(NULL, ";")){ }

            for(i = 0; i<argNum-1; i++){
                pid[i] = fork(); //create child process
                if(pid[i] == 0){ //if child process
                    int tokenNum=0;
                    //commandToken[i][0]~commandToken[i][tokenNum-2] : token
                    //commandToken[i][tokenNum-1] : NULL
                    commandToken[i][tokenNum++] = strtok(command[i], " ");
                    while(commandToken[i][tokenNum++] = strtok(NULL, " ")){ }
                    execvp(commandToken[i][0], commandToken[i]); //실행할 파일 호출
                    printf("this shouldn't print out\n"); 
                }
                else if(pid < 0){ //error when make child process
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                }
            }
            while(wait(NULL)>0){ }
        }
    }

    
    else{
        fprintf(stderr, "write only one parameter\n");
        exit(1);
    }

    return 0;
}
