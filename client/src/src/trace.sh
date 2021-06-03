#!/bin/bash
ipfw -q flush
ipfw -q pipe flush
ipfw add pipe 3 ip from 172.16.232.129 to me
ipfw add pipe 3 ip from me to 172.16.232.129

ipfw pipe 3 config bw 2951Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2799Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3094Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3246Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3301Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2614Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2618Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2676Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2706Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2767Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2895Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3083Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3129Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3460Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3409Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1962Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2650Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2837Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2814Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2310Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2324Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1542Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1599Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1365Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 875Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1833Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2197Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2125Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2174Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2115Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2103Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2115Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2136Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2095Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2092Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1868Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1714Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1694Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1800Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1798Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1939Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1891Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1952Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1928Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1788Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1936Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1788Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1613Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1484Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1567Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1719Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1182Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1132Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 817Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 420Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 396Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 724Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 444Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 302Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 440Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 456Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 548Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 477Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 444Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 256Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 361Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 303Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 325Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 209Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 373Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 851Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 724Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 677Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 605Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 583Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1068Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 794Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1063Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1306Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 759Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 887Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 793Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 653Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 585Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 864Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 910Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 804Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 863Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 706Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 607Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 128Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 871Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 350Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 620Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 981Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1413Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1658Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2251Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2311Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2370Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1413Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1448Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1798Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2018Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2533Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2893Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1260Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1144Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1819Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2496Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1694Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1740Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2611Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1892Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2671Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2345Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1436Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3079Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2779Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2473Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2577Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2337Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2010Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2107Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2207Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2209Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2466Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3289Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3449Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2882Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3637Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3982Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3920Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3460Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4133Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1783Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1221Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 687Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1015Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1541Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3105Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3586Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3594Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2856Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2443Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2042Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1368Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1833Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1928Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2589Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3243Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1669Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 642Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 840Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2036Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2557Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2452Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2276Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2181Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2501Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2150Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1870Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1691Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1505Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 944Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1110Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1313Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 759Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 126Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2134Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2503Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3682Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4779Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3337Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2947Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4906Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5165Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5213Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5514Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4728Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5552Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5640Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5078Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4750Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4549Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2627Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1152Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 382Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1133Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 619Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 678Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 930Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1172Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1692Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 956Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1249Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1009Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1294Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2315Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2905Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3511Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2520Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1879Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2037Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1862Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1398Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 412Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1332Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 935Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1424Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1954Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1684Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1585Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1914Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2304Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2741Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1681Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1913Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1454Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 741Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 105Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 444Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 210Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3401Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3689Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3546Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3595Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3777Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3681Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3764Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3258Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3968Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3764Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4090Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3101Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3365Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1828Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2569Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2349Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2241Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3435Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3083Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2613Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3476Kbit/s delay 25ms
sleep 1s

ipfw -q flush
ipfw -q pipe flush
