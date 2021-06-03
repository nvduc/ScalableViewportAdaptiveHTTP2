#!/bin/bash

ipfw -q flush
ipfw pipe flush

ipfw add pipe 1 ip from any to any

ipfw pipe 1 config bw 7595kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9522kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8535kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9382kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9095kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8838kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7355kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4417kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4944kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8281kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6860kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6727kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7668kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7795kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9029kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7358kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7719kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8189kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9557kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8878kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7954kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8286kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 9795kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8035kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5724kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5740kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13362kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6927kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7711kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5308kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 7204kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6224kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11156kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11229kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10724kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8770kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15952kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10328kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12944kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17178kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10735kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 6375kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5922kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5281kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 3618kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 2538kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 173kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10379kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4789kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 4234kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 5994kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 10001kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11567kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12682kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12383kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11911kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 8912kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15774kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 11568kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14348kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12171kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13125kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13217kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 13745kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 12541kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14684kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17308kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17086kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16848kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 18126kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19022kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15654kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 19030kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14872kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 14256kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16078kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 15879kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 16988kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 17701kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 21511kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 24175kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 24808kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 28919kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 32792kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 30510kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 35248kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 32032kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 33986kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 37709kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 38772kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 31752kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 33394kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 41958kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 37578kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 39373kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 39300kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 34333kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 36796kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 33026kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 31712kbit/s delay 5ms
sleep 1s
ipfw pipe 1 config bw 38116kbit/s delay 5ms
sleep 1s