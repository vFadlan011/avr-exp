## 74HC595
|Desc|Pin|IC|Pin|Desc|
|--:|--:|:--|:--|:--|
|Qb|1||16|5v|
|Qc|2||15|Qa|
|Qd|3||14|Serial In|
|Qe|4||13|Output Enable (AL)|
|Qf|5||12|Reg CLK|
|Qg|6||11|Shift Reg CLK|
|Qh|7||10|Shift Reg CLR|
|GND|8||9|Qh'|


## 74HC595 - ATMega328P
|SR-Desc|SR-Pin|x|MCU-Pin|MCU-Desc|
|--:|--:|----|:--|:--|
|Serial In|14|<--|2|PD0|
|Output Enable|13|-->|GND|GND|
|Reg CLK|12|<--|3|PD1|
|Shift Reg CLK|11|<--|4|PD2|
|Shift Reg CLR|10|<--|VCC|VCC|