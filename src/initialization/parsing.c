/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:19:19 by maroard           #+#    #+#             */
/*   Updated: 2026/05/18 17:50:17 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static t_config	get_config(char *argv[])
{
	t_config	config;

	config.number_of_coders = atoi(argv[1]);
	config.time_to_burnout = atoi(argv[2]);
	config.time_to_compile = atoi(argv[3]);
	config.time_to_debug = atoi(argv[4]);
	config.time_to_refactor = atoi(argv[5]);
	config.number_of_compiles_required = atoi(argv[6]);
	config.dongle_cooldown = atoi(argv[7]);
	if (!strcmp(argv[8], "fifo"))
		config.scheduler = FIFO;
	else
		config.scheduler = EDF;
	return (config);
}

static int	check_args(char *argv[])
{
	unsigned int	i;

	i = 1;
	while (i < 8)
	{
		if (!is_valid_int(argv[i]))
			return (print_invalid_arg(get_arg_name(i), "an integer", argv[i]));
		if (i == 1 && atoi(argv[i]) < 1)
			return (print_invalid_arg(get_arg_name(i),
					"an integer >= 1", argv[i]));
		if (i == 6 && atoi(argv[i]) < 0)
			return (print_invalid_arg(get_arg_name(i),
					"an integer >= 0", argv[i]));
		if (i == 7 && atoi(argv[i]) < 0)
			return (print_invalid_arg(get_arg_name(i),
					"an integer >= 0", argv[i]));
		if (is_time_arg(i) && atoi(argv[i]) < 1)
			return (print_invalid_arg(get_arg_name(i),
					"an integer > 0 milliseconds", argv[i]));
		i++;
	}
	if (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf"))
		return (print_invalid_arg("scheduler",
				"either \"fifo\" or \"edf\"", argv[8]));
	return (1);
}

int	set_config(int argc, char *argv[], t_ctx *ctx)
{
	if (argc == 1)
		return (print_args_error("None arguments were provided"), 0);
	if (argc < 9)
		return (print_args_error("Not enough arguments were provided"), 0);
	if (argc > 9)
		return (print_args_error("Too many arguments were provided"), 0);
	if (!check_args(argv))
		return (0);
	ctx->config = get_config(argv);
	return (1);
}
