#!/bin/bash
echo "Please choose one of the example node pairs that you would like to run:"
echo "  Server  Client"
echo "1. Cpp     Cpp  "
echo "2. Cpp     Py   "
echo "3. Py      Cpp  "
echo "4. Py      Py   "
read -p "Choose the config that you'd like to run: " choice

if [ $choice -eq 1 ] 
then
    cd cpp/examples/2.Tokens/
    ./make.sh
    ./run_server &

    ./run_client

elif [ $choice -eq 2 ]
then
    cd cpp/examples/2.Tokens/
    ./make.sh
    ./run_server &

    cd ../../../python/examples/2.Tokens/
    python3 run_client.py

elif [ $choice -eq 3 ]
then
    cd python/examples/2.Tokens/
    python3 run_server.py &

    cd ../../../cpp/examples/2.Tokens/
    ./make.sh
    ./run_client

elif [ $choice -eq 4 ]
then
    cd python/examples/2.Tokens/
    python3 run_server.py &
    python3 run_client.py

else
    echo "Unrecognized input. Please run again."
fi