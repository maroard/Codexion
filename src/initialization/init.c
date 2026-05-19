/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 13:20:00 by maroard           #+#    #+#             */
/*   Updated: 2026/05/19 15:56:56 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <string.h>

static void	init_requests(t_coder *coder)
{
	memset(&coder->first_request, 0, sizeof(t_request));
	memset(&coder->second_request, 0, sizeof(t_request));
	coder->first_request.coder_id = coder->id;
	coder->second_request.coder_id = coder->id;
}

static int	init_coders(t_ctx *ctx)
{
	unsigned int	i;
	t_coder			*current;

	ctx->coders = malloc(sizeof(t_coder) * ctx->config.number_of_coders);
	if (!ctx->coders)
		return (0);
	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		current = &ctx->coders[i];
		memset(current, 0, sizeof(t_coder));
		current->id = i + 1;
		current->ctx = ctx;
		assign_coder_dongles(ctx, current, i);
		if (!init_mutex(&current->mutex))
			return (0);
		init_requests(current);
		++ctx->init_state.coders_count;
		++i;
	}
	return (1);
}

static int	init_heap(t_config config, t_heap *heap)
{
	if (config.number_of_coders == 1)
		heap->capacity = 1;
	else
		heap->capacity = 2;
	heap->size = 0;
	heap->requests = malloc(sizeof(t_request *) * heap->capacity);
	if (!heap->requests)
		return (0);
	heap->scheduler = config.scheduler;
	return (1);
}

static int	init_dongles(t_ctx *ctx)
{
	unsigned int	i;
	t_dongle		*current;

	ctx->dongles = malloc(sizeof(t_dongle) * ctx->config.number_of_coders);
	if (!ctx->dongles)
		return (0);
	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		current = &ctx->dongles[i];
		memset(current, 0, sizeof(t_dongle));
		current->id = i;
		if (!init_mutex(&current->mutex))
			return (0);
		if (!init_cond(&current->cond))
			return (clean_current_dongle(current, 0), 0);
		if (!init_heap(ctx->config, &current->request_queue))
			return (clean_current_dongle(current, 1), 0);
		++ctx->init_state.dongles_count;
		++i;
	}
	return (1);
}

int	init_ctx(int argc, char *argv[], t_ctx *ctx)
{
	memset(ctx, 0, sizeof(t_ctx));
	if (!set_config(argc, argv, &ctx->config))
		return (0);
	if (!init_mutex(&ctx->stop_mutex))
		return (0);
	ctx->init_state.stop_mutex_ready = true;
	if (!init_mutex(&ctx->log_mutex))
		return (cleanup_ctx(ctx), 0);
	ctx->init_state.log_mutex_ready = true;
	if (!init_dongles(ctx))
		return (cleanup_ctx(ctx), 0);
	if (!init_coders(ctx))
		return (cleanup_ctx(ctx), 0);
	return (1);
}
