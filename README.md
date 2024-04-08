# zenoh_d3ai_trial

Quick trial a.k.a practice to learn the power of Zenoh for D3-AI :D

## Tool versions (installed in Docker)

- Ubuntu 22.04 (base image)
- Zenoh v0.10.1-rc
- Rust 1.75.0
- Python 3.10.12
- Erlang/OTP 26.1.1
- Elixir 1.15.7-otp-26

## Prepare Docker env

### Quickstart by Docker Hub image [recommended?]

Pre-built Docker image has been published on [Docker Hub](https://hub.docker.com/repository/docker/takasehideki/zenoh_d3ai_trial/general)

```
cd <git_cloned_dir>
docker run -it --rm -v `pwd`:/zenoh_d3ai_trial -w /zenoh_d3ai_trial --name zenoh_d3ai_trial takasehideki/zenoh_d3ai_trial
```

Currently, the operation in the pre-built Docker image is mainly confirmed on my M1 Mac (arm64) machine.
However, we heard reports of it working on x64 (amd64) machines as well.
If you have any problems (especially on x64 (amd64) machines), please feel free to let me know via [Issues](https://github.com/takasehideki/zenoh_d3ai_trial/issues).

### Build the image and use it locally

Please enjoy the coffee break because building the image may take too long time :-

```
cd <git_cloned_dir>
docker build -t zenoh_d3ai_trial .
docker run -it --rm -v `pwd`:/zenoh_d3ai_trial -w /zenoh_d3ai_trial --name zenoh_d3ai_trial zenoh_d3ai_trial
```

#### MEMO for ME: build and push the image to Docker Hub

The following operation has been confirmed on my M1Mac.
Note that the source build of Elixir failed for another target (for amd64 on M1Mac, or arm64 on x64/Ubuntu machine).

```
docker buildx create --name mybuilder
docker buildx use mybuilder
docker buildx build --platform linux/amd64,linux/arm64 -t takasehideki/zenoh_d3ai_trial . --push
```

## Communication between anywhere in programming languages

All subsequent terminal operations are assumed to be performed by entering the Docker container with the following command.

```
docker exec -it zenoh_d3ai_trial /bin/bash
```

### zenoh_native

Zenoh nodes (natively) implemented in Rust.  
The Source code is referred to https://docs.rs/zenoh/0.10.1-rc/zenoh/#examples

- Build
```
cd zenoh_native
cargo build
```

If the first build fails in Docker env, just try it again ;(

- Run
```
./target/debug/pub
```
```
./target/debug/sub
```

### zenoh_python

Zenoh nodes implemented in Python.  
The Source code is referred to https://zenoh.io/docs/getting-started/first-app/

- Run
```
cd zenoh_python
python3 pub.py
```
```
cd zenoh_python
python3 sub.py
```

### zenoh_c

Zenoh nodes (natively) implemented in C.  
The Source code is referred to https://ar-ray.hatenablog.com/entry/2023/05/04/121035 and https://github.com/eclipse-zenoh/zenoh-c/tree/main/examples

- Build
```
cd zenoh_c
mkdir build && cd build
cmake ..
cmake --build .
```

- Run
```
./pub
```
```
./sub
```

### zenoh_elixir

Zenoh nodes implemented in Elixir :-

- Build
```
cd zenoh_elixir
mix deps.get
mix compile
```

- Run
```
iex -S mix
iex()> ZenohElixir.Pub.main
```
```
iex -S mix
iex()> ZenohElixir.Sub.main
```
