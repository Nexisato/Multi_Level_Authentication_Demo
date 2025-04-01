# MLAuth: Multi-Level Identity Authentication Framework for Resource-Constrained IoT Environment


<div align="center">

[![][go-version-shield]][go-readme-link] &nbsp; [![][java-version-shield]][java-readme-link] &nbsp; [![][python-version-shield]][python-readme-link] &nbsp; [![][cpp-version-shield]][cpp-readme-link]

[![][SEU-shield]][SEU-link] &nbsp; [![][github-stars-shield]][github-stars-link] &nbsp; [![][github-contributors-shield]][github-contributors-link] &nbsp; [![][license-shield]][license-link]

[SEU-shield]: https://img.shields.io/badge/Southeast%20University%20-50755B
[SEU-link]: https://www.seu.edu.cn/english/main.htm
[github-stars-shield]: https://img.shields.io/github/stars/Nexisato/Multi_Level_Authentication_Demo.svg?style=flat-square&label=✨%20Stars&color=yellow
[github-stars-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/stargazers
[github-contributors-shield]: https://img.shields.io/github/contributors/Nexisato/Multi_Level_Authentication_Demo.svg?style=flat-square&label=👥%20Contributors&color=blue
[github-contributors-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/graphs/contributors
[license-shield]: https://img.shields.io/github/license/Nexisato/Multi_Level_Authentication_Demo.svg?style=flat-square&color=green
[license-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/blob/main/LICENSE

[go-version-shield]: https://img.shields.io/badge/Go-Pipeline-00ADD8?style=flat-square&logo=go&logoColor=white
[go-readme-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/tree/main/go/README.md

[java-version-shield]: https://img.shields.io/badge/Java-Pipeline-ED8B00?style=flat-square&logo=openjdk&logoColor=white
[java-readme-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/tree/main/java/README.md

[python-version-shield]: https://img.shields.io/badge/Python-Pipeline-3776AB?style=flat-square&logo=python&logoColor=white
[python-readme-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/tree/main/python/README.md

[cpp-version-shield]: https://img.shields.io/badge/C++-Pipeline-00599C?style=flat-square&logo=cplusplus&logoColor=white
[cpp-readme-link]: https://github.com/Nexisato/Multi_Level_Authentication_Demo/tree/main/README.md

</div>


## Function
1. Elliptic Curve Cryptography provides efficient authentication and key generation mechanisms. By leveraging the mathematical properties of elliptic curves, the system achieves high security with shorter key lengths compared to traditional cryptographic solutions, resulting in faster operations and reduced computational overhead.
2. Bilinear Pairings enable sophisticated functionality such as trusted delegation and fine-grained access control. These mathematical mappings allow for advanced cryptographic protocols that support complex trust relationships, attribute-based encryption schemes, and context-sensitive authorization that can adapt to varying security requirements.
3. RSA Accumulators deliver membership proofs and revocable anonymous credentials. This component allows the system to efficiently verify whether an element belongs to a set without revealing the entire set, supporting privacy-preserving identity management with the ability to selectively revoke credentials when necessary



## Requirements

### Operating System
- Linux: Ubuntu 20.04~24.04, WSL2 (Ubuntu 20.04+)

### Development Environment
- C++17 compatible compiler (GCC 9+ or Clang 10+)
- Go 1.18+
- CMake 3.16+

### Dependencies
- **Core Libraries**:
  - PBC Library v0.5.14+ (for pairing-based cryptography)
  - OpenSSL v3.X (v3.2.0 recommended, v1.1.1 also works)
  - GMP/GMPXX v6.2.0+ (GNU Multiple Precision Arithmetic Library)
  - jsoncpp v1.9.0+ (JSON data exchange format)
  
- **For Go Implementation**:
  - PBC Go Wrapper: github.com/Nik-U/pbc

- **For UI Components**:
  - Qt v5.14.2+
  - linuxdeployqt (for packaging)

 - **For Perf Utils**:
    - gprof (Define in CMakeLists.txt)
    - Valgrind

### Optional
- Docker (for containerized deployment)
- Git (for version control)

> Electron may be supported in future versions
> OpenMP is not required as parallel computation is not implemented


## Usages
- `OpenSSL` need to be compiled from source: https://github.com/openssl/openssl/releases
V3.0.x version recommended. (1.1.1x version also worked)
- `PBC Library` also need to be compiled from source:  https://crypto.stanford.edu/pbc/

```bash
# Define the KGC and Entity params in src/KGC*.cpp and src/Process*.cpp respectively.
mkdir build
cd build
cmake ..
make -j

# Verify the process in you local logs and terminal
```


### Install Dev Lib
```bash
sudo apt install build-essential
sudo apt install libgmp-dev libjsoncpp-dev

#for OpenGL-supported
sudo apt-get install libgl1-mesa-dev
```

### Docker Images
```bash
docker pull registry.cn-hangzhou.aliyuncs.com/nexisato/ubuntu_auth_demo:latest
```


## Logs
- 2023-12-06: build new version of Docker Image. Use `1024bit` RSA Prime instead of `Safe Prime`.
- 2023-11-25: Update WSL2 dev to Ubuntu-22.04.
- 2023-11-17: Tidy Project structure.
- 2023-11-17: CLS-Pair-free method validated (except for aggregate sig&ver)
- 2023-11-03: Refactor PBC with Go Wrapper.version(Serialize Supported): https://github.com/Nik-U/pbc
- 2023-10-30: PBC Wrapper aborted (NO DOCS, FUCKING USELESS)
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




