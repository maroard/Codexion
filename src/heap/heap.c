/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 11:36:27 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 15:55:32 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_push(t_heap *heap, t_request *request)
{
	if (heap->size == heap->capacity)
		return (0);
	heap->requests[heap->size] = request;
	++heap->size;
	heap_sift_up(heap);
	return (1);
}

t_request	*heap_peek(t_heap *heap)
{
	if (heap_is_empty(heap))
		return (NULL);
	return (heap->requests[0]);
}

t_request	*heap_pop(t_heap *heap)
{
	t_request	*request;

	if (heap_is_empty(heap))
		return (NULL);
	request = heap->requests[0];
	--heap->size;
	if (!heap_is_empty(heap))
	{
		heap->requests[0] = heap->requests[heap->size];
		heap_sift_down(heap);
	}
	return (request);
}

bool	heap_is_empty(t_heap *heap)
{
	if (heap->size == 0)
		return (true);
	return (false);
}
