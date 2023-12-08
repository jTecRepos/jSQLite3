#include "jLowLevel.h"


//TODO check on what platform memccpy is actually missing


/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
	Return the position in DEST one byte past where C was copied, or
	NULL if C was not found in the first N bytes of SRC.
	https://en.cppreference.com/w/c/string/byte/memccpy
*/
void * memccpy (void *dest, const void *src, int c, size_t n)
{
	/*for(size_t i = 0; i < n; i++)
	{
		*dest++
	}*/
	// https://opensource.apple.com/source/Libc/Libc-167/string.subproj/memccpy.c.auto.html
	if (n) {
		unsigned char *tp = (unsigned char*) dest;
		const unsigned char *fp = (unsigned char*) src;
		do {
			if ((*tp++ = *fp++) == c)
				return (tp);
		} while (--n != 0);
	}
	return (0);
	
	// https://elixir.bootlin.com/glibc/latest/source/string/string.h#L381
	/*
	void *p = memchr (src, c, n);

	if (p != NULL)
		return __mempcpy (dest, src, p - src + 1);

	memcpy (dest, src, n);
	return NULL;
	*/
}