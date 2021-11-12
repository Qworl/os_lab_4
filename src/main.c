#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
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
    char template[] = "/tmp/tmpXXXXXX";
    int desc = mkstemp(template);
    if(desc < 0){
    	perror("Tmp file not created\n");
		return -6;
    }
	ftruncate(desc, sizeof(float));
    printf("%s\n", template);

    string file_name;
	str_create(&file_name);
	read_filename(&file_name, 0); // 0 = STDIN
	str_push(&file_name, '\0');

	int file = open(str_get_all(&file_name), O_RDONLY);
	if(file < 0){
		perror("File can't be opened");
		str_destroy(&file_name);
		return -5;
	}

	int pid = fork();
	if(pid == -1){
		perror("fork error");
		return -1;
	}
	if(pid == 0){ //child
		if(dup2(file, 0) < 0){
			perror("Cannot dup fd[0] to stdin");
			return -5;
		}
		if(execl("child", "child", template, NULL) == -1){
			perror("Execl child problem");
			return -7;
		}
	} else{ //parent
		int status;
		wait(&status);
		if(WEXITSTATUS(status)){
			return -1;
		}
		float* fd = mmap(0, sizeof(float), 
    				PROT_READ | PROT_WRITE,
    				MAP_SHARED, desc, 0);
	    if (fd == MAP_FAILED){
	    	perror("mmap error\n");
			return -5;
	    }
		float res = fd[0];
		printf("%f\n", res);
		munmap(fd, sizeof(float));
		close(desc);
	}
	return 0;
}
