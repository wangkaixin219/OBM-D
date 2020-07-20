#include "rql-adapt.h"
#include "opt.h"

using namespace std;

double restricted_q_values[RQL_REQ_MAXSIZE][RQL_WORK_MAXSIZE][RQL_LENGTH][RQL_LENGTH];
extern res_t restricted_q_res;


void cal_states(context_t *context, int *req_state, int *work_state) {
    *req_state = context->req_num < (RQL_REQ_MAXSIZE - 1) ? context->req_num  : (RQL_REQ_MAXSIZE - 1);
    *work_state = context->work_num < (RQL_WORK_MAXSIZE - 1) ? context->work_num : (RQL_WORK_MAXSIZE - 1);
}

void init_restricted_q_values() {
    for (int i = 0; i < RQL_REQ_MAXSIZE; ++i) {
        for (int j = 0; j < RQL_WORK_MAXSIZE; ++j) {
            for (int k = 0; k < RQL_LENGTH; ++k) {
                for (int p = 0; p < k; ++p) restricted_q_values[i][j][k][p] = (double) -INFINITY;
                for (int p = k; p < RQL_LENGTH; ++p) restricted_q_values[i][j][k][p] = (double) RQL_INIT * (2 * (double )(rand() / RAND_MAX) - 1);
            }
        }
    }
}

void train_restricted_q_values(data_t *gen_data, int episode) {

    int req_state, work_state, new_req_state, new_work_state, count, lt, cnt = 0;
    long time_step = min(gen_data->queue_r[0]->appear_time, gen_data->queue_w[0]->appear_time);
    double c_t = 0, cost_t, reward, lr = (double ) 1/ (100 + episode), temp, total_reward = 0;
    context_t *context;

    context = new context_t;
    context->req_head = context->req_tail = gen_data->queue_r[0];
    context->work_head = context->work_tail = gen_data->queue_w[0];
    context->req_num = context->work_num = 0;

    tick(context, time_step, RQL_LOWER_BOUND);
    time_step += RQL_LOWER_BOUND;
    cal_states(context, &req_state, &work_state);
    count = RQL_LOWER_BOUND;

    while (true) {

        if (context->req_head == nullptr && context->work_head == nullptr) break;

        temp = restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][count - RQL_LOWER_BOUND];
        lt = count;
        if ((double ) (rand() / RAND_MAX) <= 0.9) {
            for (int j = count; j <= RQL_UPPER_BOUND; ++j) {
                if (restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][j - RQL_LOWER_BOUND] > temp) {
                    temp = restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][j - RQL_LOWER_BOUND];
                    lt = j;
                }
            }
        }
        else {
            lt = count + rand() % (RQL_UPPER_BOUND - count + 1);
        }

        if (lt == count) {
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->matched_with = nullptr;
            for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->matched_with = nullptr;
            opt4online(context, time_step, RQL, true);
            cost_t = context->ext.mmd;
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) {
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
                }
            }
            if (c_t < cost_t) {
                reward = c_t - cost_t + cnt;
                c_t = cost_t;
            } else reward = cnt;
            cnt = 0;
            tick(context, time_step, RQL_LOWER_BOUND);
            time_step += RQL_LOWER_BOUND;
            cal_states(context, &new_req_state, &new_work_state);
            double max_q_value = restricted_q_values[new_req_state][new_work_state][0][0];
            for (int i = RQL_LOWER_BOUND; i <= RQL_UPPER_BOUND; ++i) {
                if (restricted_q_values[new_req_state][new_work_state][0][i - RQL_LOWER_BOUND] > max_q_value)
                    max_q_value = restricted_q_values[new_req_state][new_work_state][0][i - RQL_LOWER_BOUND];
            }
            restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][lt - RQL_LOWER_BOUND] +=
                    lr * (reward + max_q_value - restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][lt - RQL_LOWER_BOUND]);
            req_state = new_req_state;
            work_state = new_work_state;
            count = RQL_LOWER_BOUND;
        } else {
            cnt++;
            reward = -1;
            tick(context, time_step++, 1);
            cal_states(context, &new_req_state, &new_work_state);
            double max_q_value = restricted_q_values[new_req_state][new_work_state][count - RQL_LOWER_BOUND + 1][0];
            for (int i = RQL_LOWER_BOUND; i <= RQL_UPPER_BOUND; ++i) {
                if (restricted_q_values[new_req_state][new_work_state][count - RQL_LOWER_BOUND + 1][i - RQL_LOWER_BOUND] > max_q_value)
                    max_q_value = restricted_q_values[new_req_state][new_work_state][count - RQL_LOWER_BOUND + 1][i - RQL_LOWER_BOUND];
            }
            restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][lt - RQL_LOWER_BOUND] +=
                    lr * (reward + max_q_value - restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][lt - RQL_LOWER_BOUND]);
            req_state = new_req_state;
            work_state = new_work_state;
            count++;
        }
        total_reward += reward;
    }

    delete context;
}

void restricted_q_learning(data_t *gen_data) {
    int req_state, work_state,  count, lt;
    long time_step = min(gen_data->queue_r[0]->appear_time, gen_data->queue_w[0]->appear_time);
    double c_t = 0, cost_t, temp, usertime, systime;
    struct rusage start, end;

    context_t *context;

    context = new context_t;
    context->req_head = context->req_tail = gen_data->queue_r[0];
    context->work_head = context->work_tail = gen_data->queue_w[0];
    context->req_num = context->work_num = 0;

    GetCurTime(&start);
    tick(context, time_step, RQL_LOWER_BOUND);
    time_step += RQL_LOWER_BOUND;
    cal_states(context, &req_state, &work_state);
    count = RQL_LOWER_BOUND;

    while (true) {
        if (context->req_head == nullptr && context->work_head == nullptr) break;

        temp = restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][count - RQL_LOWER_BOUND];
        lt = count;
        for (int j = count; j <= RQL_UPPER_BOUND; ++j) {
            if (restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][j - RQL_LOWER_BOUND] > temp) {
                temp = restricted_q_values[req_state][work_state][count - RQL_LOWER_BOUND][j - RQL_LOWER_BOUND];
                lt = j;
            }
        }

        if (lt == count) {
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->matched_with = nullptr;
            for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->matched_with = nullptr;
            opt4online(context, time_step, RQL, false);
            cost_t = context->ext.mmd;
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) {
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
                }
            }
            c_t = c_t > cost_t ? c_t : cost_t;
            tick(context, time_step, RQL_LOWER_BOUND);
            time_step += RQL_LOWER_BOUND;
            cal_states(context, &req_state, &work_state);
            count = RQL_LOWER_BOUND;
        } else {
            tick(context, time_step++, 1);
            cal_states(context, &req_state, &work_state);
            count++;
        }
    }

    GetCurTime(&end);
    GetTime(&start, &end, &usertime, &systime);

    delete context;
    restricted_q_res.bott_v = c_t;
    restricted_q_res.running_time = usertime + systime;
}
