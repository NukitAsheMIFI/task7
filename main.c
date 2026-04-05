#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int read_and_write(FILE *file_joint, char *fname, char* member);

int main(int argc, char **argv){
	if (argc < 2){
		printf("недостаточно аргументов");
		return 1;
	}
	char *fname_joint = argv[1];
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
            
            read_and_write(file_joint, "child_copy", "Child");
			fclose(file_joint);
			
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

			read_and_write(file_joint, "parent_copy", "Parent");
			fclose(file_joint);			
		    
            printf("Parent: Exit\n");
		}
    }       
	return 0;
}

int read_and_write(FILE *file_joint, char *fname, char* member){
	FILE* file_out = fopen(fname, "w+r");
	if (file_out == NULL){
	    perror("can't open file for write");
	    return errno;
	}

	rewind(file_joint);
	char buf[1024];
	size_t bytes;
	while ((bytes = fread(buf, 1, sizeof(buf), file_joint)) > 0) {
	    fwrite(buf, 1, bytes, file_out);
	}

	rewind(file_out);
	printf("%s: ", member);
	while ((bytes = fread(buf, 1, sizeof(buf), file_out)) > 0) {
	    fwrite(buf, 1, bytes, stdout);
	}
	
    fclose(file_out);
    return 0;
}
