#include "spi.h"

#include <stdio.h>

int spidummy_spi_init(void *pcookie)
{
	printf("%s(pcookie=%p)\n", __func__, pcookie);
	return -1;
}

int spidummy_spi_select(void *cookie, int _ss)
{
	printf("%s(cookie=%p, _ss=%i)\n", __func__, cookie, _ss);
	return -1;
}

int spidummy_spi_clock(void *cookie, int _ss, int in, int *out)
{
	printf("%s(cookie=%p, _ss=%i, in=%i, out=%p)\n", __func__, cookie, _ss, in, (void*)out);
	return -1;
}

int spidummy_spi_fini(void *cookie)
{
	printf("%s(cookie=%p)\n", __func__, cookie);
	return -1;
}

