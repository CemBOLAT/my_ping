/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/01 15:57:01 by vsteffen          #+#    #+#             */
/*   Updated: 2016/09/01 15:57:20 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static void		fill_zero(size_t length, char *output)
{
	if (length > 0)
	{
		while (--length > 0)
			output[length] = '0';
		output[0] = '0';
	}
}

char			*ft_itoa_base(uintmax_t nb, uint8_t base, size_t prec)
{
	char		*output;
	char		*alph;
	size_t		length;

	if (nb == 0)
		return (ft_strdup("0"));
	length = (size_t)count_numeral_base(nb, base);
	if (length < prec)
		length = prec;
	if (!(output = (char *)malloc(sizeof(char) * length + 1)))
		return (NULL);
	output[length] = '\0';
	alph = "0123456789abcdef";
	while (nb != 0)
	{
		length--;
		output[length] = alph[nb % base];
		nb /= base;
	}
	fill_zero(length, output);
	return (output);
}
