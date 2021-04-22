#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define MAXARG 10
#define MAXLEN 50
#define MAXCOL 45
#define MAXPATH 50

void openHelp()
{
    printf("\n\tWELCOME TO MY SHELL HELP"
        "\n\tBy Soumyajit Karmakar, 4th Sem, 1901200"
        "\n\tList of Commands supported:"
        "\n\t>exit"
        "\n\t>cd"
        "\n\t>pwd"
	    "\n\t>source"
	    "\n\t>echo"
	    "\n\t>help"
        "\n\t>all other general commands available in UNIX shell"
        "\n\t>Max 2 pipes are allowed."
        "\n\t>Improper spaces are handled.");

    return;
}


int ownCmd(char** cmd1)
{
	int N = 6, i, p = -1;
    	char* ListOfCmds[N];
	char *username;
	char path[MAXPATH];
	char *echo;
	int file;

    	ListOfCmds[0] = "exit";
    	ListOfCmds[1] = "cd";
    	ListOfCmds[2] = "pwd";
    	ListOfCmds[3] = "source";
	ListOfCmds[4] = "echo";
	ListOfCmds[5] = "help";
    	
	for (i = 0; i < N; i++) {
    	    if (strcmp(cmd1[0], ListOfCmds[i]) == 0) {
        	    p = i;
        	    break;
        	}
    	}
	
    	switch (p) {
    	case 0:
        	printf("\nGoodbye\n");
        	exit(0);
    	case 1:
        	chdir(cmd1[1]);
        	return 1;
    	case 2:
		getcwd(path, MAXPATH);
		printf("%s", path);
        	return 1;
    	case 3:
		file = open(cmd1[1], O_RDONLY);
		dup2(file, STDIN_FILENO);
        	return 1;
	case 4:
		echo = cmd1[1];
		printf("%s",echo);
		return 1;
	case 5:
		openHelp();
		return 1;
	default:
        	break;
    	}
	
    	return 0;
}

void executeBack(char **cmd1){
	if(ownCmd(cmd1))
                return;
        
        pid_t p1;
        p1 = fork();

        if(p1 == -1){
                perror("Error while forking.");
                return;
        }
        else if(p1 == 0){
                if (execvp(cmd1[0], cmd1) < 0){
                        perror("\nCould not execute command..");
                }
                exit(0);
        }
        else{
                printf("!this!");
		return;
        }
}

void execute1(char** cmd1){
        if(ownCmd(cmd1))
                return;

        pid_t p1;
        p1 = fork();

        if(p1 == -1){
                perror("Error while forking.");
                return;
        }
        else if(p1 == 0){
                if (execvp(cmd1[0], cmd1) < 0){
                        perror("\nCould not execute command..");
                }
                exit(0);
        }
        else{
                wait(NULL);
                return;
        }

}

void checkSymExecute(char **cmd){
	int i = 0;
	int a = 0,p = 0;
	char *temp;
	printf("?Some0?");
	while(cmd[i][0] == '\0'){
		if(cmd[i][0] == '&'){
			a = 1;
			printf("?Some1?");
			break;
		}
		if(cmd[i][0] == '<'){
			p = -1;
			printf("?Some2?");
			break;
		}
		if(cmd[i][0] == '>'){
			p = 1;
			printf("?Some3?");
			break;
		}
	}
	//if(a == 1){
		printf("?Some4?");
		temp = strsep(cmd, "&");
		executeBack(&temp);
		return;
	//}
	//execute1(cmd);
	return;

}


void execute2(char** cmd1, char** cmd2){
	int pipe1[2];
        pid_t p1, p2;

        if (pipe(pipe1) < 0) {
          	printf("\nPipe could not be initialized");
         	return;
    	}
    	p1 = fork();
      	if (p1 < 0) {
             	perror("\nCould not fork");
               	return;
       	}
	
       	if (p1 == 0) {
        	close(pipe1[0]);
             	dup2(pipe1[1], STDOUT_FILENO);
             	close(pipe1[1]);
		
              	if (execvp(cmd1[0], cmd1) < 0) {
                     	printf("\nCould not execute command 1..");
                	exit(0);
          	}
   	}
     	else{
       		p2 = fork();
		
             	if (p2 < 0) {
                 	printf("\nCould not fork");
                      	return;
               	}
             	if (p2 == 0) {
                  	close(pipe1[1]);
                       	dup2(pipe1[0], STDIN_FILENO);
                       	close(pipe1[0]);
                       	if (execvp(cmd2[0], cmd2) < 0) {
                           	printf("\nCould not execute command 2..");
                               	exit(0);
                     	}
              	}
               	else {
                  	wait(NULL);
                      	wait(NULL);
              	}
  	}
}

void execute3(char** cmd1, char** cmd2, char** cmd3){
	int pipe1[2];
       	int pipe2[2];
	
     	if(pipe(pipe1) < 0 || pipe(pipe2) < 0){
      		perror("\nPipes could not be initialized");
              	return;
     	}
	
    	pid_t p1, p2, p3;
	
        p1 = fork();
	
     	if (p1 < 0) {
           	printf("\nCould not fork");
             	return;
      	}
      	if (p1 == 0) {
             	close(pipe1[0]);
               	close(pipe2[0]);
           	close(pipe2[1]);
            	dup2(pipe1[1], STDOUT_FILENO);
               	close(pipe1[1]);
		
             	if (execvp(cmd1[0], cmd1) < 0) {
                    	perror("\nCould not execute command 1..");
                     	exit(0);
              	}
      	}
      	else {
		
          	p2 = fork();
		
             	if (p2 < 0) {
                   	printf("\nCould not fork");
                      	return;
            	}
		
             	if (p2 == 0) {
                   	close(pipe1[1]);
                      	close(pipe2[0]);
                      	dup2(pipe1[0], STDIN_FILENO);
                     	dup2(pipe2[1], STDOUT_FILENO);
                      	close(pipe1[0]);
                       	close(pipe2[1]);
                      	if (execvp(cmd2[0], cmd2) < 0) {
                             	printf("\nCould not execute command 2..");
                               	exit(0);
                       	}
             	}
		else{
                    	p3 = fork();
			
                      	if(p3 < 0){
                          	printf("\nCould not fork");
                             	return;
                       	}
			
                      	if(p3 == 0){
                         	close(pipe1[1]);
                             	close(pipe1[0]);
                            	close(pipe2[1]);
                               	dup2(pipe2[0], STDIN_FILENO);
                             	close(pipe2[0]);
                               	if(execvp(cmd3[0], cmd3) < 0){
                               		printf("\nCould not execute command 3..");
                               		exit(0);
                           	}
                    	}
                     	else{
                          	wait(NULL);
                              	wait(NULL);
                             	wait(NULL);
                    	}
              	}
   	}
}



void initialise(){
	printf("This is the starting. Enter help to get help.");
}


char *readLine(void){
  	char *line = NULL;
  	ssize_t bufsize = 0;

	int i = getline(&line, &bufsize, stdin);

	if (i == -1){
    		if (feof(stdin)) {
      			exit(EXIT_SUCCESS);
    		}
	       	else {
      			perror("Readline Error.");
      			exit(EXIT_FAILURE);
    		}
  	}
	
	line[i - 1] = '\0';
	
  	return line;
}


int parseCmd(char *str, char **cmd){
        int i;

        for (i = 0; i < MAXARG; i++) {
                cmd[i] = strsep(&str, " ");

                if (cmd[i] == NULL)
                        break;
                if (strlen(cmd[i]) == 0)
                        i--;
        }
}

int pipeParser(char **line, char **cmd1, char **cmd2, char **cmd3){
        char* pipedArg[3] = {NULL};
        int pipeNo = 0, i;

        for(i = 0; i < 3; i++){
		pipedArg[i] = strsep(line, "|");
                if (pipedArg[i] == NULL){
                        pipeNo = i;
			break;
		}
	}
	
        parseCmd(pipedArg[0], cmd1);
       	parseCmd(pipedArg[1], cmd2);
       	parseCmd(pipedArg[2], cmd3);

        return pipeNo;
}

int sColParser(char *line, char **cmd1Raw, char **cmd2Raw){
	*cmd1Raw = strsep(&line, ";");
	*cmd2Raw = strsep(&line, ";");

	if(*cmd2Raw == NULL)
		return 1;
	
	return 2;

}



void controller(void)
{
        char *line;
	char *cmd1Raw[MAXCOL] = {NULL};
	char *cmd2Raw[MAXCOL] = {NULL};

        char *cmd1[MAXLEN] = {NULL};
        char *cmd2[MAXLEN] = {NULL};
        char *cmd3[MAXLEN] = {NULL};
        int status;

        int m, n;

        initialise();

        while(1){
                printf("\n>>> ");
                line = readLine();
        		        
		m = sColParser(line, cmd1Raw, cmd2Raw);
		
                n = pipeParser(cmd1Raw, cmd1, cmd2, cmd3);
		
		if(n == 1)
                	//execute1(cmd1);
			checkSymExecute(cmd1);
		if(n == 2)
			execute2(cmd1, cmd2);
		if(n == 3)
			execute3(cmd1, cmd2, cmd3);
		
		if(m == 2){
			n = pipeParser(cmd2Raw, cmd1, cmd2, cmd3);
                        
			if(n == 1)
                        	execute1(cmd1);
                	if(n == 2)
                	        execute2(cmd1, cmd2);
                	if(n == 3)
                        	execute3(cmd1, cmd2, cmd3);
		}
		
        }
}

int main(){
        controller();
        return 0;
}

