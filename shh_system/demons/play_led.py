import dm
import time


d=dm.demon()
d.connect('',2027)
while 1:
    print(d.Set("system/leds/led0",'1'))
    time.sleep(1)
    print(d.Set("system/leds/led0",'0'))
    time.sleep(1)
