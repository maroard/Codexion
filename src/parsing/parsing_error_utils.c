/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_error_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 17:55:00 by maroard           #+#    #+#             */
/*   Updated: 2026/05/18 17:49:22 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

void	print_args_error(char *error_message)
{
	fprintf(stderr, "Error: %s\n", error_message);
	printf("\nUsage:\n");
	printf("  ./codexion number_of_coders time_to_burnout time_to_compile ");
	printf("time_to_debug time_to_refactor number_of_compiles_required ");
	printf("dongle_cooldown scheduler\n");
	printf("\nArguments:\n");
	printf("  number_of_coders              int    >= 1\n");
	printf("  time_to_burnout               int    milliseconds\n");
	printf("  time_to_compile               int    milliseconds\n");
	printf("  time_to_debug                 int    milliseconds\n");
	printf("  time_to_refactor              int    milliseconds\n");
	printf("  number_of_compiles_required   int    >= 0\n");
	printf("  dongle_cooldown               int    milliseconds\n");
	printf("  scheduler                     string fifo | edf\n");
	printf("\nExample:\n");
	printf("  ./codexion 5 800 200 200 200 7 50 fifo\n");
}

int	print_invalid_arg(char *name, char *expected, char *received)
{
	fprintf(stderr, "Error: %s must be %s ", name, expected);
	fprintf(stderr, "(received: %s)\n", received);
	return (0);
}
