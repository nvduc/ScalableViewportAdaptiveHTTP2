#!/bin/bash
ipfw -q flush
ipfw -q pipe flush
ipfw add pipe 3 ip from 172.16.232.129 to me
ipfw add pipe 3 ip from me to 172.16.232.129

ipfw pipe 3 config bw 3500Kbit/s delay 25ms
sleep 100s

ipfw pipe 3 config bw 1500Kbit/s delay 25ms
sleep 3s

ipfw pipe 3 config bw 3500Kbit/s delay 25ms
sleep 3s

ipfw pipe 3 config bw 1500Kbit/s delay 25ms
sleep 30s

ipfw pipe 3 config bw 2600Kbit/s delay 25ms
sleep 30s

ipfw pipe 3 config bw 3600Kbit/s delay 25ms
sleep 30s

ipfw pipe 3 config bw 1900Kbit/s delay 25ms
sleep 30s

ipfw pipe 3 config bw 1000Kbit/s delay 25ms


