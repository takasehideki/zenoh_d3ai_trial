# zenoh_d3ai_trial

Quick trial a.k.a practice to learn the power of Zenoh for D3-AI :D

## Tool versions (installed in Docker)

- Ubuntu 22.04 (base image)
- Zenoh v0.10.1-rc
- Rust 1.77.1
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
cd zenoh_native
./target/debug/pub
```
```
cd zenoh_native
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
cd zenoh_c
./build/pub
```
```
cd zenoh_c
./build/sub
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
cd zenoh_elixir
iex -S mix
iex()> ZenohElixir.Pub.main
```
```
cd zenoh_elixir
iex -S mix
iex()> ZenohElixir.Sub.main
```

## Communication with cloud

Let's connect to the cloud with Zenoh!

### Preliminary

Please prepare the cloud environment as the example for Azure VM.

- Deploy the Ubuntu 22.04 LTS instance as [the follow](https://qiita.com/t_ymgt/items/0c473f73cfe4794a8036) except for http and https accesses.
- Add the connection rules
  - send port 4000 for Phoenix
  - both send and receive ports 7447 for Zenoh
- Install Erlang/OTP 26.1.1 and Elixir 1.15.7

### Operation

On the cloud side, please deploy the following Phoenix project (`d3ai_demo` branch for now).  
https://github.com/b5g-ex/zenohex_phoenix_demo/tree/d3ai_demo

- 1st terminal: deploy Phoenix server
```
git clone -b d3ai_demo https://github.com/b5g-ex/zenohex_phoenix_demo
### start Phoenix server according to README
mix phx.server
```
- 2nd terminal: operate zenohd
```
zenohd
```

On the local side (your laptop), operate zenohd in the container, and then you can operate any pub/sub zenoh nodes!

- 3rd terminal: operate zenohd
```
docker exec -it zenoh_d3ai_trial /bin/bash
zenohd -e tcp/<cloud_ip_or_url>:7447
```
- 4th terminal: Python subscriber (e.g.)
```
docker exec -it zenoh_d3ai_trial /bin/bash
python3 zenoh_python/pub.py
```

## zenoh-pico: From device to the cloud!

Embedded devices can also communicate to the cloud with Zenoh!

The source code for zenoh-pico app is referred to the following under EPL-2.0 OR Apache-2.0 license.
 https://github.com/eclipse-zenoh/zenoh-pico/tree/main/examples/arduino

### Preliminary

Please install VS Code and its PlatformIO extension on the host.  
https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode

This example uses [M5Stack CoreS3](https://docs.m5stack.com/en/core/CoreS3) as the example.
You may change the setting in platformio.ini when using other boards.
Also, this example assumes that the Phoenix server has been deployed according to the previous section.

### Operation

- Open `zenoh_pico/` directory as the PlatformIO Project
- Edit `src/main.cpp` to adjust your environment
  - `SSID` and `PASS` in L13-14 for Wi-Fi AP
  - `CONNECT` in L19 for cloud IP or URL and port, e.g., `tcp/hoge.janaeast.cloudapp.azure.com:7447`
- Change the setting in `platformio.ini` if needed
- Build and Upload to the board
- Start the device to communicate with the cloud!

What happens??
No problem,,, I cannot understand that :D
