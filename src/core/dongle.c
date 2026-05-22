/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 16:07:53 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 18:45:29 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <time.h>

static struct timespec	make_timespec_from_ms(long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}

static int	wait_until_pickable(
	t_ctx *ctx, t_dongle *dongle, t_request *request)
{
	struct timespec	ready_at;

	while (!(
			ctx_should_stop(ctx) == false
			&& dongle->has_owner == false
			&& get_time_ms() >= dongle->cooldown_until
			&& heap_peek(&dongle->request_queue) == request)
	)
	{
		if (ctx_should_stop(ctx))
			return (0);
		if (get_time_ms() < dongle->cooldown_until)
		{
			ready_at = make_timespec_from_ms(dongle->cooldown_until);
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ready_at);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	return (1);
}

static void	prepare_request(
	t_ctx *ctx, t_coder *coder, t_dongle *dongle, t_request *request)
{
	request->coder_id = coder->id;
	request->dongle_id = dongle->id;
	request->arrival_time = get_elapsed_ms(ctx);
	pthread_mutex_lock(&coder->mutex);
	request->deadline = coder->last_compile_start + ctx->config.time_to_burnout;
	pthread_mutex_unlock(&coder->mutex);
	request->is_active = true;
}

int	request_dongle(
	t_ctx *ctx, t_coder *coder, t_dongle *dongle, t_request *request)
{
	prepare_request(ctx, coder, dongle, request);
	pthread_mutex_lock(&dongle->mutex);
	if (!heap_push(&dongle->request_queue, request))
		return (pthread_mutex_unlock(&dongle->mutex), 0);
	if (!wait_until_pickable(ctx, dongle, request))
	{
		request->is_active = false;
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	heap_pop(&dongle->request_queue);
	dongle->has_owner = true;
	dongle->owner_id = coder->id;
	request->is_active = false;
	pthread_mutex_unlock(&dongle->mutex);
	log_event(ctx, coder->id, MSG_TAKEN_DONGLE);
	return (1);
}

void	release_dongle(t_ctx *ctx, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->has_owner = false;
	dongle->owner_id = 0;
	dongle->cooldown_until = get_time_ms() + ctx->config.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
