FROM quay.io/pypa/manylinux1_i686
MAINTAINER The aGruM Team
LABEL version="1.1"

ADD https://ftp.pcre.org/pub/pcre/pcre-8.40.tar.gz /opt/
ADD https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz /opt/
ADD https://downloads.sourceforge.net/project/swig/swig/swig-3.0.12/swig-3.0.12.tar.gz /opt/

WORKDIR /opt
RUN tar xzf pcre-8.40.tar.gz
WORKDIR /opt/pcre-8.40
RUN ./configure
RUN make
RUN make install

WORKDIR /opt
RUN tar xzf cmake-3.7.2.tar.gz
WORKDIR /opt/cmake-3.7.2
RUN ./configure
RUN make
RUN make install

WORKDIR /opt
RUN tar xzf swig-3.0.12.tar.gz
WORKDIR /opt/swig-3.0.12
RUN ./configure
RUN make
RUN make install

WORKDIR /opt
RUN rm -rf cmake-3.7.2* swig-3.0.12*

RUN rm -f python/cp26-cp26m* python/cp33-cp33m python/cp34-cp34m

