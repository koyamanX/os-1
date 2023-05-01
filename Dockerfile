FROM ubuntu:latest
MAINTAINER ckoyama<ckoyama.1996@gmail.com>

RUN apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gcc-riscv64-unknown-elf cmake bash build-essential

CMD ["/bin/bash"]
