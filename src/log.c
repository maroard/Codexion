/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 18:05:52 by maroard           #+#    #+#             */
/*   Updated: 2026/05/21 18:25:24 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <string.h>
#include <stdio.h>

void	log_state(t_ctx *ctx, int coder_id, char *message)
{
    long    elapsed_time;

    if (strcmp(message, MSG_BURNED_OUT) && ctx_should_stop(ctx))
        return ;
    elapsed_time = get_time_ms() - ctx->coders[coder_id].last_compile_start;
    pthread_mutex_lock(&ctx->log_mutex);
    printf("%ld %d %s\n", elapsed_time, coder_id, message);
    pthread_mutex_unlock(&ctx->log_mutex);
}
