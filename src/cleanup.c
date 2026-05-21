/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 16:01:35 by maroard           #+#    #+#             */
/*   Updated: 2026/05/20 14:17:49 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>

static void	cleanup_heap(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->requests);
	heap->requests = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

static void	cleanup_dongles(t_ctx *ctx)
{
	unsigned int	i;
	t_dongle		*current;

	if (!ctx || !ctx->dongles)
		return ;
	i = 0;
	while (i < ctx->init_state.dongles_count)
	{
		current = &ctx->dongles[i];
		cleanup_heap(&current->request_queue);
		pthread_cond_destroy(&current->cond);
		pthread_mutex_destroy(&current->mutex);
		++i;
	}
	free(ctx->dongles);
	ctx->dongles = NULL;
	ctx->init_state.dongles_count = 0;
}

static void	cleanup_coders(t_ctx *ctx)
{
	unsigned int	i;
	t_coder			*current;

	if (!ctx || !ctx->coders)
		return ;
	i = 0;
	while (i < ctx->init_state.coders_count)
	{
		current = &ctx->coders[i];
		pthread_mutex_destroy(&current->mutex);
		++i;
	}
	free(ctx->coders);
	ctx->coders = NULL;
	ctx->init_state.coders_count = 0;
}

void	cleanup_ctx(t_ctx *ctx)
{
	if (!ctx)
		return ;
	cleanup_coders(ctx);
	cleanup_dongles(ctx);
	if (ctx->init_state.log_mutex_ready)
	{
		pthread_mutex_destroy(&ctx->log_mutex);
		ctx->init_state.log_mutex_ready = false;
	}
	if (ctx->init_state.stop_mutex_ready)
	{
		pthread_mutex_destroy(&ctx->stop_mutex);
		ctx->init_state.stop_mutex_ready = false;
	}
}
