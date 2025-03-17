# Installing Flash Attention (`flash-attn`) in Docker

*Posted: March 16, 2025*


Over this past week, I ran into a bunch of dead ends and open issues trying to install `flash-attn` in a Docker container. I thought I'd make a post about the solution here in case anyone else runs into the same problem!


## Issues

When trying to install `flash-attn` even outside of Docker, I found it was a huge pain. The main reasons for this are that `flash-attn` is a relatively older project with support seeming for CUDA 12.1 only. My PC's drivers are for CUDA 12.4, and installing flash-attn using pip will cause an `unkown symbol` error during the compilation steps for one of its submodules. To get around this, you have to build the entire package from source, and update or install CUDA version 12.1.



Issues with Docker arise as well, as in order to get this working you need to also build the package from source. This is a pain, since when installing PyTorch I noticed that the CUDA architecture is not specified causing a list of things to break or fail. Additionally, submodules require specific versions of other packages or system libraries, and it's a pain to figure out what you need to install.


## Solution

Here's a Dockerfile that should work for installing `flash-attn` in a Docker container. I've tested it on an NVIDIA a6000 running Ubuntu 22.04 and it works perfectly. Deploying this image to a cloud service like AWS or GCP has also been tested and works as well. 


```docker
FROM nvidia/cuda:12.1.1-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && \
    apt-get install -y software-properties-common build-essential ninja-build git clang

# install pip
RUN apt-get install -y python3-pip


# Install PyTorch and Flash Attention dependencies
RUN python3 -m pip install wheel packaging ninja setuptools

# Install PyTorch for the Ampere architecture, change based on your GPU
ENV TORCH_CUDA_ARCH_LIST="8.0;8.6"

# Install PyTorch with CUDA support
RUN python3 -m pip install torch==2.3.0 torchvision==0.18.0 torchaudio==2.3.0 --index-url https://download.pytorch.org/whl/cu121

ENV CUDA_HOME=/usr/local/cuda

# Install Flash Attention components
RUN python3 -m pip install --no-cache-dir \
    git+https://github.com/HazyResearch/flash-attention.git#subdirectory=csrc/layer_norm \
    git+https://github.com/HazyResearch/flash-attention.git#subdirectory=csrc/rotary \
    git+https://github.com/HazyResearch/flash-attention.git#subdirectory=csrc/fused_dense_lib \
    git+https://github.com/HazyResearch/flash-attention.git#subdirectory=csrc/xentropy

# Install additional Flash Attention dependencies
RUN python3 -m pip install --no-cache-dir flash-attn --no-build-isolation

```

## Things to note:

- **CUDA Architecture**: The `ENV TORCH_CUDA_ARCH_LIST` variable specifies the CUDA architecture. You can change this based on your GPU.
- **CUDA Version**: The Dockerfile installs CUDA 12.1. You can change this based on your GPU.
- **PyTorch Version**: The Dockerfile installs PyTorch 2.3.0. You can change this based on your GPU.
- **Flash Attention Version**: The Dockerfile installs Flash Attention 2.4.1. You can change this based on your version of `flash-attn`.


Hopefully this saves you some time! Feel free to reach out to me on [X](https://x.com/jonathanzkoch) if you have any questions or feedback.

