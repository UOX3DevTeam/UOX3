#syntax = docker/dockerfile:1.4.0

FROM quay.io/centos/centos:stream AS buildbase
RUN yum -y install tar unzip glibc.i686 gcc dos2unix make gcc-c++ && yum clean all && rm -rf /var/cache/yum
RUN mkdir -p ~/uox3

FROM buildbase AS buildjs
COPY automake.sh /root/uox3
COPY spidermonkey /root/uox3/spidermonkey/
RUN find /root/uox3 -name \* -type f -exec dos2unix {} \;
RUN cd /root/uox3 && ./automake.sh 1
CMD ["/bin/bash"]

FROM buildbase AS buildzlib
COPY automake.sh /root/uox3
COPY zlib /root/uox3/zlib/
RUN find /root/uox3 -name \* -type f -exec dos2unix {} \;
RUN cd /root/uox3 && ./automake.sh 2
CMD ["/bin/bash"]

FROM buildbase AS buildapp
WORKDIR /root/uox3
RUN mkdir -p spidermonkey && mkdir -p zlib
COPY automake.sh .
COPY source source/
RUN find /root/uox3 -name \* -type f -exec dos2unix {} \;
COPY --from=buildjs   /root/uox3/spidermonkey spidermonkey/
COPY --from=buildzlib /root/uox3/zlib         zlib/
RUN cd /root/uox3 && ./automake.sh 3
CMD ["/bin/bash"]

FROM quay.io/centos/centos:stream 
RUN yum -y install glibc.i686 dos2unix && yum clean all && rm -rf /var/cache/yum
RUN adduser --system --create-home uox3
USER uox3
RUN mkdir -p ~/app
WORKDIR /home/uox3/app
COPY --from=buildapp --chown=uox3 /root/uox3/uox3 .
COPY --chown=uox3 data data/
COPY --chown=uox3 docs docs/
RUN <<EOF cat >> ~/app/run.sh
#!/usr/bin/env bash
cd ~/app
./uox3 data/uox.ini
EOF
RUN chmod 777 /home/uox3/app/run.sh && dos2unix /home/uox3/app/run.sh
CMD ["/home/uox3/app/run.sh"]