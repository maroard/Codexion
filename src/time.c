/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:24:49 by maroard           #+#    #+#             */
/*   Updated: 2026/05/21 15:42:30 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>
#include <unistd.h>


long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

long	get_elapsed_ms(t_ctx *ctx)
{
	return (get_time_ms() - ctx->start_time);
}

int	smart_sleep(t_ctx *ctx, long duration_ms)
{
	long	start;

	start = get_time_ms();
	while (!ctx_should_stop(ctx))
	{
		if (get_time_ms() - start >= duration_ms)
			return (1);
		usleep(500);
	}

	return (0);
}
