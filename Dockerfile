# Use Ubuntu Trusty LTS
FROM ubuntu:trusty-20170330

# Pre-cache neurodebian key
COPY .docker/neurodebian.gpg /root/.neurodebian.gpg

# Prepare environment
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
                    curl \
                    bzip2 \
                    ca-certificates \
                    xvfb \
                    pkg-config && \
    curl -sSL http://neuro.debian.net/lists/trusty.us-ca.full >> /etc/apt/sources.list.d/neurodebian.sources.list && \
    apt-key add /root/.neurodebian.gpg && \
    (apt-key adv --refresh-keys --keyserver hkp://ha.pool.sks-keyservers.net 0xA5D32F012649A5A9 || true) && \
    apt-get update

# Installing Ubuntu packages and cleaning up
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
                    libinsighttoolkit4-dev=4.7.0-1~nd14.04+1 \
                    cmake=2.8.12.2-0ubuntu3 \
                    g++ \
                    build-essential \
                    libjsoncpp-dev \
                    libvtk6-dev \
                    libvtkgdcm2-dev \
                    libboost-filesystem-dev \
                    libboost-system-dev \
                    libboost-program-options-dev \
                    libfftw3-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY ./Code /root/regseg/src
RUN mkdir /root/regseg/Release && \
    cd /root/regseg/Release && \
    cmake ../src/ -G"Unix Makefiles"  -DCMAKE_BUILD_TYPE=Release -DITK_DIR=/usr/local/lib/cmake/ITK-4.7/ && \
    make -j$( grep -c ^processor /proc/cpuinfo ) && \
    make install

ENTRYPOINT ["/usr/local/bin/regseg"]

# Store metadata
ARG BUILD_DATE
ARG VCS_REF
ARG VERSION
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name="RegSeg" \
      org.label-schema.description="RegSeg -" \
      org.label-schema.url="https://github.com/oesteban/RegSeg" \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url="https://github.com/oesteban/RegSeg" \
      org.label-schema.version=$VERSION \
      org.label-schema.schema-version="1.0"
