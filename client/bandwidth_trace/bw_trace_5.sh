#!/bin/bash

ipfw -q flush
ipfw pipe flush

ipfw add pipe 1 ip from any to any

ipfw pipe 1 config bw 29838kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 43988kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 34025kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 29913kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21868kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 31490kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22301kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18135kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16737kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15216kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5334kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 23897kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8649kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2308kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6145kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6090kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7673kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10008kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7660kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8339kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6494kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5689kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5662kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 20905kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11167kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10973kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10606kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5645kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2753kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15831kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4475kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2114kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14792kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9629kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7657kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5737kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6125kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7150kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9296kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7872kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8315kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8015kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8972kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12263kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12003kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14203kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13372kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13669kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14348kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14479kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13829kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14796kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15686kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13258kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14158kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12935kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1019kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 25005kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15155kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15233kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19634kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12628kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14465kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12251kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 24942kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11487kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9211kbit/s delay 5ms
sleep 1s