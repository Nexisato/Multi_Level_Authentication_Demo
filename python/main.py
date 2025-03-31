"""
Author: nexisato
Date: 2024-03-04 11:34:04
FilePath: /py_validation/main.py
Description:

"""

from kgc import KGC
from entity import Entity, Payload
import utils
from fastecdsa import curve, keys, point
import gmpy2
import time
import secrets


acc_cur: str = "0x529b91b217ad52c99489ee4a3ea3c8687ea6ac84e3c2361b365936ba9c51d8686756cabe4f3271a330265ead3c6a8a14369eef709c0d8b453f8233e81cd066"
N_str: str = "0x20dcf162a31f29d02c211f16718b5576249272d77cf0de8f9f8751d8ee8e0887a3a109800174ee1dcb3a6da7447a2a1c01db86db98e80c8eb567eefdc08d55f"
pid1: str = "197b4c349c9e1a426383f09ba576249cb"
wit1: str = "0xf799337dc827f9aa6e4fb3c082adf5f5d177fc7f727b960d50d25228e7aac00f524dbfef3f08784cc3293e12dead55a2a0899fbdf9e37b8404dd920f8e1bd5"
pid2: str = "5158ee15ec9beeae6cfcb3c5728e4313"


def test_acc():
    rhs = gmpy2.powmod(utils.hex2int(wit1), utils.hex2int(pid1), utils.hex2int(N_str))
    return rhs == utils.hex2int(acc_cur)


if __name__ == "__main__":
    # print("acc: ", test_acc())

    # 1. Initialize KGC
    kgc = KGC()

    print("s: ", kgc.s)
    print("Ppub: ", kgc.Ppub)
    print("P: ", kgc.P)

    print("s * P: ", kgc.s * kgc.P)
    print("s * P == Ppub?: ", kgc.s * kgc.P == kgc.Ppub)

    print("----------------- [2] Generate Partial Key -----------------")

    # 2. Generate partial key

    entity1 = Entity(pid1, curve.secp256k1, kgc.Ppub)

    #######

    entity2 = Entity(pid2, curve.secp256k1, kgc.Ppub)

    partial_key1 = kgc.generate_partial_key(pid1)
    print("r: ", partial_key1[0])
    print("R: ", partial_key1[1])

    print("----------------- [3] Generate Full Key -----------------")

    # 3. Generate full key
    flag: bool = entity1.generate_full_key(partial_key1[0], partial_key1[1])
    if flag:
        print("secret_key: ", entity1.secret_key)
        print("public_key: ", entity1.public_key)

    # 4. Sign
    threshold: int = 1000
    sign_start = time.perf_counter()
    print("----------------- [4] Entity1 sign the msg -----------------")
    for _ in range(threshold):
        msg1: str = secrets.token_hex(32)
        payload1 = entity1.sign(msg1, utils.hex2int(wit1), utils.hex2int(N_str))
    # print("raw_wit1: ", utils.hex2int(wit1))
    # print("payload1: ", payload1.wit_new)
    # print("payload1.ti: ", payload1.time_stamp)
    sign_end = time.perf_counter()

    print(f"[SignTime]: {utils.get_duration(sign_start, sign_end) / threshold} ms")

    # 5. Verify
    print("----------------- [5] Entity2 verify the msg -----------------")
    verify_start = time.perf_counter()
    for _ in range(threshold):
        entity2.verify(payload1, utils.hex2int(acc_cur), utils.hex2int(N_str))
    verify_end = time.perf_counter()
    print(
        f"[VerifyTime]: {utils.get_duration(verify_start, verify_end) / threshold} ms"
    )
