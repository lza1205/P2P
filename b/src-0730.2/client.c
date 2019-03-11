
#define __CLIENT_C_

#include "all.h"

#include "client.h"

int main(int argc, char *argv[])
{
	char command[1024];
	char *str;

	p2p_client_init();

	while(1)
	{
        str = fgets(command, 1024, stdin);
		if(str != NULL)
		{
	        command_format(command);
			printf("client @: $ ");
		}

	}
	return 0;
}


