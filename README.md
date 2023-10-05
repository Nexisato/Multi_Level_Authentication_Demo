# Multi-Level Authentication Demo
---

## Denpendencies
- gmp && gmpxx
- openssl
- jsoncpp
- pbc

## Logs
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

## Environment
- **OS**：Ubuntu 20.04 （Windows Subsystem for Linux 2）
- **Language**：C++, Rust, Golang, Python
- **Dependencies**：
  - pbc library & PBC-Wrapper: 无证书通信构造
    - https://crysp.uwaterloo.ca/software/PBCWrapper/
    - https://jeza-chen.com/2020/06/05/PBC-Cpp-Wrapper/ 
  - OpenSSL 3.0.10（C++）
  - gmpy2 (Python)
  - PyPBC Python）
  - Accumulator-rs(Rust): [[Repo]](https://github.com/mikelodder7/accumulator-rs)
  - libsm(rust): [[Repo]](https://github.com/citahub/libsm) 
  - 蚂蚁金服铜锁密码库：
    - https://www.yuque.com/tsdoc/ts

## UI
- Electron 模板：https://github.com/dromara/electron-egg
- 前端模板：https://github.com/773579084/leno-admin-electron
- Qt in CLion: https://zhuanlan.zhihu.com/p/461896034



## References
- [【Original Share Space】](https://rjwalmzfj2.feishu.cn/drive/folder/fldcnpvD9hTdMQ1ZXHU9ufPumTf?from=space_shared_folder&fromShareWithMeNew=1)
    - [【Design Docs】](https://uqc1fm1ybzl.feishu.cn/docx/TveddkLhtoxDByxmqmBcS9fsnge)
    - [【Demand Analysis】](https://rjwalmzfj2.feishu.cn/docx/Mp6XdDhTPoZhclxc2Yvc4kRZn9i)

- TREMEL E. Real-World Performance of Cryptographic Accumulators.


## Acknowledgements
- IDEA-ReadPaper Project Group
- 火山引擎




