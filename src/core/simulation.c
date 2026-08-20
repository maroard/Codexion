/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 22:38:47 by maroard           #+#    #+#             */
/*   Updated: 2026/06/11 11:41:27 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	abort_simulation_start(
	t_ctx *ctx, unsigned int coder_threads_created)
{
	unsigned int	i;

	i = 0;
	ctx_set_stop(ctx);
	wake_all_dongles(ctx);
	while (i < coder_threads_created)
		join_thread(ctx->coders[i++].thread);
}

int	start_threads(t_ctx *ctx)
{
	unsigned int	i;
	unsigned int	coder_threads_created;

	i = 0;
	coder_threads_created = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		if (!create_thread(
				&ctx->coders[i].thread, coder_routine, &ctx->coders[i])
		)
			return (abort_simulation_start(ctx, coder_threads_created), 0);
		++coder_threads_created;
		++i;
	}
	if (!create_thread(&ctx->monitor_thread, monitor_routine, ctx))
		return (abort_simulation_start(ctx, coder_threads_created), 0);
	return (1);
}

int	join_threads(t_ctx *ctx)
{
	unsigned int	i;

	i = 0;
	while ((int)i < ctx->config.number_of_coders)
	{
		if (!join_thread(ctx->coders[i].thread))
			return (0);
		++i;
	}
	if (!join_thread(ctx->monitor_thread))
		return (0);
	return (1);
}
