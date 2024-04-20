# Dockerfile for hogutils
#
# hogUtils-i686 is a dynamically linked 32-bit x86 binary, this is difficult on an aarch64 system,
# so just wrap it in a debian container. We set QEMU_GUEST_BASE so it's friendly with "qemu-user" mode emulation.
#
# build in this directory, example:
# $ podman build . -t localhost/hogutils
#
# then run in one of the scripts/data/linux*hog directories, example:
# $ podman run -it -v .:/work:Z localhost/hogutils --makehog d3linuxfullhog.txt
#
# in the same directory, verify you see new.hog

FROM --platform=linux/386 debian:bookworm
ENV QEMU_GUEST_BASE=0x10000

RUN apt-get update && apt-get install -y --no-install-recommends libxext6 && rm -rf /var/lib/apt/lists/*

RUN mkdir /app
COPY hogUtils-i686 /app

WORKDIR /work

ENTRYPOINT ["/app/hogUtils-i686"]
