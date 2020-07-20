
#ifndef DELAY_MATCHING_V2_Q_LEARNING_H
#define DELAY_MATCHING_V2_Q_LEARNING_H

#include "def.h"

#if !defined(DIDI) && !defined(OLIST)
#define MAP_SIZE 1000
#define BIN_SIZE 30
#define STEP_SIZE 5
#define QL_UPPER_BOUND 300


#elif defined(DIDI)
#define MAP_SIZE 5000
#define BIN_SIZE 100
#define STEP_SIZE 30
#define QL_UPPER_BOUND 30 * 600


#elif defined(OLIST)
#define MAP_SIZE 5000
#define BIN_SIZE 100
#define STEP_SIZE 1000
#define QL_UPPER_BOUND 1000 * 600
#endif


#define QL_LENGTH  (int)((QL_UPPER_BOUND) / (STEP_SIZE) + 1)
#define QL_INIT             1e-4


void init_q_values();
void release_q_values();
double train_q_values(data_t *gen_data, int episode, double offline);
void q_learning(data_t *gen_data);


#endif //DELAY_MATCHING_V2_Q_LEARNING_H
