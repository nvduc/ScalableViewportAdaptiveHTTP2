#!/bin/bash
ipfw -q flush
ipfw -q pipe flush
ipfw add pipe 3 ip from 192.168.216.129 to me
ipfw add pipe 3 ip from me to 192.168.216.129

ipfw pipe 3 config bw 3000Kbit/s delay 25ms
sleep 30s

ipfw pipe 3 config bw 1200Kbit/s delay 25ms
sleep 20s

ipfw pipe 3 config bw 500Kbit/s delay 25ms
sleep 50s

ipfw pipe 3 config bw 2500Kbit/s delay 25ms
sleep 50s

ipfw pipe 3 config bw 500Kbit/s delay 25ms
sleep 50s

ipfw pipe 3 config bw 2500Kbit/s delay 25ms