FROM ubuntu:22.04

ARG TARGETPLATFORM

# Install Zenoh
RUN apt-get update && apt-get install -y \
  ca-certificates \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*
RUN echo "deb [trusted=yes] https://download.eclipse.org/zenoh/debian-repo/ /" | tee -a /etc/apt/sources.list.d/zenoh.list > /dev/null
RUN apt-get update && apt-get install -y \
  zenoh \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

# Install Rust
RUN apt-get update && apt-get install -y \
  curl build-essential \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

# Install Python & zenoh-python
RUN apt-get update && apt-get install -y \
  python3-pip \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*
RUN pip install -U pip && \
  pip install --no-cache-dir eclipse-zenoh

# Install Zenoh-c
RUN apt-get update && apt-get install -y \
  cmake git \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*
RUN git clone https://github.com/eclipse-zenoh/zenoh-c.git /usr/local/src/zenoh-c \
  && mkdir -p /usr/local/src/zenoh-c/build \
  && cd /usr/local/src/zenoh-c/build \
  && cmake .. \
  && PATH=$PATH:/root/.cargo/bin cmake --build . --config Release \
  && cmake --build . --target install \
  && rm -rf /usr/local/src/zenoh-c

# Install Erlang
RUN apt-get update && apt-get install -y \
  autoconf m4 libncurses5-dev libwxgtk3.0-gtk3-dev libgl1-mesa-dev libglu1-mesa-dev libpng-dev libssh-dev unixodbc-dev xsltproc fop libxml2-utils libncurses-dev openjdk-11-jdk \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

# Erlang for linux/arm64 from source
ENV OTP_VERSION="26.1.1"
RUN if [ ${TARGETPLATFORM} = "linux/arm64" ]; then \
	( set -xe \
	&& OTP_DOWNLOAD_URL="https://github.com/erlang/otp/archive/OTP-${OTP_VERSION}.tar.gz" \
	&& OTP_DOWNLOAD_SHA256="a47203930e4b34a0e23bdf0a968127e5ec9d0e6c69ccf2e53be81cd2360eee2d" \
	&& runtimeDeps='libodbc1 \
			libsctp1 \
			libwxgtk3.0 \
			libwxgtk-webview3.0-gtk3-0v5' \
	&& buildDeps='unixodbc-dev \
			libsctp-dev \
			libwxgtk-webview3.0-gtk3-dev' \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends $runtimeDeps \
	&& apt-get install -y --no-install-recommends $buildDeps \
	&& curl -fSL -o otp-src.tar.gz "$OTP_DOWNLOAD_URL" \
	&& echo "$OTP_DOWNLOAD_SHA256  otp-src.tar.gz" | sha256sum -c - \
	&& export ERL_TOP="/usr/src/otp_src_${OTP_VERSION%%@*}" \
	&& mkdir -vp $ERL_TOP \
	&& tar -xzf otp-src.tar.gz -C $ERL_TOP --strip-components=1 \
	&& rm otp-src.tar.gz \
	&& ( cd $ERL_TOP \
	  && ./otp_build autoconf \
	  && gnuArch="$(dpkg-architecture --query DEB_HOST_GNU_TYPE)" \
	  && ./configure --build="$gnuArch" \
	  && make -j$(nproc) \
	  && make -j$(nproc) docs DOC_TARGETS=chunks \
	  && make install install-docs DOC_TARGETS=chunks ) \
	&& find /usr/local -name examples | xargs rm -rf \
	&& apt-get purge -y --auto-remove $buildDeps \
	&& rm -rf $ERL_TOP /var/lib/apt/lists/* \
	) ; \
	fi;
# Erlang for linux/amd64 from deb
RUN if [ ${TARGETPLATFORM} = "linux/amd64" ]; then \
	( apt-get update && apt-get install -y \
	libncurses5 libsctp1 \ 
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& curl -fSL -o esl-erlang.deb "https://binaries2.erlang-solutions.com/ubuntu/pool/contrib/e/esl-erlang/esl-erlang_26.1.1-1~ubuntu~jammy_amd64.deb" \
	&& dpkg -i esl-erlang.deb \
	&& rm -rf esl-erlang.deb \
	) ; \
	fi;

# Install Elixir
ENV ELIXIR_VERSION="v1.15.7" \
	LANG=C.UTF-8
# Elixir for linux/arm64 from source
RUN if [ ${TARGETPLATFORM} = "linux/arm64" ]; then \
	( set -xe \
	&& ELIXIR_DOWNLOAD_URL="https://github.com/elixir-lang/elixir/archive/${ELIXIR_VERSION}.tar.gz" \
	&& ELIXIR_DOWNLOAD_SHA256="78bde2786b395515ae1eaa7d26faa7edfdd6632bfcfcd75bccb6341a18e8798f" \
	&& curl -fSL -o elixir-src.tar.gz $ELIXIR_DOWNLOAD_URL \
	&& echo "$ELIXIR_DOWNLOAD_SHA256  elixir-src.tar.gz" | sha256sum -c - \
	&& mkdir -p /usr/local/src/elixir \
	&& tar -xzC /usr/local/src/elixir --strip-components=1 -f elixir-src.tar.gz \
	&& rm elixir-src.tar.gz \
	&& cd /usr/local/src/elixir \
	&& make install clean \
	&& find /usr/local/src/elixir/ -type f -not -regex "/usr/local/src/elixir/lib/[^\/]*/lib.*" -exec rm -rf {} + \
	&& find /usr/local/src/elixir/ -type d -depth -empty -delete \
	&& mix local.hex --force \
	) ; \
	fi;
# Elixir for linux/amd64 from deb
RUN if [ ${TARGETPLATFORM} = "linux/amd64" ]; then \
	( curl -fSL -o elixir.deb "https://binaries2.erlang-solutions.com/ubuntu/pool/contrib/e/elixir/elixir_1.15.7_1_otp_26.1.2~ubuntu~jammy_all.deb" \
	&& dpkg -i elixir.deb \
	&& rm -rf elixir.deb \
	&& mix local.hex --force \
	) ; \
	fi;

CMD ["/bin/bash"]
