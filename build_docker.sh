echo "Choose your preference:"
echo "1. Run demo directly"
echo "2. Load into docker to dev/play"
read -p "Choose how you would like to launch the docker: " choice

if [ $choice -eq 1 ] 
then
    echo -e '$d\nw\nq'| ed Dockerfile
    echo "CMD [\"./run_examples.sh\"]" >> Dockerfile
elif [ $choice -eq 2 ]
then
    echo -e '$d\nw\nq'| ed Dockerfile
    echo "# CMD [\"./run_examples.sh\"]" >> Dockerfile
else
    echo "Unrecognized input. Please run again."
fi

docker build -t ezsocket .