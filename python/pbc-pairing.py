"""
Author: nexisato
Date: 2024-03-05 10:23:54
FilePath: /miracl_validation/python/pbc-pairing.py
Description: pbc库实现-基于双线性配对的密钥生成方案
"""

import pypbc
import utils
from typing import List, Tuple
import time

PAIRING_PARAMETERS = (
    "type a\n"
    "q 8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791\n"
    "h 12016012264891146079388821366740534204802954401251311822919615131047207289359704531102844802183906537786776\n"
    "r 730750818665451621361119245571504901405976559617\n"
    "exp2 159\n"
    "exp1 107\n"
    "sign1 1\n"
    "sign0 1\n"
)


class Payloadpair:
    def __init__(self):
        self.msg: str = None
        self.pid: str = None
        self.sig1: pypbc.Element = None  # G1
        self.sig2: pypbc.Element = None  # Zr
        self.pk1: pypbc.Element = None  # G1
        self.pk2: pypbc.Element = None  # G1


class KGCpair:
    """Key Generation Center Constructor (Pairing Version)"""

    def __init__(self, param_str: str = PAIRING_PARAMETERS):
        self.params = pypbc.Parameters(param_str)
        self.pairing = pypbc.Pairing(self.params)
        self.secret_key = pypbc.Element.random(self.pairing, pypbc.Zr)

        P = pypbc.Element.random(self.pairing, pypbc.G1)
        P0 = P * self.secret_key
        # Public Key: (P, P0)
        self.public_key: Tuple[pypbc.Element, pypbc.Element] = (P, P0)

    def generate_partial_key(self, pid_val: str) -> pypbc.Element:
        Qi = pypbc.Element.from_hash(self.pairing, pypbc.G1, pid_val.encode("utf-8"))
        Di = Qi * self.secret_key
        return Di


class Entitypair:
    """Communication Entity Constructor (Pairing Version)"""

    def __init__(self, pid_val: str, kgc: KGCpair):
        if pid_val is None:
            raise ValueError("pid_val can not be None")
        self.pid = pid_val
        self.kgc = kgc

        self.secret_value = pypbc.Element.random(self.kgc.pairing, pypbc.Zr)
        self.secret_key: pypbc.Element = None
        self.public_key: Tuple[pypbc.Element, pypbc.Element] = None

    def generate_full_key(self):
        partial_key = self.kgc.generate_partial_key(self.pid)  # G1
        self.secret_key = partial_key * self.secret_value

        pk1 = self.kgc.public_key[0] * self.secret_value
        pk2 = self.kgc.public_key[1] * self.secret_value
        self.public_key = (pk1, pk2)

    def sign(self, msg: str) -> Payloadpair:
        payload = Payloadpair()
        payload.msg = msg

        ri = pypbc.Element.random(self.kgc.pairing, pypbc.Zr)
        ri_P = self.kgc.public_key[0] ** ri  # G1

        T = self.kgc.pairing.apply(self.kgc.public_key[0], ri_P)  # GT

        # print("T: ", T)

        v_input: str = utils.int2hex(utils.hash256Function2int(msg + str(T)))
        v = pypbc.Element.from_hash(self.kgc.pairing, pypbc.Zr, v_input.encode("utf-8"))

        U = self.secret_key * v + ri_P
        payload.pid = self.pid
        payload.sig1 = U
        payload.sig2 = v
        payload.pk1 = self.public_key[0]
        payload.pk2 = self.public_key[1]

        # print("sig1: ", U)
        # print("sig2: ", v)

        return payload

    def verify(self, payload: Payloadpair) -> bool:
        lhs = self.kgc.pairing.apply(payload.pk1, self.kgc.public_key[1])
        rhs = self.kgc.pairing.apply(payload.pk2, self.kgc.public_key[0])

        if lhs != rhs:
            print("[ERROR]sender public key is invalid")
            return False

        pid_hash = pypbc.Element.from_hash(
            self.kgc.pairing, pypbc.G1, payload.pid.encode("utf-8")
        )
        neg_pk_second = -payload.pk2 * payload.sig2

        pair1 = self.kgc.pairing.apply(payload.sig1, self.kgc.public_key[0])
        pair2 = self.kgc.pairing.apply(pid_hash, neg_pk_second)
        T = pair1 * pair2  # GT * GT

        msg_input: str = utils.int2hex(utils.hash256Function2int(msg + str(T)))
        msg_hash = pypbc.Element.from_hash(
            self.kgc.pairing, pypbc.Zr, msg_input.encode("utf-8")
        )

        if msg_hash != payload.sig2:
            print("[ERROR]signature is invalid")
            return False

        print("[SUCCESS] 验证通过")
        return True


if __name__ == "__main__":
    kgc = KGCpair()
    print(kgc.params)

    print(kgc.pairing.order())
    print("s: ", kgc.secret_key)
    print("P: ", kgc.public_key[0])
    print("P0: ", kgc.public_key[1])

    pid1: str = "197b4c349c9e1a426383f09ba576249cb"
    entity1 = Entitypair(pid1, kgc)
    entity1.generate_full_key()
    # print("Entity1 Secret Key: ", entity1.secret_key)
    # print("Entity1 Public Key: ", entity1.public_key)

    msg: str = "hello world"
    #################################
    sign_start = time.perf_counter()

    payload1 = entity1.sign(msg)

    sign_end = time.perf_counter()
    print(
        f"[SIGN] pairing sign time cost: {utils.get_duration(sign_start, sign_end)} ms"
    )
    #################################

    # print("Payload1-sig: ", payload1.sig1, payload1.sig2)
    # print("Payload-pk1: ", payload1.pk1)
    # print("Payload-pk2: ", payload1.pk2)

    pid2: str = "5158ee15ec9beeae6cfcb3c5728e4313"
    entity2 = Entitypair(pid2, kgc)
    entity2.generate_full_key()

    #################################
    verify_start = time.perf_counter()

    entity2.verify(payload1)

    verify_end = time.perf_counter()
    print(
        f"[Verify] pairing verify time cost: {utils.get_duration(verify_start, verify_end)} ms"
    )
