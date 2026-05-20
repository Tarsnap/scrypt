#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "crypto_scrypt.h"

static int
check_invalid_params(uint64_t N, uint32_t r, uint32_t p)
{
	uint8_t kbuf[16];

	errno = 0;
	if (crypto_scrypt((const uint8_t *)"passwd", 6,
	    (const uint8_t *)"salt", 4, N, r, p, kbuf, sizeof(kbuf)) != -1) {
		printf("crypto_scrypt(%llu, %u, %u) succeeded\n",
		    (unsigned long long)N, (unsigned int)r, (unsigned int)p);
		return (1);
	}
	if (errno != EINVAL) {
		printf("crypto_scrypt(%llu, %u, %u) failed with errno %d\n",
		    (unsigned long long)N, (unsigned int)r, (unsigned int)p,
		    errno);
		return (1);
	}

	return (0);
}

int
main(void)
{
	int failures = 0;

	failures += check_invalid_params(16, 0, 1);
	failures += check_invalid_params(16, 1, 0);
	failures += check_invalid_params(1, 1, 1);

	return (failures ? 1 : 0);
}
