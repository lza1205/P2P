#!/bin/bash

cp include/agent_proto.h	client_qdy/agent_proto.h
cp include/config.h			client_qdy/config.h

cd client_qdy
make clean
make
cd ..

