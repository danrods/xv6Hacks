#include "types.h"
#include "fcntl.h"
#include "user.h"

const char* usage = "Usage : filemanager <command> [command opts]\n";

int
main(int argc, char *argv[])
{
	if(argc == 1){
		printf(2, usage);
		exit();
	}

	char* cmd = argv[1];

	if(0 == strcmp(cmd, "create_file")){
		char* filename = argv[2];
		printf(1, "Creating File %s\n", filename);
		int fd = open(filename, O_CREATE);
		if(fd > 0){
			printf(1, "File Creation Successful!\n");
			close(fd);
		}
		else printf(2, "There was an error creating the file!\n");
	}
	else if(0 == strcmp(cmd, "create_dir")){
		char* path = argv[2];
		printf(1, "Creating Folder %s\n", path);
		if(mkdir(path)){
			printf(2, "There was an error creating the directory\n");
		}
		else printf(1, "Dir created successfully!\n");
	}
	else{
		printf(2, "Command is required! --> Found %s\n", cmd);
		printf(2, usage);
	}


	exit();
	return 0;
}