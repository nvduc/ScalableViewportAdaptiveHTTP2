#!/bin/bash

# build server
cd src
g++ -o server_6f_2x2 server_6f_2x2_v2.cpp -lnghttp2_asio -lboost_system -std=c++11 -lssl -lcrypto -lpthread -lev -g
cp server_6f_2x2 ../bin/
