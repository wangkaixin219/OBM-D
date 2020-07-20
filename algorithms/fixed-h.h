
#ifndef DELAY_MATCHING_V2_CCLO_H
#define DELAY_MATCHING_V2_CCLO_H

#include "def.h"

#if !defined(DIDI) && !defined(OLIST)
#define CCLO_THRESHOLD  100

#elif defined(DIDI)
#define CCLO_THRESHOLD  1000

#elif defined(OLIST)
#define CCLO_THRESHOLD  500

#endif

void cclo(data_t *gen_data);

#endif //DELAY_MATCHING_V2_CCLO_H
