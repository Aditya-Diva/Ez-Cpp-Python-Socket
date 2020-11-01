FROM opencvcourses/opencv-docker:latest
ENV LD_LIBRARY_PATH=/usr/local/lib
ENV PYTHONPATH=/home/EzSocket/python/
COPY . /home/EzSocket
WORKDIR /home/EzSocket
CMD ["./run_examples.sh"]
