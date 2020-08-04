FROM ubuntu:16.04
RUN apt-get update; apt-get install libtesseract3 libtesseract-dev libleptonica-dev liblept5 build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev -y; apt-get clean; rm -rf /var/lib/apt/lists/*
RUN git clone https://github.com/opencv/opencv.git; git clone https://github.com/opencv/opencv_contrib.git; cd /opencv; git checkout 3.2.0; cd /opencv_contrib; git checkout 3.2.0; mkdir /opencv/build; cd /opencv/build; cmake -D OPENCV_EXTRA_MODULES_PATH="../../opencv_contrib/modules/" BUID_DOCS BUILD_EXAMLES CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..; make -j4; make install; rm -rf opencv opencv_contrib
RUN mkdir -p /usr/src/app
COPY . /usr/src/app/
WORKDIR /usr/src/app
RUN cd /usr/src/app/; rm CMakeCache.txt; cmake .; make -j4;
CMD ["./uic"]
