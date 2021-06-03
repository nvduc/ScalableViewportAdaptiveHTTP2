#!/bin/bash

ipfw -q flush
ipfw -q pipe flush

ipfw add pipe 1 ip from any to any

ipfw pipe 1 config bw 8000kbit/s delay 5ms