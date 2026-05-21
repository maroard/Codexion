/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 13:20:00 by maroard           #+#    #+#             */
/*   Updated: 2026/05/20 14:17:24 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_mutex(pthread_mutex_t *mutex)
{
	if (pthread_mutex_init(mutex, NULL) != 0)
		return (0);
	return (1);
}

int	init_cond(pthread_cond_t *cond)
{
	if (pthread_cond_init(cond, NULL) != 0)
		return (0);
	return (1);
}

void	assign_coder_dongles(t_ctx *ctx, t_coder *coder, unsigned int i)
{
	coder->left_dongle = &ctx->dongles[i];
	coder->right_dongle = &ctx->dongles[(i + 1) % ctx->config.number_of_coders];
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		coder->first_dongle = coder->left_dongle;
		coder->second_dongle = coder->right_dongle;
	}
	else
	{
		coder->first_dongle = coder->right_dongle;
		coder->second_dongle = coder->left_dongle;
	}
}

void	clean_current_dongle(t_dongle *dongle, int cond_ready)
{
	if (cond_ready)
		pthread_cond_destroy(&dongle->cond);
	pthread_mutex_destroy(&dongle->mutex);
}
