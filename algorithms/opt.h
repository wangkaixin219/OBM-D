
#ifndef DELAY_MATCHING_V2_OPT_H
#define DELAY_MATCHING_V2_OPT_H

#include "def.h"

void opt(data_t *gen_data);
void opt4online(context_t *context, long time_step, int algorithm, bool training);

#endif //DELAY_MATCHING_V2_OPT_H
