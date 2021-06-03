#!/bin/bash

ipfw -q flush
ipfw pipe flush

ipfw add pipe 1 ip from any to any


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
ipfw pipe 1 config bw 1857kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 29627kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8992kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11797kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9802kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9642kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8528kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9589kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9595kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8007kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12525kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6815kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1812kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15076kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11259kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13644kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16566kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14580kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18438kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18678kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16305kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11927kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8069kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18866kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21177kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 24141kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 24133kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22366kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16650kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14407kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15998kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15057kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15149kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16289kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18683kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 20613kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13469kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4945kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2495kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11305kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1057kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22539kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8433kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6426kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 30905kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15085kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14584kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12402kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16305kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18893kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21128kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 23429kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21624kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 20310kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22860kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17491kbit/s delay 5ms
sleep 1s