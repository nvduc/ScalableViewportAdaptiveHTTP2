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
ipfw pipe 1 config bw 18905kbit/s delay 5ms
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
ipfw pipe 1 config bw 13831kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4475kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2114kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12792kbit/s delay 5ms
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
ipfw pipe 1 config bw 10263kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10003kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12203kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11372kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11669kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12348kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12479kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11829kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12796kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13686kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11258kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12158kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10935kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1019kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 23005kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13155kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13233kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17634kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10628kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12465kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10251kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22942kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11487kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9211kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1857kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 550kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 27627kbit/s delay 5ms
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
ipfw pipe 1 config bw 10525kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6815kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 1812kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 500kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13076kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11259kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11644kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14566kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12580kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16438kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16678kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14305kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11927kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8069kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16866kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19177kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22141kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 22133kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 20366kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14650kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12407kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13998kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13057kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13149kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14289kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16683kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18613kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11469kbit/s delay 5ms
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
ipfw pipe 1 config bw 20539kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8433kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6426kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 28905kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13085kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12584kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10402kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14305kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16893kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19128kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21429kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19624kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18310kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 20860kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15491kbit/s delay 5ms
sleep 1s