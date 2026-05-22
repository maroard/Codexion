/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 15:21:17 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 23:50:56 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

void	wake_all_dongles(t_ctx *ctx)
{
	unsigned int	i;
	t_dongle		*current;

	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		current = &ctx->dongles[i];
		pthread_mutex_lock(&current->mutex);
		pthread_cond_broadcast(&current->cond);
		pthread_mutex_unlock(&current->mutex);
		++i;
	}
}

static void	stop_simulation(t_ctx *ctx, t_coder *burned_out_coder)
{
	ctx_set_stop(ctx);
	if (burned_out_coder)
		log_event(ctx, burned_out_coder->id, MSG_BURNED_OUT);
	wake_all_dongles(ctx);
}

static bool	all_coders_done(t_ctx *ctx)
{
	unsigned int	i;
	t_coder			*current;
	int				compile_count;

	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		current = &ctx->coders[i];
		pthread_mutex_lock(&current->mutex);
		compile_count = current->compile_count;
		pthread_mutex_unlock(&current->mutex);
		if (compile_count < ctx->config.number_of_compiles_required)
			return (false);
		++i;
	}
	return (true);
}

static t_coder	*find_burned_out_coder(t_ctx *ctx)
{
	unsigned int	i;
	t_coder			*current;
	long			time_since_compile;

	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		current = &ctx->coders[i];
		pthread_mutex_lock(&current->mutex);
		time_since_compile = get_elapsed_ms(ctx) - current->last_compile_start;
		if (
			current->compile_count < ctx->config.number_of_compiles_required
			&& time_since_compile >= ctx->config.time_to_burnout
		)
			return (pthread_mutex_unlock(&current->mutex), current);
		pthread_mutex_unlock(&current->mutex);
		++i;
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_ctx		*ctx;
	t_coder		*burned_out_coder;

	ctx = (t_ctx *)arg;
	while (!ctx_should_stop(ctx))
	{
		burned_out_coder = find_burned_out_coder(ctx);
		if (burned_out_coder || all_coders_done(ctx))
			return (stop_simulation(ctx, burned_out_coder), NULL);
		usleep(1000);
	}
	return (NULL);
}
