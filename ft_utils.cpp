#include "ft_utils.hpp"

void	*ft_memset(void *s, int c, unsigned long n)
{
	unsigned char	*ptr;

	ptr = static_cast<unsigned char*>(s);
	while (n-- > 0)
		*ptr++ = c;
	return (s);
}