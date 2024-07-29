/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <vsteffen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/12/22 22:39:23 by vsteffen          #+#    #+#             */
/*   Updated: 2015/12/23 04:58:07 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *s1, const char *s2, size_t n)
{
	size_t	len;

	len = ft_strlen(s2);
	if (!len)
		return ((char *)s1);
	while (*s1 && n-- >= len)
	{
		if (!ft_memcmp(s1, s2, len))
			return ((char *)s1);
		s1++;
	}
	return (NULL);
}
