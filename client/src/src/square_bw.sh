#!/bin/bash
ipfw -q flush
ipfw -q pipe flush
ipfw add pipe 3 ip from 192.168.164.128 to me
ipfw add pipe 3 ip from me to 192.168.164.128

ipfw pipe 3 config bw 3855Kbit/s delay 25ms
sleep 2s

ipfw pipe 3 config bw 895Kbit/s delay 25ms
sleep 10s