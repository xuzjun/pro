#include "cmd.h"
#include "bu.h"

typedef struct cmd_cb_s {
	int cmd;
	int (*cb)(head_t *h);
} cmd_cb_t;

cmd_cb_t cmd_cb[] = {
	{cmd_to_upper, to_upper},
	{cmd_to_lower, to_lower}
};
