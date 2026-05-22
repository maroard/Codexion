/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 17:47:40 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 18:33:06 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	ctx_should_stop(t_ctx *ctx)
{
	bool	should_stop;

	pthread_mutex_lock(&ctx->stop_mutex);
	should_stop = ctx->stop;
	pthread_mutex_unlock(&ctx->stop_mutex);
	return (should_stop);
}

void	ctx_set_stop(t_ctx *ctx)
{
	pthread_mutex_lock(&ctx->stop_mutex);
	ctx->stop = true;
	pthread_mutex_unlock(&ctx->stop_mutex);
}
