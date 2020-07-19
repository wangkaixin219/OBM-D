//
// Created by Kaixin Wang on 10/4/2019.
//

#ifndef DELAY_MATCHING_V2_RESTRICTED_Q_LEARNING_H
#define DELAY_MATCHING_V2_RESTRICTED_Q_LEARNING_H

#include "def.h"

#define RQL_UPPER_BOUND     50
#define RQL_LOWER_BOUND     1
#define RQL_REQ_MAXSIZE     1000
#define RQL_WORK_MAXSIZE    1000


#ifdef DIDI
#define RQL_UPPER_BOUND     100
#define RQL_LOWER_BOUND     1
#define RQL_REQ_MAXSIZE     200
#define RQL_WORK_MAXSIZE    200
#endif


#ifdef OLIST
#define RQL_UPPER_BOUND     100
#define RQL_LOWER_BOUND     1
#define RQL_REQ_MAXSIZE     1000
#define RQL_WORK_MAXSIZE    1000
#endif


#define RQL_LENGTH          (RQL_UPPER_BOUND - RQL_LOWER_BOUND + 1)
#define RQL_INIT            1e-4


void init_restricted_q_values();
void train_restricted_q_values(data_t *gen_data, int episode);
void restricted_q_learning(data_t *gen_data);


#endif //DELAY_MATCHING_V2_RESTRICTED_Q_LEARNING_H
