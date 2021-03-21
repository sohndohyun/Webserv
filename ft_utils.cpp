#include "ft_utils.hpp"

void *ft_memset(void *s, int c, unsigned long n)
{
	unsigned char	*ptr;

	ptr = static_cast<unsigned char*>(s);
	while (n-- > 0)
		*ptr++ = c;
	return (s);
}

bool islittelendian()
{
	int n = 1;
	if (((char*)(&n))[0] == 1)
		return true;
	return false;
}

short ft_htons(short n)
{
	if (islittelendian())
	{
		for (int i = 0 ; i < 2 / 2; i++)
		{
			char tmp = ((char*)&n)[1 - i];
			((char*)&n)[1 - i] = ((char*)&n)[i];
			((char*)&n)[i] = tmp;
		}
	}
	return n;
}

long ft_htonl(long n)
{
	if (islittelendian())
	{
		for (int i = 0 ; i < 8 / 2; i++)
		{
			char tmp = ((char*)&n)[7 - i];
			((char*)&n)[7 - i] = ((char*)&n)[i];
			((char*)&n)[i] = tmp;
		}
	}
	return n;
}