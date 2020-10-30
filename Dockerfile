FROM opencvcourses/opencv-docker:latest
ENV LD_LIBRARY_PATH=/usr/local/lib
COPY . /home/EzSocket
WORKDIR /home/EzSocket
CMD ["./run_examples.sh"]