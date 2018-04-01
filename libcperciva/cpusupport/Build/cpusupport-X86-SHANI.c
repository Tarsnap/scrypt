#include <immintrin.h>

static char a[16];

int
main(void)
{
	__m128i x, y;

	x = _mm_loadu_si128((__m128i *)a);
	y = _mm_sha256msg1_epu32(x, x);
	_mm_storeu_si128((__m128i *)a, y);
	return (a[0]);
}
