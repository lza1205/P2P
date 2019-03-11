#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <linux/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>


int main(void)
{
	int ret;
	
	while(1){
		
		printf("cp ./agent_client_new ./agent_client_st \n\n");
		ret = system("cp ./agent_client_new ./agent_client_st");

		ret = system("ls ./");
		
		printf("start ./agent_client_st\n\n");
		ret = system("./agent_client_st");

		printf("ret : %d\n", ret);

		
		if((-1 == ret) || (WEXITSTATUS(ret) != 0xa)){		//´íÎó
			printf("start ./agent_client\n\n");
			ret = system("./agent_client");

			if((-1 == ret) || (WEXITSTATUS(ret) != 0xa))
				return -1;
		}
	}
}


