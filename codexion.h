/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maroard <maroard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 15:31:12 by maroard           #+#    #+#             */
/*   Updated: 2026/05/18 17:50:41 by maroard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdbool.h>
# include <pthread.h>

typedef struct s_ctx	t_ctx;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_request
{
	int		coder_id;
	int		dongle_id;
	long	arrival_time;
	long	deadline;
	bool	active;
}	t_request;

typedef struct s_heap
{
	t_request		**array;
	int				size;
	int				capacity;
	t_scheduler		scheduler;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	bool			has_owner;
	int				owner_id;
	long			cooldown_until;
	t_heap			waiters;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	mutex;
	long			last_compile_start;
	int				compile_count;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_dongle		*first_dongle;
	t_dongle		*second_dongle;
	t_request		first_request;
	t_request		second_request;
	t_ctx			*ctx;
}	t_coder;

typedef struct s_ctx
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	bool			stop;
	long			start_time;
}	t_ctx;

int		set_config(int argc, char *argv[], t_ctx *ctx);
void	print_args_error(char *error_message);
int		print_invalid_arg(char *name, char *expected, char *received);
bool	is_valid_int(char *arg);
char	*get_arg_name(int index);
int		is_time_arg(int index);

#endif
