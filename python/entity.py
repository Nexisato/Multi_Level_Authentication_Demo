"""
Author: nexisato
Date: 2024-03-04 14:30:32
FilePath: /py_validation/entity.py
Description: Entiy 封装类

"""

from fastecdsa import curve, keys, point
import secrets
import utils
from typing import Tuple
import gmpy2


class Payload:
    def __init__(self):
        self.pid: str = None
        self.msg: str = None
        # sig: (Y1, w)
        self.sig1: point.Point = None
        self.sig2: int = None
        # public key: (X, R)
        self.pk1: point.Point = None
        self.pk2: point.Point = None
        self.time_stamp: str = None
        self.wit_new: int = None


class Entity:
    """Entity Constructor"""

    def __init__(self, pid_val: str = None, ec_curve=curve.secp256k1, ec_Ppub=None):
        self.ec_curve = ec_curve
        self.ec_Ppub = ec_Ppub
        self.ec_q = ec_curve.q
        self.ec_P = ec_curve.G
        self.pid = pid_val

        self.secret_key: Tuple[int, int] = None
        self.public_key: Tuple[point.Point, point.Point] = None

    def generate_full_key(self, d: int, R: point.Point) -> None:
        """generate full secret key according to the d and R"""
        h1_input: str = self.pid + utils.point2hex(R) + utils.point2hex(self.ec_Ppub)
        h1: int = utils.hash256Function2int(h1_input)

        lhs: point.Point = d * self.ec_P
        rhs: point.Point = R + h1 * self.ec_Ppub

        if lhs != rhs:
            raise ValueError("lhs != rhs")

        x: int = secrets.randbelow(self.ec_q)
        X: point.Point = x * self.ec_P

        self.secret_key = (d, x)
        self.public_key = (X, R)

    def sign(self, msg: str, wit: int, N: int) -> Payload:
        """sign the message"""
        ti: str = utils.get_time_stamp()
        ht: int = utils.hash256Function2int(ti)

        wit_new: int = gmpy2.powmod(wit, ht, N)

        y1: int = secrets.randbelow(self.ec_q)
        Y1: point.Point = y1 * self.ec_P

        h3_input: str = (
            msg
            + utils.int2hex(wit_new)
            + self.pid
            + utils.point2hex(self.public_key[0])
            + utils.point2hex(self.public_key[1])
            + ti
        )
        u_input: str = h3_input + utils.point2hex(Y1)

        u: int = utils.hash256Function2int(u_input)
        h3: int = utils.hash256Function2int(h3_input)

        w: int = (u * y1 + h3 * (self.secret_key[0] + self.secret_key[1])) % self.ec_q

        payload = Payload()
        payload.pid = self.pid
        payload.msg = msg
        payload.sig1 = Y1
        payload.sig2 = w
        payload.pk1 = self.public_key[0]
        payload.pk2 = self.public_key[1]
        payload.time_stamp = ti
        payload.wit_new = wit_new

        return payload

    def verify(self, payload: Payload, acc_cur: int, N: int) -> bool:
        ti: str = payload.time_stamp
        ht: int = utils.hash256Function2int(ti)

        h1_input: str = (
            payload.pid + utils.point2hex(payload.pk2) + utils.point2hex(self.ec_Ppub)
        )
        h3_input: str = (
            payload.msg
            + utils.int2hex(payload.wit_new)
            + payload.pid
            + utils.point2hex(payload.pk1)
            + utils.point2hex(payload.pk2)
            + ti
        )
        u_input: str = h3_input + utils.point2hex(payload.sig1)

        h1: int = utils.hash256Function2int(h1_input)
        h3: int = utils.hash256Function2int(h3_input)
        u: int = utils.hash256Function2int(u_input)

        lhs: point.Point = payload.sig2 * self.ec_P - u * payload.sig1
        rhs: point.Point = h3 * (h1 * self.ec_Ppub + payload.pk1 + payload.pk2)

        if lhs != rhs:
            print("Payload Public Key Invalid")
            return False

        lhs_last = gmpy2.powmod(payload.wit_new, utils.hex2int(payload.pid), N)
        rhs_last = gmpy2.powmod(acc_cur, ht, N)

        if lhs_last != rhs_last:
            print("lhs_last: ", lhs_last)
            print("rhs_last: ", rhs_last)
            print("Payload Witness Invalid")
            return False

        # print("Verify Success")
        return True
