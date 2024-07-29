/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_int64_to_array.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 20:13:07 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/12 20:20:42 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	nb_numeral(int64_t n)
{
	size_t count;

	count = 1;
	while (n /= 10)
		count++;
	return (count);
}

char			*ft_int64_to_array(int64_t n)
{
	size_t				len;
	char				*str;
	uint64_t			tmp_nb;

	len = nb_numeral(n);
	tmp_nb = n;
	if (n < 0)
	{
		tmp_nb = -n;
		len++;
	}
	str = (char*)malloc(sizeof(char) * len + 1);
	if (!str)
		return (NULL);
	str[len] = 0;
	str[--len] = tmp_nb % 10 + '0';
	while (tmp_nb /= 10)
		str[--len] = tmp_nb % 10 + '0';
	if (n < 0)
		str[0] = '-';
	return (str);
}
