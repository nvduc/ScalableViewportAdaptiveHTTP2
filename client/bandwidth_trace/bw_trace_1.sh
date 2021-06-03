#!/bin/bash

ipfw -q flush
ipfw -q pipe flush

ipfw add pipe 1 ip from any to any

ipfw pipe 1 config bw 12000kbit/s delay 5ms

sleep 14s

ipfw pipe 1 config bw 1000kbit/s delay 5ms

sleep 5s

ipfw pipe 1 config bw 5000kbit/s delay 5ms

sleep 20s

ipfw pipe 1 config bw 12000kbit/s delay 5ms
