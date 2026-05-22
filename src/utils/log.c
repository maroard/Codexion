/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 18:05:52 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 19:24:15 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <string.h>
#include <stdio.h>

void	log_event(t_ctx *ctx, int coder_id, char *message)
{
	pthread_mutex_lock(&ctx->log_mutex);
	if (strcmp(message, MSG_BURNED_OUT) && ctx_should_stop(ctx))
	{
		pthread_mutex_unlock(&ctx->log_mutex);
		return ;
	}
	printf("%ld %d %s\n", get_elapsed_ms(ctx), coder_id, message);
	pthread_mutex_unlock(&ctx->log_mutex);
}
