#include "sign_verify.h"

/*
    ARM MCU端签名验证模块
    郭强(guoqiang@huaqin.com)
    2023-11-24 创建
    注意！！！
        本模块“魔改”了malloc/free体系，使得签名验证过程简化存储分配活动
        所以本函数是不可重入的
        *** 系统的其它部分也不得使用malloc/free族函数***

 */

#include <string.h>

#include "mbedtls/sha512.h"
#include "mbedtls/pk.h"

// 重置malloc/free族函数。请谨慎使用
void reset_malloc(void);

#define CHECK_RESULT \
    if (ret) {\
        break; \
    } else

/*
    用途： 签名验证。
    说明： 签名端需要对数据签名，签名用私钥， 规格是2048的RSA算法，采用PKSC1方案填充，摘要则使用SHA512
    参数rsa_public_key_pem_string，pem格式的公钥字串
    参数origin_data/origin_size，被签名对象以及长度
    参数sign_data， 签名本身
    返回值： true表示签名通过，反之反是
 */
bool sign_verify_sha512_rsa2048_pkcs1_padding(
    const uint8_t *rsa_public_key_pem_string,
    const uint8_t *origin_data,
    size_t origin_size,
    const uint8_t sign_data[RSA2048_RESULT_LEN]
)
{
    reset_malloc(); // 专用内存清除
    int ret;

    mbedtls_pk_context ctx_pk; // 公钥
    mbedtls_rsa_context *rsa;  // rsa公钥，指向ctx_pk
    uint8_t sha512_value[SHA512_RESULT_LEN] = {0};       // 存放sha512
    uint8_t rsa2048_decrypted[RSA2048_RESULT_LEN] = {0}; // rsa解码存放的数据

    do {
        mbedtls_pk_init(&ctx_pk); // 初始化之
        ret = mbedtls_pk_parse_public_key(&ctx_pk, rsa_public_key_pem_string, strlen((const char *)rsa_public_key_pem_string) + 1); // 从pem中获得公钥
        CHECK_RESULT;

        rsa = (mbedtls_rsa_context*)ctx_pk.pk_ctx; // 取得rsa公钥
        mbedtls_rsa_set_padding(rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA512); // 基本初始化，可以不理
        ret = mbedtls_rsa_check_pubkey(rsa); // 检查rsa公钥是否正常（基本上）
        CHECK_RESULT;

        ret = mbedtls_rsa_public(rsa, sign_data, rsa2048_decrypted);  // 最复杂的部分：用RSA公钥还原签名对应的数据
        CHECK_RESULT;

        ret = mbedtls_sha512_ret(origin_data, origin_size, sha512_value, false); // 自己计算一下hash
        CHECK_RESULT;

        ret = memcmp(rsa2048_decrypted + RSA2048_RESULT_LEN - SHA512_RESULT_LEN, sha512_value, SHA512_RESULT_LEN); // 对比hash。解码数据前面是填充，后面是数据
        CHECK_RESULT;
    } while (false); // do while(0) 结构是标准套路

    reset_malloc(); // 专用内存清除
    return ret == 0; // 全部通过才算验签完成
}

// 设定了一个8K的内存区域供malloc/free使用。
// 策略极其简单：忽略释放函数；申请函数线性增长；用完了后集体释放
// 这是个“魔改”方案，请慎重使用

// 内存块和已使用大小
#define OWN_MEM_BLOCK_SIZE 8192
static uint8_t mem_block[OWN_MEM_BLOCK_SIZE];
static size_t mem_top = 0;

// 覆盖了标准malloc
void *malloc(size_t n)
{
    // 对n搞8字节对齐，并累计到申请总大小
    size_t new_top = mem_top + ((n - 1) / 8 + 1)* 8;
    // 超出则不分配
    if (new_top > OWN_MEM_BLOCK_SIZE) {
        return 0;
    }
    // 没超，返回前一次的头。
    void *result = mem_block + mem_top;
    mem_top = new_top;
    return result;
}

// 啥也不干
void free(void *p)
{
}

// 总清除函数
void reset_malloc(void)
{
    mem_top = 0;
}
