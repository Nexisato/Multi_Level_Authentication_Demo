"""
Author: nexisato
Date: 2024-03-04 11:34:36
FilePath: /py_validation/utils.py
Description: 定义一些密码学操作中常用的工具类函数

"""

from fastecdsa import point
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
import time
import orjson
from typing import Any, List
from loguru import logger


def hash_sha256(input: str) -> str:
    """sha256 encoding sting to hex string"""
    data: bytes = input.encode("utf-8")
    digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
    digest.update(data)
    return digest.finalize().hex()


def hex2int(hex: str) -> int:
    """convert hex string to int"""
    return int(hex, 16)


def int2hex(val: int) -> str:
    """convert int to hex string"""
    return hex(val)


def hash256Function2int(input: str) -> int:
    """convert hash string to int"""
    hash_val: str = hash_sha256(input)
    return hex2int(hash_val)


def get_bits(n: int) -> int:
    """get the number of bits of a number"""
    return n.bit_length()


def point2hex(p: point.Point) -> str:
    """convert point to hex string"""
    return str(p.x) + str(p.y)


def get_time_stamp() -> str:
    """get the current time stamp
    ---
    use time.perf_counter() to record the time point
    """
    return str(int(time.time()))


def time_perf(func):
    """decorator to calculate the duration of a function"""

    def wrapper(*args, **kwargs):
        start_time = time.perf_counter()
        res = func(*args, **kwargs)
        end_time = time.perf_counter()
        duration = get_duration(start_time, end_time)
        print(f"{func.__name__} | execution: {duration} ms")
        if "threshold" in kwargs.keys():
            avg_duration = duration / kwargs["threshold"]
            print(f"{func.__name__} | average: {avg_duration} ms")
        print()
        return res

    return wrapper


def get_duration(start_time: float, end_time: float) -> float:
    """
    Calculate the duration between two time points in milliseconds.

    Parameters:
    start_time (float): The start time in seconds.
    end_time (float): The end time in seconds.

    Returns:
    float: The duration between start_time and end_time in seconds.
    """
    return (end_time - start_time) * 1000


#! hexidecimal might cause wrong answer
# def quick_pow_str(a: str, b: str, p: str):
#     """ quick power calculation
#     ---
#     a^b mod p
#     """
#     a_mpz = mpz(a, 10)
#     b_mpz = mpz(b, 10)
#     p_mpz = mpz(p, 10)
#     res = powmod(a_mpz, b_mpz, p_mpz)
#     return res

# def quick_pow_int(a: int, b: int, p: int) -> int:
#     """ quick power calculation
#     ---
#     a^b mod p
#     """
#     res = powmod(a, b, p)
#     return res


def load_from_json(file_path: str) -> List[Any]:
    """load pid list from json file"""

    with open(file_path, "rb") as f:
        data = orjson.loads(f.read())

    pids = []
    for item in data["packages"]:
        pids.append(item["pid"])
    return pids
