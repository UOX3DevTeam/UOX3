FROM quay.io/centos/centos:stream AS buildbase
RUN yum -y install tar unzip glibc.i686 gcc dos2unix cmake gcc-c++ && yum clean all && rm -rf /var/cache/yum
RUN mkdir -p ~/uox3

FROM buildbase AS buildapp
WORKDIR /root/uox3
COPY . .
RUN find /root/uox3 -name \* -type f -exec dos2unix {} \;
RUN cd /root/uox3 && ./automake.sh
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
COPY --chown=uox3 docker/run.sh .
RUN chmod 777 /home/uox3/app/run.sh && dos2unix /home/uox3/app/run.sh
CMD ["/home/uox3/app/run.sh"]