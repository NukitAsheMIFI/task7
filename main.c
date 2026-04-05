#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv){
	if (argc < 2){
		printf("недостаточно аргументов");
	}
	char *fname_joint = argv[1];
	char *fname_parent = "parent_copy";
	char *fname_child = "child_copy";
	FILE* file_joint = fopen(fname_joint, "r"); //argv[1]
	if (file_joint == NULL){
		perror("can't open file for read");
		return errno;
	}
	pid_t pid;
	int status;
	switch(pid = fork()){
		case -1:
			perror("fork error");
			return errno;
		case 0: {
			printf("Child: My PID -- %d\n", getpid());
            printf("Child: My parent's PID -- %d\n", getppid());
            printf("Child: Enter my return code\n");
            scanf("%d", &status);

            printf("Child: do job\n");

            
            FILE* file_child = fopen(fname_child, "w+r");
            if (file_child == NULL){
                perror("can't open child file");
                fclose(file_joint);
                return errno;
            }

			fseek(file_joint, 0, SEEK_END); 
			size_t len = ftell(file_joint); 
			rewind(file_joint); 
			char *text = malloc(len + 1);
			size_t bytes_read = fread(text, 1, len, file_joint); 
			if (bytes_read < len) {;
				perror("fread error");
				fclose(file_child);
				fclose(file_joint);
				free(text);
				return errno;
			}

			size_t bytes_write = fwrite(text, 1, len, file_child); 
			if (bytes_write < len) {;
				perror("fwrite error");
				fclose(file_child);
				fclose(file_joint);
				free(text);
				return errno;
			}			

			fclose(file_joint);
			fclose(file_child);
			
            printf("Child: Exit\n");
            exit(status);
		}
        default: {
        	printf("Parent: My PID -- %d\n", getpid());
        	printf("Parent: My child's PID -- %d\n", pid);
        	printf("Parent: Waiting for my child to call exit()...\n");    
            wait(&status);
            
            if (WIFEXITED(status)) {
                printf("Parent: Child's exit status: %d\n", WEXITSTATUS(status));
            } else {
                printf("Parent: Child did not exit normally\n");
            }

			printf("Parent: do job\n");

			
			FILE* file_parent = fopen(fname_parent, "w+r");
			if (file_parent == NULL){
				perror("can't open parent file");
				fclose(file_joint);
				return errno;
			}
			
			fseek(file_joint, 0, SEEK_END); 
			size_t len = ftell(file_joint); 
			rewind(file_joint); 
			char *text = malloc(len + 1);
			size_t bytes_read = fread(text, 1, len, file_joint); 
			if (bytes_read < len) {;
				perror("fread error");
				fclose(file_parent);
				fclose(file_joint);
				free(text);
				return errno;
			}

			size_t bytes_write = fwrite(text, 1, len, file_parent); 
			if (bytes_write < len) {;
				perror("fwrite error");
				fclose(file_parent);
				fclose(file_joint);
				free(text);
				return errno;
			}
			

			fclose(file_joint);			
		    fclose(file_parent); 
		    
            printf("Parent: Exit\n");
		}
    }       
	return 0;
}
