#ifndef _CRYPTO_SHA_SHANI_H_
#define _CRYPTO_SHA_SHANI_H_

#include <stdint.h>

/**
 * crypto_sha_hash_blocks_shani(state, block):
 * Hash one block using Intel SHA hardware acceleration. This implementation
 * uses x86 SHANI instructions, and should only be used if CPUSUPPORT_X86_SHANI
 * is defined and cpusupport_x86_shani() returns nonzero.
 */
void crypto_sha256_shani(uint32_t * state, const uint8_t * data);

#endif