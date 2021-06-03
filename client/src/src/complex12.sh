#!/bin/bash
ipfw -q flush
ipfw -q pipe flush
ipfw add pipe 3 ip from 172.16.232.129 to me
ipfw add pipe 3 ip from me to 172.16.232.129

ipfw pipe 3 config bw 3243Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1669Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 642Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 840Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2036Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2557Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2452Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2276Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2181Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2501Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2150Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1870Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1691Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1505Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 944Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1110Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1313Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 759Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 126Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2134Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2503Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3682Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4779Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3337Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2947Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4906Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5165Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5213Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5514Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4728Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5552Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5640Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5078Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4750Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4549Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2627Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1152Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 382Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1133Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 619Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 678Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 930Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1172Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1692Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 956Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1249Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1009Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1294Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2315Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2905Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3511Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2520Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1879Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2037Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1862Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1398Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 412Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1332Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 935Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1424Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1954Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1684Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1585Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1914Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2304Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2741Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1681Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1913Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1454Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 741Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 105Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 444Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 210Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3401Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3689Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3546Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3595Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3777Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3681Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3764Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3258Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3968Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3764Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4090Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3101Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3365Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1828Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2569Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2349Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2241Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3435Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3083Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2613Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3476Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2778Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2272Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2343Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2541Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2042Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 958Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1693Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1563Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1390Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1530Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1618Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1385Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1415Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1296Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1413Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1378Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1108Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1400Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2278Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2324Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1546Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2028Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2483Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1497Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1541Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1714Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2466Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2758Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2653Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3602Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2887Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2466Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3048Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3311Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3316Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2641Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3015Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3231Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2787Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3770Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2933Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3290Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3286Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3960Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3817Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4043Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4466Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3954Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4386Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5001Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3960Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4766Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4047Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4054Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4064Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3962Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3459Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2302Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1764Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4028Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3120Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3738Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4993Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3981Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3338Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 897Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 245Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5227Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5446Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4606Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4873Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4049Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3547Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5260Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5324Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3789Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5458Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5272Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4853Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4742Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4886Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5048Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4864Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4207Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4892Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4396Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1926Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3264Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3818Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3694Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1438Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1811Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2628Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2574Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2881Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2437Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2692Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3506Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3477Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3172Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3074Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3965Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4254Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3541Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3048Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4150Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4659Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3584Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1309Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2045Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2779Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2989Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4148Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3432Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4179Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3542Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4060Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3919Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4067Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4955Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4342Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4995Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3955Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3808Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4453Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4927Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4515Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4095Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 5318Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4664Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2976Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1966Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3868Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3709Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 4044Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3773Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3420Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3332Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3287Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2767Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1936Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1741Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1836Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1445Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 1581Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2968Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2938Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2208Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2036Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2676Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 2150Kbit/s delay 50ms
sleep 1s

ipfw pipe 3 config bw 3857Kbit/s delay 50ms
sleep 1s

ipfw -q flush
ipfw -q pipe flush
