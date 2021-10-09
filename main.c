#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "unistd.h"
#include "string.h"

void read_filename(string* str, int fd){
	char cur = 'a';
	while(read(fd, &cur, sizeof(char)) > 0){
		if(cur == '\n'){
			break;
		}
		str_push(str, cur);
	}
}

int main(){
	string file_name;
	str_create(&file_name);
	read_filename(&file_name, 0); // 0 = STDIN
	str_push(&file_name, '\0');
	int file = open(str_get_all(&file_name), O_RDONLY);
	if(file == -1){
		perror("File can't be opened");
		str_destroy(&file_name);
		return -5;
	}
	str_destroy(&file_name);
	int fd[2];
	if(pipe(fd) < 0){
		perror("pipe error");
		return -2;
	}
	int pid = fork();
	if(pid == -1){
		perror("fork error");
		return -1;
	}
	if(pid == 0){ //child
		close(fd[0]);
		if(dup2(file, 0) < 0){
			perror("Cannot dup fd[0] to stdin");
			return -5;
		}
		if(dup2(fd[1], 1) < 0){
			perror("Cannot dup fd[1] to stdout");
			return -6;
		}
		if(execl("child", "", NULL) == -1){
			perror("Execl child problem");
			return -7;
		}
	} else{ //parent
		int status;
		wait(&status);
		if(WEXITSTATUS(status)){
			return -1;
		}
		float res = 0.0f;
		read(fd[0], &res, sizeof(float));
		printf("%f\n", res);
		//write(1, &res, sizeof(float));
		close(fd[0]);
		close(fd[1]);
	}
	return 0;
}