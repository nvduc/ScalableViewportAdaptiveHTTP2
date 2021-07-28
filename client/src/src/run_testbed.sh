#!/bin/bash
#
SERVER_ADDR=127.0.0.1

# run client
./nghttp -snv -w30 -rlog.har  -p30 http://${SERVER_ADDR}:3002/face0/bitrate=3000/num=1
