#include "codexion.h"
#include <string.h>

void	init(int argc, char *argv[], t_ctx *ctx)
{
	memset(&ctx, 0, sizeof(ctx));
	set_config(argc, argv, ctx);
}
