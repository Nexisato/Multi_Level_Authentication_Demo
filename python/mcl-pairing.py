"""
Author: nexisato
Date: 2024-03-10 15:33:07
FilePath: /miracl_validation/python/mcl-pairing.py
Description: mcl库实现-基于双线性配对的密钥生成方案

"""

import pymcl
import utils
from typing import List, Tuple
import time


class KGCPair:
    def __init__(self) -> None:
        self.secret_key = pymcl.Fr.random()

        P = pymcl.g1
        P0 = P * self.secret_key
        # Public Key: (P, P0) G1
        self.public_key: Tuple[pymcl.G1, pymcl.G1] = (P, P0)

    def generate_partial_key(self, pid_val: str) -> pymcl.G1:
        Qi = pymcl.G1.hash(pid_val.encode("utf-8"))
        Di = Qi * self.secret_key
        return Di


class EntityPair:
    """Communication Entity Constructor (Pairing Version)"""

    def __init__(self, pid_val: str, kgc: KGCPair):
        if pid_val is None:
            raise ValueError("pid_val can not be None")
        self.pid = pid_val
        self.kgc = kgc

        self.secret_value = pymcl.Fr.random()
        self.secret_key = self.kgc.generate_partial_key(pid_val)
        self.public_key = None

    def generate_full_key(self):
        partial_key = self.kgc.generate_partial_key(self.pid)  # G1
        self.secret_key = partial_key * self.secret_value


if __name__ == "__main__":
    kgc = KGCPair()
    print("Public Key: ", kgc.public_key)

    pid1: str = "197b4c349c9e1a426383f09ba576249cb"
    entity1 = EntityPair(pid1, kgc)

    D = kgc.generate_partial_key(pid_val=pid1)
    print("partial key: ", D)
