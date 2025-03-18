# Use latest UBI8 image for building from
FROM registry.access.redhat.com/ubi8/ubi:latest AS buildbase
# Install build dependencies and cleanup
RUN yum -y install tar unzip glibc.i686 gcc dos2unix cmake gcc-c++ && yum clean all && rm -rf /var/cache/yum
# Make our home folder
RUN mkdir -p ~/uox3

FROM buildbase AS buildapp
WORKDIR /root/uox3
COPY . .
# Ensure Linux linefeeds
RUN find /root/uox3 -name \* -type f -exec dos2unix {} \;
# Build it
RUN cd /root/uox3 && ./automake.sh
CMD ["/bin/bash"]


# Now actually generate the executable image
FROM registry.access.redhat.com/ubi8/ubi:latest
RUN yum -y install glibc.i686 dos2unix && yum clean all && rm -rf /var/cache/yum
# Define our non-root user
RUN adduser --system --create-home uox3
USER uox3
RUN mkdir -p ~/app
WORKDIR /home/uox3/app
# Copy in the built application as our user
COPY --from=buildapp --chown=uox3 /root/uox3/uox3 .
COPY --chown=uox3 data data/
COPY --chown=uox3 docs docs/
COPY --chown=uox3 docker/run.sh .
RUN chmod 777 /home/uox3/app/run.sh && dos2unix /home/uox3/app/run.sh
CMD ["/home/uox3/app/run.sh"]