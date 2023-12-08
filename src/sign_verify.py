#!/bin/python3

import misc

from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.serialization import load_pem_private_key, load_pem_public_key
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding



def sha512(data):
    digest = hashes.Hash(hashes.SHA512())
    digest.update(data)
    return digest.finalize()


def padding_arg():
    arg = padding.PSS(mgf=padding.MGF1(hashes.SHA512()),
                      salt_length=padding.PSS.MAX_LENGTH)
    return arg


def hash_arg():
    return hashes.SHA512()


def sign_by_sha512_rsa2048_pkcs1_v21_padding(origin_data, rsa_private_key_pem_filename):
    pri_pem = misc.read_file_into_bytes(rsa_private_key_pem_filename)
    pri_key = load_pem_private_key(pri_pem, password=None)
    if not isinstance(pri_key, rsa.RSAPrivateKey) or pri_key.key_size != 2048:
        raise RuntimeError("private key is not a RSA2048 key")

    signed_data = pri_key.sign(origin_data, padding_arg(), hash_arg())
    return signed_data


def verify_sign_by_sha512_rsa2048_pkcs1_v21_padding(signed_data, origin_data, rsa_public_key_pem_filename):
    pub_pem = misc.read_file_into_bytes(rsa_public_key_pem_filename)
    pub_key = load_pem_public_key(pub_pem)
    if not isinstance(pub_key, rsa.RSAPublicKey) or pub_key.key_size != 2048:
        raise RuntimeError("public key is not a RSA2048 key")

    pub_key.verify(signed_data, origin_data, padding_arg(), hash_arg())

if __name__ == '__main__':
    origin_data = b'1234'
    signed_data = sign_by_sha512_rsa2048_pkcs1_v21_padding(
        origin_data, 'financial.pem')
    verify_sign_by_sha512_rsa2048_pkcs1_v21_padding(
        signed_data, origin_data, 'financial_pub.pem')
    misc.print_c_array(signed_data)
