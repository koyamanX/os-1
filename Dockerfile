FROM ubuntu:latest
MAINTAINER ckoyama<ckoyama.1996@gmail.com>

RUN apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gcc-riscv64-unknown-elf cmake bash build-essential git ca-certificates texinfo

COPY ./install_deps.sh /tmp
RUN bash /tmp/install_deps.sh

CMD ["/bin/bash"]
