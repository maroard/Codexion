/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:16:52 by maroard           #+#    #+#             */
/*   Updated: 2026/05/18 18:19:20 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>

static int	ft_isdigit(int c)
{
	return (c >= 48 && c <= 57);
}

static bool	fits_int_limit(char *arg, unsigned int i, long long limit)
{
	long long	nb;
	int			digit;

	nb = 0;
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (false);
		digit = arg[i] - '0';
		if (nb > (limit - digit) / 10)
			return (false);
		nb = nb * 10 + digit;
		++i;
	}
	return (true);
}

bool	is_valid_int(char *arg)
{
	unsigned int	i;
	long long		limit;

	i = 0;
	limit = 2147483647;
	while ((arg[i] >= 9 && arg[i] <= 13) || arg[i] == ' ')
		++i;
	if (arg[i] == '-' || arg[i] == '+')
	{
		if (arg[i] == '-')
			limit = 2147483648;
		++i;
	}
	if (!arg[i])
		return (false);
	return (fits_int_limit(arg, i, limit));
}

char	*get_arg_name(int index)
{
	if (index == 1)
		return ("number_of_coders");
	if (index == 2)
		return ("time_to_burnout");
	if (index == 3)
		return ("time_to_compile");
	if (index == 4)
		return ("time_to_debug");
	if (index == 5)
		return ("time_to_refactor");
	if (index == 6)
		return ("number_of_compiles_required");
	if (index == 7)
		return ("dongle_cooldown");
	return ("unknown_argument");
}

int	is_time_arg(int index)
{
	return (index == 2 || index == 3 || index == 4 || index == 5);
}
