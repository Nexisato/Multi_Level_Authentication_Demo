"""
Author: nexisato
Date: 2024-03-04 20:43:57
FilePath: /miracl_validation/python/benchmark.py
Description:

"""

import utils
import time
from fastecdsa import point, curve, keys
import gmpy2
import secrets


@utils.time_perf
def test_point_add(a: point.Point, b: point.Point, threshold: int):
    for _ in range(threshold):
        c = a + b


@utils.time_perf
def test_point_mul(a: point.Point, q: int, threshold: int):
    for _ in range(threshold):
        b = secrets.randbelow(q)
        _ = b * a


@utils.time_perf
def test_pow_mod(bit_num: int, threshold: int):
    for _ in range(threshold):
        a = secrets.randbits(bit_num)
        b = secrets.randbits(bit_num)
        c = secrets.randbits(bit_num * 2)
        c = gmpy2.powmod(a, b, c)


@utils.time_perf
def test_hash2int(threshold: int):
    for _ in range(threshold):
        input_str: str = secrets.token_hex(256)
        _ = utils.hash256Function2int(input_str)


if __name__ == "__main__":
    ec_curve = curve.secp256k1
    ec_P = ec_curve.G
    ec_secret = keys.gen_private_key(ec_curve)
    ec_Ppub = keys.get_public_key(ec_secret, ec_curve)
    ec_q = ec_curve.q

    test_point_add(ec_P, ec_P, threshold=1000)
    test_point_mul(ec_P, ec_q, threshold=1000)

    test_pow_mod(256, threshold=1000)
    test_hash2int(threshold=1000)
