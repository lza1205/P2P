
#include "config.h"
#include "agent_proto.h"

void qdy_resolve_recv_data(char *buf, int len)
{
	char *recv_data = "test";
	int recv_len;

	dump_data(buf, len);

	struct check_head *head = (struct check_head *)buf;
	struct proto_c_send_data *recv_proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	recv_data = (char *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));

	printf("recv from %s 's %s data : [%s] \r\n", recv_proto->src_name, 
										((head->affairs == _aff_client_p2p_data_)?"stun":"turn"),
										recv_data);	

	//·¢ËÍÊý¾Ý
	char *msg = "I fine!";
	qdy_send_data(recv_proto->src_name, msg, strlen(msg) + 1);
}


