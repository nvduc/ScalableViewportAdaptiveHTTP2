#!/bin/bash
# server addr
SERVER_ADDR=127.0.0.1
#
ipfw -q flush
ipfw -q pipe flush
echo ipfw add pipe 3 ip from ${SERVER_ADDR} to me
echo ipfw add pipe 3 ip from me to ${SERVER_ADDR}

ipfw pipe 3 config bw 5336Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5026Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4739Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4461Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3798Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4403Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5292Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4556Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4623Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5365Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5062Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4797Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4814Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4967Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4956Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4535Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4549Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4644Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3161Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2595Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3541Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3756Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2566Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1624Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2219Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2601Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2727Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2659Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2564Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3099Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3491Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3324Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3123Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3519Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3897Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3294Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3599Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4404Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4121Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2446Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1677Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2412Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2884Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3568Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3790Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3805Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3860Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3801Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3989Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3993Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4511Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4648Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4668Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4475Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3881Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4130Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4690Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4721Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4305Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4706Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4991Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3820Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2471Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2917Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3788Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3876Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3908Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3596Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3376Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3309Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3027Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2351Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1838Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1788Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1640Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1513Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2274Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2953Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2573Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2122Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2356Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2413Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3003Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3912Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4304Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4268Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3865Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4261Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3861Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2928Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3165Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3395Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3752Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4392Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4743Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4645Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3961Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3804Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3732Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3608Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3371Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3107Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3574Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3884Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3794Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3946Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4204Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4033Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3857Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3394Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2490Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2574Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3372Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3376Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3608Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3698Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3135Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2721Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3029Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3504Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3697Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3837Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3137Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2289Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2304Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2436Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2488Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2677Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2941Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3399Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3612Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3555Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3599Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3832Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3741Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3052Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1795Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1800Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3038Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3154Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1759Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 869Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 921Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1558Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2455Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3019Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3220Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3575Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4304Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4590Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4624Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4903Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3161Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2693Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3637Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3845Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3583Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3429Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3735Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3925Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3700Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3456Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2629Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1951Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1415Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1249Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1067Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1355Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2559Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3171Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3650Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4422Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5055Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5179Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5025Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4935Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4664Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4559Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4390Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3895Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3154Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1918Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1582Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3661Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4563Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3307Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2656Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2257Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1621Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1799Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2464Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2580Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2515Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2156Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1473Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 816Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 838Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1573Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2031Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2219Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2647Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2968Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2832Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2466Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2356Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2535Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2582Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2541Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2672Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1375Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1201Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1645Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1620Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2551Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2270Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2578Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1758Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1713Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3219Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2866Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2470Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2417Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2450Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2286Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2018Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2001Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2137Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2073Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2037Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2061Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1975Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1868Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1954Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2162Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2088Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1722Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1722Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2114Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2104Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1671Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1705Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2147Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2330Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2349Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2359Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2463Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2565Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2857Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2995Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2985Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3483Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3601Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4028Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3810Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3817Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3872Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4143Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4354Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4900Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5099Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4574Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4340Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3960Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4012Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4040Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4120Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4577Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5250Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5303Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5075Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4843Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4999Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5303Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5308Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5362Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4998Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4846Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5283Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5519Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5054Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5066Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5242Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4380Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3171Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3600Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4630Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4802Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5261Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5386Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5209Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5377Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5104Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4952Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5363Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5353Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5347Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5401Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5479Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5248Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5059Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4401Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4392Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5006Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4932Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5028Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5215Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5154Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5212Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5455Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5417Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5525Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5496Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5496Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5549Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5265Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4135Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3273Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3361Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3655Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3913Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3956Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4011Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4186Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4287Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4740Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4770Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4495Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4929Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4723Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4286Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4088Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3553Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3221Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3145Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3397Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3668Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3797Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3990Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4463Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5180Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5450Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5287Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5351Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5163Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3382Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1916Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1831Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1786Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1597Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 765Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1567Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3592Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3649Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1781Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1273Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2785Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3783Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4037Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4022Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4006Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4154Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4215Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4057Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4242Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4242Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4067Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4048Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3972Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3711Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3783Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3839Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3652Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3822Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3863Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3841Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3806Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3667Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3543Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3024Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2231Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2238Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3345Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4165Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4186Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3571Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3066Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2993Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2701Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1950Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1765Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1883Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1157Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 998Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2425Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3457Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3575Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3576Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3780Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2785Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2875Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3895Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3909Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3866Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3820Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3833Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3911Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3891Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3890Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3915Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3820Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3870Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3886Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3849Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3816Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3886Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3868Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3874Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3923Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3786Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3819Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3894Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3532Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3041Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2728Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2756Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3035Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3095Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3142Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3269Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3511Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3768Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3846Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3724Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3824Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3919Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3843Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3872Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3806Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3834Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3904Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3888Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3865Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3895Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3803Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3708Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3942Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3997Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3874Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3840Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3846Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3868Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3829Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3856Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3825Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3952Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3898Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3772Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3559Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3009Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2843Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2884Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3025Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2747Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2281Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2439Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2548Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2398Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2445Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2210Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2113Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2079Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1739Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2093Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2682Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2947Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2943Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2558Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2983Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3116Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2620Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2119Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1520Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1072Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 702Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 552Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 492Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 686Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 438Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 195Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 306Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 217Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 214Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 413Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 518Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1311Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1895Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1485Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1346Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1401Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1016Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 903Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1026Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1143Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1091Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 904Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 910Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 869Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1009Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 968Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 828Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 968Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1055Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1114Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1155Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1201Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1220Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 823Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 484Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 426Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 548Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 731Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 609Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 566Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 542Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 490Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 496Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 759Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 992Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 884Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 656Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 653Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 711Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 664Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 682Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 752Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 782Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 775Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 857Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 881Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 805Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 653Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 501Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 518Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 483Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 455Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 513Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 572Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 647Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 530Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 402Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 619Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 928Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1267Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1483Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1575Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1437Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1392Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1729Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1980Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2020Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2079Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2797Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3169Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2570Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2657Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2802Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2605Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2642Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2556Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2397Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1869Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1978Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2393Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2442Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2756Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2875Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2946Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3101Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3177Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3273Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2957Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2616Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2647Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2691Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2736Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2831Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2989Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3106Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3294Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3434Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2685Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2306Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2743Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2825Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2562Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2317Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1933Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1570Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1482Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1120Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1354Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2015Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2161Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2149Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2144Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2125Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2115Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2093Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1980Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1791Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1704Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1747Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1799Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1868Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1915Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1921Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1940Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1858Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1862Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1862Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1700Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1548Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1525Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1643Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1450Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1157Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 974Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 618Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 408Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 560Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 584Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 373Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 371Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 448Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 502Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 512Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 460Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 350Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 308Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 332Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 314Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 267Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 291Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 612Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 700Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 641Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 594Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 291Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 534Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 931Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 928Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1184Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1032Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 823Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 840Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 723Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 619Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 724Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 887Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 857Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 833Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 784Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 656Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 367Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 499Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 610Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 485Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 800Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1197Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1535Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1954Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2281Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2340Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1891Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1430Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1623Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1908Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2275Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2713Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2076Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1202Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1481Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2157Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2095Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1717Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2175Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2251Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2281Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2508Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1890Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2257Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2929Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2626Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2525Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2457Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2173Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2058Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2157Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2208Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2337Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2877Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3369Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3165Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3259Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3809Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3951Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3690Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3796Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2958Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1502Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 954Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 851Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1278Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2323Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3345Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3590Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3225Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2649Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2242Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1705Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1600Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1880Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2258Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2916Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2456Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1155Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 741Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1438Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2296Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2504Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2364Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2228Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2341Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2325Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2010Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1780Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1598Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1224Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1027Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1211Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1036Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 442Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1130Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2318Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3092Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4230Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4058Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3142Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3926Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5035Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5189Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5363Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5121Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5140Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5596Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 5359Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4914Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4649Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3588Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1889Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 767Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 191Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 589Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 876Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 648Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 804Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1051Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1432Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1324Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1102Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 624Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 504Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1151Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1804Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2610Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3208Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3015Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2199Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1958Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1949Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1630Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 905Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 252Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 712Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1133Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1179Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1689Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1819Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1634Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1749Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2109Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2522Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2211Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1797Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1683Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1097Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 423Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 274Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 327Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1805Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3545Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3617Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3570Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3686Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3729Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3722Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3511Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3613Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3866Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3927Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3595Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3233Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2596Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2198Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2459Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2295Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2838Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3259Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2848Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3044Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3127Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2525Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2307Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2442Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2291Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1500Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1325Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1628Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1476Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1460Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1574Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1501Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1400Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1355Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1354Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1395Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1243Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1254Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1839Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2301Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1935Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1787Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2255Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1990Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1519Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1627Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2090Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2612Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2705Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3127Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3244Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2676Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2757Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3179Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3313Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2978Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2828Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3123Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3009Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3278Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3351Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3111Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3288Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3623Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3888Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3930Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4254Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4210Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4170Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4693Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4480Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4363Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4406Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4050Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4059Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4013Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3710Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2880Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2033Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2896Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3574Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3429Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4365Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4487Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3659Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2117Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 571Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2736Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2213Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2621Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3888Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3572Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3545Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3660Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3785Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3689Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3260Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2925Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2950Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3042Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2983Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2773Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2583Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2235Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1712Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1290Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1108Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1032Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 839Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 822Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1173Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1196Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1045Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1050Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1056Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 893Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1044Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1285Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1034Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1045Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1232Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1287Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1339Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1350Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1374Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1450Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1496Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1565Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1682Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1839Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2067Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2260Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2530Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2677Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2748Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2748Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2771Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2804Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2669Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2845Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3025Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3079Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3176Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3092Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2756Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2284Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2441Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3164Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3467Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3171Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3096Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2697Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1611Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1091Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1114Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1407Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1721Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1397Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 628Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 712Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1132Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1307Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1493Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1517Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1436Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1149Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1160Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 822Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 350Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 934Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1330Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1009Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 764Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 700Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 636Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 379Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 100Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 238Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 227Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 180Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 244Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 489Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 880Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 881Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 653Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 1834Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3183Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 2930Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3189Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3742Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3906Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3793Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3172Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3088Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3331Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3321Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 3852Kbit/s delay 25ms
sleep 1s

ipfw pipe 3 config bw 4483Kbit/s delay 25ms
sleep 1s

ipfw -q flush
ipfw -q pipe flush
