# Multi-Level Authentication Demo
## Environment
- **OS**：Ubuntu 20.04~22.04
- **Language**：C++; GO
- **Build**: CMake
- **Dependencies**：
  - PBC Library
  - PBC Go Wrapper
  - OpenSSL v3.X (3.2.0)
  - gmp & gmpxx
  - jsoncpp
- **UI**:
  - Qt v5.14.2
  - Packing: linuxdeployqt
> Conisdering Electron supported in the future.
> No need OpenMP for parallel computing

## Usages
- `OpenSSL` need to be compiled from source: https://github.com/openssl/openssl/releases
V3.0.x version recommended. (1.1.1x version also worked)
- `PBC Library` also need to be compiled from source:  https://crypto.stanford.edu/pbc/

### Install Dev Lib
```bash
sudo apt install build-essential
sudo apt install libgmp-dev libjsoncpp-dev

#for OpenGL-supported
sudo apt-get install libgl1-mesa-dev
```

### ~~Docker~~(deprecreated)
```bash
docker pull registry.cn-hangzhou.aliyuncs.com/nexisato/ubuntu_auth_demo:latest
```


## Logs
- 2023-11-25: Update WSL2 dev to Ubuntu-22.04.
- 2023-11-17: Tidy Project structure.
- 2023-11-17: CLS-Pair-free method validated (except for aggregate sig&ver)
- 2023-11-03: Refactor PBC with Go Wrappe.version(Serialize Supported): https://github.com/Nik-U/pbc
- 2023-10-30: PBC Wrapper aborted (NO DOCS, USELESS)
- 2023-10-25: fix CL-PKS bug & add PBC Wrapper Version
- 2023-10-25: Performance Optimized (just shut down omp)
- 2023-10-23: release portable CA application
- 2023-10-13: entityID NO NEED to be SAFE PRIME (less time cost!)
- 2023-10-11: Packed to Docker Image on Ubuntu 22.04
- 2023-10-05: Code Splitting Success.
- 2023-10-02: Accumulator Revocation Success
- 2023-09-28: RSA Accmulator Verification Success
  - Only rely on `jsoncpp` and `gmp`
- 2023-09-24: RSA Accumulator init
  - References: https://github.com/etremel/crypto-accumulators
  - Dependencies (DO NOT MAKE INSTALL, PUT HERE):
    - ~~flint-2.9.0: (local docs)~~
      - compile the docs manually
    - gmp-6.3.0: https://gmplib.org/
    - ~~mpfr-4.2.1: https://www.mpfr.org/~~
  - ~~cryptopp880~~ (INSTALLED in /usr/local)
  - jsoncpp (INSTALLED in /usr/local)
  - OpenSSL (INSTALLED in /usr/local/openssl): 
- 2023-09-23: finish batch compute of MD5 checksum stored in JSON
  - ignore all built target dirs
- 2023-09-17: finish CL_Auth_Demo with PBC: 
  - References: https://github.com/Superhank718/Java_CLS_jpbc



## References
- [【Original Share Space】](https://rjwalmzfj2.feishu.cn/drive/folder/fldcnpvD9hTdMQ1ZXHU9ufPumTf?from=space_shared_folder&fromShareWithMeNew=1)
    - [【Design Docs】](https://uqc1fm1ybzl.feishu.cn/docx/TveddkLhtoxDByxmqmBcS9fsnge)
    - [【Demand Analysis】](https://rjwalmzfj2.feishu.cn/docx/Mp6XdDhTPoZhclxc2Yvc4kRZn9i)

- https://github.com/v0y4g3r/rsa-accumulator-java
- https://jeza-chen.com/2020/06/04/PBC-Library/
- https://zhuanlan.zhihu.com/p/461896034


## Acknowledgements
- IDEA-ReadPaper Project Group
- 火山引擎




