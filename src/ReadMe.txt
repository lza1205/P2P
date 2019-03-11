/*--------------------------------------------------------------------
　　　/\~~~~~~~~~~~~~\　　　▓　　^*^　　　☆　　$$　 .☆ 
　　./　\~~~▓~　 ~~~~\ ◆　　圣诞 .快乐　 *　 $◢◣$　 * 
　　/ ^^ \ ══════\.◆　　　 *　*　　*　 $◢★◣$　 * 
　..▎[]　▎田　田 ▎ |┃◆　 .　　　　　*　 $◢■■◣$　 * 
　&&▎　　▎　　　 ▎'|'▎ @　　　　　　　* $◢■■■◣$ * 
＃ ■■■■■■■■■■〓▄▃▂▁愿你圣诞快乐︸︸||︸︸ 


				编译 ucp2p 库
版本：1.0
时间：2015.08.01
作者：Faker

--------------------------------------------------------------------*/



1.	需要注意的是，ucp2p 库依赖于其他库，请确认其它库已经安装好
	客户端 依赖的库有： pthread openssl json
			-lpthread -lssl -lcrypto -lm -ljson
	服务器依赖的库有：	pthread openssl json sqlclient
			-lpthread -lssl -lcrypto -lm -ljson -lmysqlclient
	
	如果没有安装好对应的lib库，请执行：
	apt-get install openssl-dev
	apt-get install libjson0

	安装mysql 数据库。具体安装参考百度
	1. sudo apt-get install mysql-server
　　2. apt-get install mysql-client
　　3. sudo apt-get install libmysqlclient-dev

2.	确认所有库都安装好后，执行 make 即可

3.	如果没有意外的话，编译通过后会在当前目录下生成下面几个文件

	p2p_client ：
		P2P的简单客户端
	p2p_server ：
		P2P的简单服务器程序
	
	libp2p-client.so ：
		P2P 客户端依赖的动态库
	
	libp2p-server.so ：
		P2P 服务器端依赖的动态库

4	把 libp2p-client.so libp2p-server.so 拷贝到 /usr/lib/ 下面

