
#ifndef DELAY_MATCHING_V2_DATA_H
#define DELAY_MATCHING_V2_DATA_H

#include "def.h"

data_t* generate_data(config_t *data_config);
void reset_data(data_t *gen_data);
void free_data_mem(data_t *gen_data);

data_t* read_data_from_file(const char *filename);

#endif //DELAY_MATCHING_V2_DATA_H
