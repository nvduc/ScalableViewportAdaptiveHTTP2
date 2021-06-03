#!/bin/bash

# tc-based script for controlling out going traffic

DEV=eth2 # NIC's name
src_ip=192.168.9.90 # server's ip address

#
tc qdisc del dev $DEV root
#
tc qdisc add dev $DEV root handle 1: htb default 20
#
tc class add dev $DEV parent 1: classid 1:1 htb rate 2mbit
tc class add dev $DEV parent 1:1 classid 1:10 htb rate 1mbit ceil 2mbit
tc class add dev $DEV parent 1:1 classid 1:20 htb rate 1mbit ceil 2mbit

tc filter add dev $DEV protocol ip parent 1:0 prio 1 u32 match ip src "$src_ip" flowid 1:10

sleep 1

# next throughput

sleep 1

# next throughput
