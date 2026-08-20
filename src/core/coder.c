/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 18:16:47 by maroard           #+#    #+#             */
/*   Updated: 2026/08/20 14:44:38 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	refactoring_phase(t_coder *coder)
{
	log_event(coder->ctx, coder->id, MSG_REFACTORING);
	if (!smart_sleep(coder->ctx, coder->ctx->config.time_to_refactor))
		return (0);
	return (1);
}

static int	debugging_phase(t_coder *coder)
{
	log_event(coder->ctx, coder->id, MSG_DEBUGGING);
	if (!smart_sleep(coder->ctx, coder->ctx->config.time_to_debug))
		return (0);
	return (1);
}

static int	compiling_phase(t_coder *coder)
{
	long	now;

	pthread_mutex_lock(&coder->mutex);
	now = get_elapsed_ms(coder->ctx);
	if (now - coder->last_compile_start
		>= coder->ctx->config.time_to_burnout)
	{
		pthread_mutex_unlock(&coder->mutex);
		return (0);
	}
	coder->last_compile_start = now;
	pthread_mutex_unlock(&coder->mutex);
	log_event(coder->ctx, coder->id, MSG_COMPILING);
	if (!smart_sleep(coder->ctx, coder->ctx->config.time_to_compile))
		return (0);
	pthread_mutex_lock(&coder->mutex);
	++coder->compile_count;
	pthread_mutex_unlock(&coder->mutex);
	return (1);
}

static int	cycle(t_coder *coder)
{
	if (!request_dongle(
			coder->ctx, coder, coder->first_dongle, &coder->first_request
		)
	)
		return (0);
	if (!request_dongle(
			coder->ctx, coder, coder->second_dongle, &coder->second_request
		)
	)
		return (release_dongle(coder->ctx, coder->first_dongle), 0);
	if (!compiling_phase(coder))
	{
		release_dongle(coder->ctx, coder->first_dongle);
		release_dongle(coder->ctx, coder->second_dongle);
		return (0);
	}
	release_dongle(coder->ctx, coder->first_dongle);
	release_dongle(coder->ctx, coder->second_dongle);
	if (!debugging_phase(coder) || !refactoring_phase(coder))
		return (0);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!ctx_should_stop(coder->ctx))
	{
		if (coder->first_dongle == coder->second_dongle)
		{
			smart_sleep(coder->ctx, coder->ctx->config.time_to_burnout);
			return (NULL);
		}
		pthread_mutex_lock(&coder->mutex);
		if (
			coder->compile_count
			>= coder->ctx->config.number_of_compiles_required
		)
			return (pthread_mutex_unlock(&coder->mutex), NULL);
		pthread_mutex_unlock(&coder->mutex);
		if (!cycle(coder))
			return (NULL);
	}
	return (NULL);
}
