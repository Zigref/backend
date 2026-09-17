FROM fedora:latest

RUN dnf -y update

RUN dnf -y install git curl make clang pkg-config asio-devel sqlite sqlite-devel

RUN dnf clean all

RUN git clone --depth=1 https://github.com/Zigref/Zigref.git

WORKDIR /Zigref/Zigref_backend

RUN make

EXPOSE 8080

CMD ["./a.out"]