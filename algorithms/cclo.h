
#ifndef DELAY_MATCHING_V2_CCLO_H
#define DELAY_MATCHING_V2_CCLO_H

#include "def.h"

#define CCLO_THRESHOLD  100

#ifdef DIDI
#define CCLO_THRESHOLD  1000
#endif

#ifdef OLIST
#define CCLO_THRESHOLD  500
#endif

void cclo(data_t *gen_data);

#endif //DELAY_MATCHING_V2_CCLO_H
