/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 13:20:00 by maroard           #+#    #+#             */
/*   Updated: 2026/06/11 14:28:58 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_simulation(t_ctx *ctx)
{
	ctx->start_time = get_time_ms();
	if (!start_threads(ctx))
		return (0);
	if (!join_threads(ctx))
		return (0);
	return (1);
}

int	main(int argc, char *argv[])
{
	t_ctx	ctx;

	if (!init_ctx(argc, argv, &ctx))
		return (-1);
	if (!start_simulation(&ctx))
		return (cleanup_ctx(&ctx), -1);
	cleanup_ctx(&ctx);
	return (0);
}
