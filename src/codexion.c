/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 13:20:00 by maroard           #+#    #+#             */
/*   Updated: 2026/05/19 16:07:57 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"
#include <string.h>
#include <stdio.h>

int	main(int argc, char *argv[])
{
	t_ctx	ctx;

	if (!init_ctx(argc, argv, &ctx))
		return (-1);
	cleanup_ctx(&ctx);
	return (0);
}
