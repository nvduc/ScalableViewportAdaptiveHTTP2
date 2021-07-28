#!/bin/bash
# must run as 'root'
tc qdisc delete dev lo root

tc qdisc add dev lo root handle 1: tbf rate 5000kbit buffer 500000 limit 30000
tc qdisc add dev lo parent 1: handle 10: netem delay 25ms

tc qdisc show

