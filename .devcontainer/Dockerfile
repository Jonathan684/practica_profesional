FROM ubuntu:22.04

RUN apt update
RUN apt-get -y upgrade

RUN apt-get install -y python3
RUN apt-get -y update
RUN apt-get -y install python3-pip
RUN python3 -m pip install numpy
RUN pip install matplotlib
RUN pip install mpld3


RUN apt-get -y install git
RUN apt-get -y install wget
RUN apt update && apt install -y make
RUN apt-get -y install vim
RUN apt-get -y install iputils-ping
RUN apt-get -y install unzip
RUN apt-get -y install xz-utils
RUN apt-get -y update
RUN apt-get -y install make
RUN apt-get -y install sshpass
RUN apt-get -y install tree

RUN wget -P/usr/local/bin "https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2018.08/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf.tar.xz?revision=51f3ba22-a569-4dda-aedc-7988690c3c17&ln=en&hash=2DC0BEB0D6E413AEC39F456D213578D2"
RUN cd /usr/local/bin && tar -Jxvf gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf.tar.xz\?revision\=51f3ba22-a569-4dda-aedc-7988690c3c17\&ln\=en\&hash\=2DC0BEB0D6E413AEC39F456D213578D2

RUN wget -P/home "https://github.com/analogdevicesinc/plutosdr-fw/releases/download/v0.32/sysroot-v0.32.tar.gz"
RUN cd /home && tar -xf sysroot-v0.32.tar.gz
RUN cd /home && mv staging $HOME/pluto-0.32.sysroot
RUN mkdir /tmp/plutoapp
RUN cd /tmp/plutoapp && wget "https://raw.githubusercontent.com/analogdevicesinc/libiio/master/examples/ad9361-iiostream.c"
ENV PATH=$PATH:/usr/local/bin/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin
