#include "types.h"
#include "fcntl.h"
#include "user.h"

char* usage = "Usage : filemanager <command> [command opts]\n";

int createDir(char* path);
int createFile(char* filename);
void secondHeuristicTest(char* file, char* folder);


int 
createFile(char* filename){
	printf(1, "Creating File %s\n", filename);
	int fd = open(filename, O_CREATE);
	if(fd > 0){
		printf(1, "File Creation Successful!\n");
		close(fd);
	}
	else {
		printf(2, "There was an error creating the file!\n");
		exit();
	}

	return fd;
}

int 
createDir(char* path){
	int val;

	printf(1, "Creating Folder %s\n", path);
	if( (val = mkdir(path))){
		printf(2, "There was an error creating the directory\n");
		exit();
	}
	else printf(1, "Dir created successfully!\n");

	return val;
}



void 
secondHeuristicTest(char* file, char* folder){

	//Step 1  : Create a dir
	createDir(folder);

	//Step 2  : Cd into new dir
	if(chdir(folder)){
		printf(2, "Error Changing to dir : %s\n", folder);
		exit();
	}
	else printf(1, "Successfully Changed to dir : %s\n", folder);

	//Step 3  : Create a file
	createFile(file);

	//Step 4  : Verify file in same block group
	printStats();
}

int
main(int argc, char *argv[])
{
	if(argc == 1){
		printf(2, usage);
		exit();
	}

	char* cmd = argv[1];

	if(0 == strcmp(cmd, "create_file")){
		createFile(argv[2]);
	}
	else if(0 == strcmp(cmd, "create_dir")){
		createDir(argv[2]);
	}
	else if (0 == strcmp(cmd, "tests")){
		char *file = "H2F1", 
			 *folder="test2"; 

		if(argc > 2){
			file = argv[2];
		}
		if(argc > 3){
			folder = argv[3];
		}

		secondHeuristicTest(file, folder);
	}


	exit();
	return 0;
}