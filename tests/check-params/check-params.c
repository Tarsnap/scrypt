#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include "warnp.h"

#include "crypto_scrypt.h"

/* How much data should scrypt return? */
#define OUTPUT_BUFLEN 8

static struct testcase {
	uint64_t N;
	uint32_t r;
	uint32_t p;
	int desired_errno;	/* 0 indicates no failure / errno is unset. */
} tests[] = {
	/* Should fail. */
	{ 0, 0, 0, EINVAL },
	{ 1, 1, 1, EINVAL },
	{ 2, 0, 1, EINVAL },
	{ 2, 1, 0, EINVAL },
	{ 3, 1, 1, EINVAL },
	{ 16384, 1, 2147483648, EFBIG },
	{ 16384, 2147483648, 1, EFBIG },
	{ 16384, 32768, 32768, EFBIG },
	{ 16384, 65536, 65537, EFBIG },
	{ (UINT64_C(1) << 62) + 1, 1, 1, EINVAL },

	/* Should succeed. */
	{ 2, 1, 1, 0 },
	{ 2, 3, 1, 0 },
	{ 2, 1, 3, 0 }
};

static int
check_params(uint64_t N, uint32_t r, uint32_t p, int desired_errno)
{
	const char * passwd = "hunter2";
	const char * salt = "DANGER -- this should be a random salt -- DANGER";
	uint8_t output[OUTPUT_BUFLEN];
	int rc;

	/* Call crypto_scrypt. */
	rc = crypto_scrypt((const uint8_t *)passwd, strlen(passwd),
	    (const uint8_t *)salt, strlen(salt), N, r, p,
	    output, OUTPUT_BUFLEN);

	if (desired_errno == 0) {
		if (rc != 0) {
			warn0("Should have succeeded with N=%" PRIu64
			    " r=%" PRIu32 " p=%" PRIu32, N, r, p);
			goto err0;
		}
	} else {
		if ((rc != -1) || (errno != desired_errno)) {
			warn0("Should have errno %i with N=%" PRIu64
			    " r=%" PRIu32 " p=%" PRIu32,
			    desired_errno, N, r, p);
			goto err0;
		}
	}

	/* Success! */
	return (0);

err0:
	/* Failure! */
	return (-1);
}

int
main(int argc, char * argv[])
{
	size_t i;

	WARNP_INIT;

	(void)argc; /* UNUSED */

	/* Run test cases. */
	for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
		if (check_params(tests[i].N, tests[i].r, tests[i].p,
		    tests[i].desired_errno))
			goto err0;
	}

	/* Success! */
	return (0);

err0:
	/* Failure! */
	return (1);
}
