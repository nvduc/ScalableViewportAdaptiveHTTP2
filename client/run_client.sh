#!/bin/bash

SERVER_ADDR=192.168.0.121
# run client
./src/src/nghttp -n -w30 -rlog.har -p30 http://${SERVER_ADDR}:3002/face0/bitrate=3000/num=1
