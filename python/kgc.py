"""
Author: nexisato
Date: 2024-03-04 11:34:30
FilePath: /py_validation/kgc.py
Description: kgc 封装类

"""

from fastecdsa import curve, keys, point
import secrets
import utils
from typing import Tuple


class KGC:
    """KGC Constructor

    ---Parameters---
    ec_curve: elliptic curve to be used for the key generation
    """

    def __init__(self, ec_curve=curve.secp256k1) -> None:
        if ec_curve is None:
            raise ValueError("Curve not set")
        self.ec_curve = ec_curve
        self.s = keys.gen_private_key(self.ec_curve)
        self.Ppub = keys.get_public_key(self.s, self.ec_curve)
        self.P = self.ec_curve.G
        self.q = self.ec_curve.q

    def generate_partial_key(self, pid_val: str) -> Tuple[int, point.Point]:
        """generate partial secret key according to the pid_val"""
        r: int = secrets.randbelow(self.q)
        R: point.Point = r * self.P

        h1_input: str = pid_val + utils.point2hex(R) + utils.point2hex(self.Ppub)
        h1: int = utils.hash256Function2int(h1_input)

        d: int = self.s * h1 + r
        return d, R
