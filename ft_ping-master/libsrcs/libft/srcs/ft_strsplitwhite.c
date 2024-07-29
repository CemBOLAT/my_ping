/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsplitwhite.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <vsteffen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/12/22 22:42:05 by vsteffen          #+#    #+#             */
/*   Updated: 2019/04/13 19:04:30 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int		nbword(const char *s)
{
	int		in_word;
	int		count;

	in_word = 0;
	count = 0;
	while (*s != '\0')
	{
		if (in_word == 1 && ft_isspace(*s))
			in_word = 0;
		else if (in_word == 0 && !ft_isspace(*s))
		{
			in_word = 1;
			count++;
		}
		s++;
	}
	return (count);
}

static int		len_w(const char *s)
{
	int				len;

	len = 0;
	while (!ft_isspace(*s) && *s != '\0')
	{
		len++;
		s++;
	}
	return (len);
}

char			**ft_strsplitwhite(char const *s)
{
	char	**tab;
	int		nb_word;
	int		index;

	if (!s)
		return (NULL);
	index = 0;
	nb_word = nbword((const char *)s);
	tab = (char **)malloc(sizeof(*tab) * (nbword((const char *)s) + 1));
	if (!tab)
		return (NULL);
	while (nb_word--)
	{
		while (ft_isspace(*s) && *s != '\0')
			s++;
		tab[index] = ft_strsub((const char *)s, 0, len_w((const char *)s));
		if (!tab[index])
			return (NULL);
		s = s + len_w(s);
		index++;
	}
	tab[index] = NULL;
	return (tab);
}
