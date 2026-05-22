/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 11:58:28 by maroard           #+#    #+#             */
/*   Updated: 2026/05/22 16:05:31 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	*highest_priority_request(
	t_scheduler scheduler, t_request *a, t_request *b)
{
	if (scheduler == FIFO)
	{
		if (a->arrival_time < b->arrival_time
			|| (a->arrival_time == b->arrival_time
				&& a->coder_id < b->coder_id))
			return (a);
		return (b);
	}
	else
	{
		if (a->deadline < b->deadline
			|| (a->deadline == b->deadline
				&& a->arrival_time < b->arrival_time)
			|| (a->deadline == b->deadline
				&& a->arrival_time == b->arrival_time
				&& a->coder_id < b->coder_id))
			return (a);
		return (b);
	}
}

void	swap_requests(t_request **a, t_request **b)
{
	t_request	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_sift_up(t_heap *heap)
{
	int	child_index;
	int	parent_index;

	child_index = heap->size - 1;
	parent_index = (child_index - 1) / 2;
	while (
		child_index > 0
		&& highest_priority_request(
			heap->scheduler,
			heap->requests[child_index],
			heap->requests[parent_index]
		) == heap->requests[child_index])
	{
		swap_requests(
			&heap->requests[child_index], &heap->requests[parent_index]);
		child_index = parent_index;
		parent_index = (child_index - 1) / 2;
	}
}

static int	get_priority_index(t_heap *heap, int candidate, int best)
{
	if (candidate < heap->size
		&& highest_priority_request(
			heap->scheduler,
			heap->requests[candidate],
			heap->requests[best]
		) == heap->requests[candidate])
		return (candidate);
	return (best);
}

void	heap_sift_down(t_heap *heap)
{
	int	parent_index;
	int	left_child_index;
	int	right_child_index;
	int	priority_index;

	parent_index = 0;
	while (parent_index < heap->size)
	{
		left_child_index = parent_index * 2 + 1;
		right_child_index = parent_index * 2 + 2;
		priority_index = get_priority_index(
				heap, left_child_index, parent_index);
		priority_index = get_priority_index(
				heap, right_child_index, priority_index);
		if (priority_index == parent_index)
			return ;
		swap_requests(
			&heap->requests[parent_index], &heap->requests[priority_index]);
		parent_index = priority_index;
	}
}
