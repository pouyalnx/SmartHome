#! /bin/bash

PWD=/home/pouyasq/Desktop/quera_python/shh_system/

python3 $PWD/cm.py &
sleep 2
python3 $PWD/dm.py &
sleep 10
python3 $PWD/demons/*.py &


