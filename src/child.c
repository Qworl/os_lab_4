#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "unistd.h"

typedef enum{
	reading_suc,
	reading_eol,
	reading_wrong_value,
	reading_eof,
} read_rvl_stat;

typedef enum{
	reading_before,
	reading_after,
} read_status;

read_rvl_stat reading_float(int fd, float* cur){
	bool dot_fnd = false;
	char c;
	*cur = 0;
	double i = 0.1;
	int k = read(fd, &c, sizeof(char));
	while(k > 0){
		if(c == '\n') return reading_eol;
		if(c == ' ') break;
		if(((c < '0') || (c > '9')) && c != '.'){
			return reading_wrong_value;
		}
		if(!dot_fnd){
			if(c == '.') dot_fnd = true;
			else{
				*cur = *cur * 10 + c - '0';
			}
		} else{
			if(c == '.'){
				return reading_wrong_value;
			}
			*cur = *cur + i * (c - '0');
			i /= 10;
		}
		k = read(fd, &c, sizeof(char));
	}
	if(k == 0) return reading_eof;
	return reading_suc;
}

int main(int argc, char* argv[]){
	if(argc != 2){
		perror("Error in execl\n");
		return -1;
	}
	float sum = 0.0f, cur = 0.0f;
	read_rvl_stat status = reading_float(0, &cur);
	while(status != reading_wrong_value){
		sum += cur;
		if(status == reading_eof){
			perror("Test should end with <endline>\n");
			return -1;
		} else if(status == reading_eol){
			break;
		}
		cur = 0.0f;
		status = reading_float(0, &cur);
	}
	if (status == reading_wrong_value){
		perror("Wrong value in test_file\n");
		return -2;
	}

	int desc = open(argv[1], O_RDWR);
	if(desc < 0){
    	perror("Tmp file not created\n");
		return -6;
	}
	float* fd = mmap(0, sizeof(float), 
    				PROT_WRITE,
    				MAP_SHARED,	desc, 0);
    if (fd == MAP_FAILED){
    	perror("mmap error\n");
		return -5;
    }
    fd[0] = sum;
    if(msync(fd, sizeof(float), MS_SYNC) < 0){
		perror("Msync problem");
		return -6;
	}
    if(munmap(fd, sizeof(float)) < 0){
		perror("Munmap problem");
		return -7;
	}
    close(desc);
    return 0;
}
