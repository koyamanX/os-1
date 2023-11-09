FROM ubuntu:22.04
MAINTAINER ckoyama<ckoyama.1996@gmail.com>

RUN apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gcc-riscv64-unknown-elf libc6-dev-riscv64-cross gcc-riscv64-linux-gnu \
	cmake bash build-essential git ca-certificates texinfo qemu-user qemu-user-binfmt

COPY ./install_deps.sh /tmp
RUN bash /tmp/install_deps.sh

CMD ["/bin/bash"]
