FROM opencvcourses/opencv-docker:latest
RUN adduser --disabled-password --gecos '' user
RUN usermod -aG video user
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN pip3 install ezpysocket
COPY --chown=user . /home/EzSocket
WORKDIR /home/EzSocket
# CMD ["./run_examples.sh"]
