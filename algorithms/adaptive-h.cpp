
#include "adaptive-h.h"
#include "opt.h"

using namespace std;

double ***q_values;
extern res_t q_res;


void cal_states(context_t *context, long *lc_state, long *tr_state, long time_step) {
    if (context->req_head == context->req_tail) {
        *lc_state = 0;
        *tr_state = 0;
    } else if (context->work_head == context->work_tail) {
        long time_diff = (time_step - context->req_head->appear_time) / STEP_SIZE;
        *lc_state = time_diff < QL_LENGTH - 1 ? time_diff : (QL_LENGTH - 1);
        *tr_state = 0;
    } else {
        long time_diff = (time_step - context->req_head->appear_time) / STEP_SIZE;
        *lc_state = time_diff < QL_LENGTH - 1 ? time_diff : (QL_LENGTH - 1);
        double distance, min_value, minmax_value = 1;
        if (context->req_num <= context->work_num) {
            for (node_t *p = context->req_head; p != context->req_tail; p = p->next) {
                min_value = INFINITY;
                for (node_t *q = context->work_head; q != context->work_tail; q = q->next) {
                    distance = dist(p, q);
                    if (distance < min_value) min_value = distance;
                }
                if (min_value != INFINITY && minmax_value < min_value) minmax_value = min_value;
            }
        } else {
            for (node_t *q = context->work_head; q != context->work_tail; q = q->next) {
                min_value = INFINITY;
                for (node_t *p = context->work_head; p != context->work_tail; p = p->next) {
                    distance = dist(p, q);
                    if (distance < min_value) min_value = distance;
                }
                if (min_value != INFINITY && minmax_value < min_value) minmax_value = min_value;
            }
        }
        *tr_state = (int) minmax_value / BIN_SIZE;
    }
}

void init_q_values() {

    int max_tr = (2 * MAP_SIZE) / BIN_SIZE + 1;
    q_values = new double** [max_tr + 1];
    for (int i = 0; i <= max_tr; ++i) {
        q_values[i] = new double* [QL_LENGTH];
        for (int j = 0; j < QL_LENGTH; ++j) {
            q_values[i][j] = new double[QL_LENGTH];
            for (int k = 0; k < QL_LENGTH; ++k) {
                q_values[i][j][k] = QL_INIT * (2 * (double )(rand() / RAND_MAX) - 1);
            }
        }
    }
}

void release_q_values() {
    int max_tr = (2 * MAP_SIZE) / BIN_SIZE + 1;
    for (int i = 0; i <= max_tr; ++i) {
        for (int j = 0; j < QL_LENGTH; ++j) {
            delete [] q_values[i][j];
        }
        delete [] q_values[i];
    }
    delete [] q_values;
}

double train_q_values(data_t *gen_data, int episode, double offline) {
    int predict, cnt = 0;
    long lc_state, tr_state, new_lc_state, new_tr_state;
    long time_step = min(gen_data->queue_r[0]->appear_time, gen_data->queue_w[0]->appear_time);
    double c_t = offline, cost_t, reward, learning_rate = (double) 1 / (100 + episode), temp;
    context_t *context;

    context = new context_t;
    context->req_head = context->req_tail = gen_data->queue_r[0];
    context->work_head = context->work_tail = gen_data->queue_w[0];
    context->req_num = context->work_num = 0;

    tick(context, time_step, STEP_SIZE);
    time_step += STEP_SIZE; 
    cal_states(context, &lc_state, &tr_state, time_step);

    while (true) {

        if (context->req_head == nullptr && context->work_head == nullptr) break;

        predict = 1;
        temp = q_values[tr_state][lc_state][predict];
        if ((double) rand() / RAND_MAX <= 0.9) {
            for (int j = 1; j < QL_LENGTH; ++j) {
                if (q_values[tr_state][lc_state][j] > temp) {
                    temp = q_values[tr_state][lc_state][j];
                    predict = j;
                }
            }
        }
        else predict = 1 + rand() % (QL_LENGTH - 1);

        if (predict <= lc_state) {

            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->matched_with = nullptr;
            for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->matched_with = nullptr;
            opt4online(context, time_step, QL, true);
            cost_t = -1.0;
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail && req_temp->appear_time <= time_step - (lc_state - predict) * STEP_SIZE; req_temp = req_temp->next) {
                if (req_temp->matched_with != nullptr) {
                    if (req_temp->pre == nullptr && req_temp->next == nullptr)
                        context->req_head = nullptr;
                    else if (req_temp->pre == nullptr) {
                        req_temp->next->pre = nullptr;
                        context->req_head = req_temp->next;
                    }
                    else if (req_temp->next == nullptr)
                        req_temp->pre->next = nullptr;
                    else {
                        req_temp->pre->next = req_temp->next;
                        req_temp->next->pre = req_temp->pre;
                    }
                    node_t *work_temp = req_temp->matched_with;
                    if (work_temp->pre == nullptr && work_temp->next == nullptr)
                        context->work_head = nullptr;
                    else if (work_temp->pre == nullptr) {
                        work_temp->next->pre = nullptr;
                        context->work_head = work_temp->next;
                    }
                    else if (work_temp->next == nullptr)
                        work_temp->pre->next = nullptr;
                    else {
                        work_temp->pre->next = work_temp->next;
                        work_temp->next->pre = work_temp->pre;
                    }
                    context->req_num--;
                    context->work_num--;
                    double value = dist(req_temp, work_temp) + (double) (time_step - req_temp->appear_time);
                    cost_t = cost_t > value ? cost_t : value;
                }
            }
            if (c_t < cost_t) {
                reward = c_t - cost_t + cnt;
                c_t = cost_t;
            }
            else reward = cnt;
            cnt = 0;
        } else {
            cnt += STEP_SIZE; 
            reward = -STEP_SIZE ;
        }

        tick(context, time_step, STEP_SIZE);
        time_step += STEP_SIZE;
        cal_states(context, &new_lc_state, &new_tr_state, time_step);
        double max_q_value = q_values[new_tr_state][new_lc_state][1];
        for (int i = 1; i < QL_LENGTH; ++i) {
            if (q_values[new_tr_state][new_lc_state][i] > max_q_value)
                max_q_value = q_values[new_tr_state][new_lc_state][i];
        }
        q_values[tr_state][lc_state][predict] += learning_rate * (reward + max_q_value - q_values[tr_state][lc_state][predict]);
        tr_state = new_tr_state;
        lc_state = new_lc_state;

    }
    delete context;
    return c_t;
}

void q_learning(data_t *gen_data) {

    int predict;
    long lc_state, tr_state;
    long time_step = min(gen_data->queue_r[0]->appear_time, gen_data->queue_w[0]->appear_time);
    double c_t = 0, cost_t, temp, usertime, systime;

    struct rusage start, end;
    context_t *context;

    context = new context_t;
    context->req_head = context->req_tail = gen_data->queue_r[0];
    context->work_head = context->work_tail = gen_data->queue_w[0];
    context->req_num = context->work_num = 0;

    GetCurTime(&start);

    tick(context, time_step, STEP_SIZE);

    time_step += STEP_SIZE; 
    cal_states(context, &lc_state, &tr_state, time_step);

    while (true) {

        if (context->req_head == nullptr && context->work_head == nullptr) break;

        predict = 1;
        temp = q_values[tr_state][lc_state][predict];
        for (int j = 1; j < QL_LENGTH; ++j) {
            if (q_values[tr_state][lc_state][j] > temp) {
                temp = q_values[tr_state][lc_state][j];
                predict = j;
            }
        }

        if (predict <= lc_state) {
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->matched_with = nullptr;
            for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->matched_with = nullptr;
            opt4online(context, time_step, QL, false);
            cost_t = -1.0;
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail && req_temp->appear_time <= time_step - (lc_state - predict) * STEP_SIZE; req_temp = req_temp->next) {
                if (req_temp->matched_with != nullptr) {
                    if (req_temp->pre == nullptr && req_temp->next == nullptr)
                        context->req_head = nullptr;
                    else if (req_temp->pre == nullptr) {
                        req_temp->next->pre = nullptr;
                        context->req_head = req_temp->next;
                    } else if (req_temp->next == nullptr)
                        req_temp->pre->next = nullptr;
                    else {
                        req_temp->pre->next = req_temp->next;
                        req_temp->next->pre = req_temp->pre;
                    }
                    node_t *work_temp = req_temp->matched_with;
                    if (work_temp->pre == nullptr && work_temp->next == nullptr)
                        context->work_head = nullptr;
                    else if (work_temp->pre == nullptr) {
                        work_temp->next->pre = nullptr;
                        context->work_head = work_temp->next;
                    } else if (work_temp->next == nullptr)
                        work_temp->pre->next = nullptr;
                    else {
                        work_temp->pre->next = work_temp->next;
                        work_temp->next->pre = work_temp->pre;
                    }
                    context->req_num--;
                    context->work_num--;
                    double value = dist(req_temp, work_temp) + (double) (time_step - req_temp->appear_time);
                    cost_t = cost_t > value ? cost_t : value;
                }
            }
            c_t = c_t > cost_t ? c_t : cost_t;
            tick(context, time_step, STEP_SIZE);
            time_step += STEP_SIZE;
            cal_states(context, &lc_state, &tr_state, time_step);

        } else {
            tick(context, time_step, STEP_SIZE);
            time_step += STEP_SIZE;
            cal_states(context, &lc_state, &tr_state, time_step);
        }

    }

    GetCurTime(&end);
    GetTime(&start, &end, &usertime, &systime);

    delete context;
    q_res.bott_v = c_t;
    q_res.running_time = usertime + systime;
}
