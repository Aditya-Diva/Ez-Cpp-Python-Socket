#!/bin/bash
cd python
pip3 install -r requirements.txt
python3 run.py &

cd ../cpp
./make.sh
./client