
#include <ctime>
#include <cstring>
#include "opt.h"

extern res_t q_res, restricted_q_res, cclo_res, opt_res;

bool init_matching_sort(context_t *context, long time_step, int algorithm, bool training) {
    if (context->req_num == 0 || context->work_num == 0) return false;
    node_t *ext_req = nullptr, *ext_work = nullptr;
    double mmd = 0;
    for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) {
        node_t *best_work = nullptr;
        double cur_value, best_value = INFINITY;
        for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) {
            if (work_temp->matched_with == nullptr) {
                if (time_step < 0) {
                    long match_time = req_temp->appear_time > work_temp->appear_time ? req_temp->appear_time : work_temp->appear_time;
                    cur_value = dist(req_temp, work_temp) + (double) (match_time - req_temp->appear_time);
                } else cur_value = dist(req_temp, work_temp) + (double) (time_step - req_temp->appear_time);
                if (cur_value < best_value) {
                    best_value = cur_value;
                    best_work = work_temp;
                }
            }
        }
        if (best_work != nullptr) {
            req_temp->matched_with = best_work;
            best_work->matched_with = req_temp;
            if (best_value > mmd) {
                mmd = best_value;
                ext_req = req_temp;
                ext_work = best_work;
            }
        }
    }
    context->ext.mmd = mmd;
    context->ext.r = ext_req;
    context->ext.w = ext_work;
    return true;
}

bool init_matching(context_t *context, int time_step, int algorithm, bool training) {
    if (context->req_num == 0 || context->work_num == 0) return false;
    double mmd = 0, value;
    node_t *ext_req = nullptr, *ext_work = nullptr;
    for (node_t *req_temp = context->req_head, *work_temp = context->work_head; req_temp != context->req_tail && work_temp != context->work_tail; req_temp = req_temp->next, work_temp = work_temp->next) {
        req_temp->matched_with = work_temp;
        work_temp->matched_with = req_temp;
        if (time_step < 0) {
            long match_time = req_temp->appear_time > work_temp->appear_time ? req_temp->appear_time : work_temp->appear_time;
            value = dist(req_temp, work_temp) + (double) (match_time - req_temp->appear_time);
        } else value = dist(req_temp, work_temp) + (double) (time_step - req_temp->appear_time);
        if (value > mmd) {
            mmd = value;
            ext_req = req_temp;
            ext_work = work_temp;
        }
    }
    context->ext.mmd = mmd;
    context->ext.r = ext_req;
    context->ext.w = ext_work;
    return true;
}

bool break_extreme_pair(context_t *context) {
    if (context->ext.r != nullptr && context->ext.w != nullptr) {
        context->ext.r->matched_with = nullptr;
        context->ext.w->matched_with = nullptr;
        return true;
    }
    return false;
}


void reform_matching(context_t *context, queue_node_t **queue, int rear, int time_step, int algorithm, bool training) {
    int cur = rear - 1;
    while (cur != -1) {
        node_t *work = queue[cur]->q_node;
        cur = queue[cur]->prev;
        node_t *req = queue[cur]->q_node;
        work->matched_with = req;
        req->matched_with = work;
        cur = queue[cur]->prev;
    }

    double mmd = 0, value;
    node_t *ext_req = nullptr, *ext_work = nullptr;
    for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) {
        node_t *matched_work = req_temp->matched_with;
        if (matched_work != nullptr) {
            if (time_step < 0) {
                long match_time = req_temp->appear_time > matched_work->appear_time ? req_temp->appear_time : matched_work->appear_time;
                value = dist(req_temp, matched_work) +(double) (match_time - req_temp->appear_time);
            } else value = dist(req_temp, matched_work) +(double) (time_step - req_temp->appear_time);
            if (value > mmd) {
                mmd = value;
                ext_req = req_temp;
                ext_work = matched_work;
            }
        }
    }
    context->ext.mmd = mmd;
    context->ext.r = ext_req;
    context->ext.w = ext_work;
}

bool find_swap_chain(context_t *context, long time_step, int algorithm, bool training) {
    for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->checked = false;
    for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->checked = false;

    int cur = 0, rear = 0;
    bool succeed = false;
    queue_node_t *q_node_v = nullptr, *q_node_temp = nullptr;
    queue_node_t **queue = nullptr;

    queue = new queue_node_t*[context->req_num + context->work_num];
    q_node_v = new queue_node_t;
    q_node_v->prev = -1;
    q_node_v->q_node = context->ext.r;
    q_node_v->q_node->checked = true;
    queue[rear++] = q_node_v;

    while (cur != rear) {
        q_node_temp = queue[cur];
        if (q_node_temp->q_node->type == 0) {
            double value;
            for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) {
                if (time_step < 0) {
                    long match_time = q_node_temp->q_node->appear_time > work_temp->appear_time ? q_node_temp->q_node->appear_time : work_temp->appear_time;
                    value = dist(q_node_temp->q_node, work_temp) + (double) (match_time - q_node_temp->q_node->appear_time);
                } else value = dist(q_node_temp->q_node, work_temp) + (double) (time_step - q_node_temp->q_node->appear_time);
                if (!work_temp->checked && value < context->ext.mmd) {
                    work_temp->checked = true;
                    q_node_v = new queue_node_t;
                    q_node_v->q_node = work_temp;
                    q_node_v->prev = cur;
                    queue[rear++] = q_node_v;
                    if (work_temp->matched_with == nullptr) {
                        succeed = true;
                        goto SUCCESS;
                    }
                }
            }
        } else {
           if (!q_node_temp->q_node->matched_with->checked) {
                q_node_temp->q_node->matched_with->checked = true;
                q_node_v = new queue_node_t;
                q_node_v->q_node = q_node_temp->q_node->matched_with;
                q_node_v->prev = cur;
                queue[rear++] = q_node_v;
            }
        }
        cur++;
    }
SUCCESS:
    if (succeed) reform_matching(context, queue, rear, time_step, algorithm, training);
    else {
        context->ext.r->matched_with = context->ext.w;
        context->ext.w->matched_with = context->ext.r;
    }
    for (int i = 0; i < rear; ++i) delete queue[i];
    delete queue;
    return succeed;
}

void opt4offline(context_t *context) {
    if (init_matching_sort(context, -1, OPT, false)) {
        break_extreme_pair(context);
        while (find_swap_chain(context, -1, OPT, false)) break_extreme_pair(context);
    }
}

void opt4online(context_t *context, long time_step, int algorithm, bool training) {
    if (init_matching_sort(context, time_step, algorithm, training)) {
        break_extreme_pair(context);
        while (find_swap_chain(context, time_step, algorithm, training)) break_extreme_pair(context);
    }
}

void opt(data_t *gen_data) {
    double c_t, usertime, systime;
    struct rusage start, end;
    context_t *context;

    context = new context_t;
    context->req_head = gen_data->queue_r[0];
    context->work_head = gen_data->queue_w[0];
    context->work_tail = context->req_tail = nullptr;
    context->req_num = context->work_num = gen_data->cardinality;
    
    GetCurTime(&start);
    opt4offline(context);
    GetCurTime(&end);
    GetTime(&start, &end, &usertime, &systime);
    c_t = context->ext.mmd;
    
    delete context;
    opt_res.bott_v = c_t;
    opt_res.running_time = usertime + systime;
}
