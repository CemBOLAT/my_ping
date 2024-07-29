/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/09/01 14:02:42 by vsteffen          #+#    #+#             */
/*   Updated: 2019/11/26 21:47:41 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

static t_gnl	*search_fd(t_gnl **lst_fd, int fd)
{
	t_gnl *tmp;

	tmp = *lst_fd;
	while (tmp)
	{
		if (tmp->fd == fd)
			break ;
		tmp = tmp->next;
	}
	if (!tmp)
	{
		if (!(tmp = malloc(sizeof(t_gnl))))
			return (NULL);
		tmp->next = *lst_fd;
		tmp->fd = fd;
		*tmp->line = 0;
		*lst_fd = tmp;
	}
	return (tmp);
}

static intmax_t	ft_pow(int i, int j)
{
	intmax_t	ret;

	ret = 1;
	while (j > 0)
	{
		ret *= i;
		j--;
	}
	return (ret);
}

int				create_line(char **line, t_gnl *curr,
	size_t *size, size_t *curr_size)
{
	char	c;
	char	*tmp;

	c = 0;
	while (*size < *curr_size + BUFF_SIZE + 1 && (c = 1))
		*size *= 2;
	if (c || !*line)
	{
		tmp = *line;
		if (!(*line = malloc(*size)))
			return (-1);
		tmp ? (size_t)ft_strcpy(*line, tmp) : (**line = 0);
		free(tmp);
	}
	if ((tmp = ft_strchr(curr->line, '\n')))
	{
		ft_strncat(*line, curr->line, tmp - curr->line);
		ft_strcpy(curr->line, tmp + 1);
		return (1);
	}
	ft_strcat(*line, curr->line);
	*curr_size += ft_strlen(curr->line);
	*curr->line = 0;
	return (0);
}

static int		get_line(char **line, t_gnl *curr)
{
	size_t	size;
	size_t	curr_size;
	int		ret;
	int		tmp;

	size = ft_pow(2, GNL_PAGE);
	curr_size = 0;
	ret = ft_strlen(curr->line);
	while (*curr->line || (ret = read(curr->fd, curr->line, BUFF_SIZE)) > 0)
	{
		curr->line[ret] = 0;
		if ((tmp = create_line(line, curr, &size, &curr_size)))
			return (tmp);
	}
	if (ret < 0)
		return (-1);
	*(curr->line) = 0;
	if (!*line)
	{
		*line = ft_strdup("");
		return (0);
	}
	return (1);
}

int				get_next_line(const int fd, char **line)
{
	static t_gnl	*lst_fd = NULL;
	t_gnl			*curr;

	if (!line)
		return (-1);
	*line = NULL;
	if (!(curr = search_fd(&lst_fd, fd)))
		return (-1);
	return (get_line(line, curr));
}
