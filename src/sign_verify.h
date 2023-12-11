#ifndef __SIGN_VERIFY_H__
#define __SIGN_VERIFY_H__

/*
    ARM MCU端签名验证模块
    郭强(guoqiang@huaqin.com)
    2023-11-24 创建
    注意！！！
        本模块“魔改”了malloc/free体系，使得签名验证过程简化存储分配活动
        所以本函数是不可重入的
        *** 系统的其它部分也不得使用malloc/free族函数***

    2023-12-09 签名方案修改为更先进的PSS

 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define SHA512_RESULT_LEN 64
#define RSA2048_RESULT_LEN 256

/*
    用途： 签名验证。
    说明： 签名端需要对数据签名，签名用私钥， 规格是2048的RSA算法，采用PKSC1 PSS方案填充，摘要则使用SHA512
    参数rsa_public_key_pem_string，pem格式的公钥字串
    参数origin_data/origin_size，被签名对象以及长度
    参数sign_data， 签名本身
    返回值： true表示签名通过，反之反是
 */
bool sign_verify_sha512_rsa2048_pkcs1_pss_padding(
    const uint8_t *rsa_public_key_pem_string,
    const uint8_t *origin_data,
    size_t origin_size,
    const uint8_t sign_data[RSA2048_RESULT_LEN]
);

#endif

