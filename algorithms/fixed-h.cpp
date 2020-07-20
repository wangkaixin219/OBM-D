
#include <ctime>
#include <cstdio>
#include "fixed-h.h"
#include "opt.h"

using namespace std;

extern res_t cclo_res;

int cmp (const void *a, const void *b) {
    node_t *req1 = *(node_t **) a, *req2 = *(node_t **)b;
    return dist(req1, req1->matched_with) < dist(req2, req2->matched_with);
}

void cclo(data_t *gen_data) {
    int cnt = 0;
    int time_step = 0;
    double cost_t, c_t = 0, usertime, systime;
    struct rusage start, end;
    context_t *context;

    context = new context_t;
    context->req_head = context->req_tail = gen_data->queue_r[0];
    context->work_head = context->work_tail = gen_data->queue_w[0];
    context->req_num = context->work_num = 0;

    GetCurTime(&start);
    tick(context, time_step, 1);
    time_step++;
    while (true) {
        if (context->req_tail == nullptr && context->work_tail == nullptr) break;
        for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) req_temp->matched_with = nullptr;
        for (node_t *work_temp = context->work_head; work_temp != context->work_tail; work_temp = work_temp->next) work_temp->matched_with = nullptr;
        if (context->req_num > CCLO_THRESHOLD && context->work_num > CCLO_THRESHOLD) {
            cnt++;
            opt4online(context, time_step, CCLO, false);
            vector<node_t *> node_v;
            for (node_t *req_temp = context->req_head; req_temp != context->req_tail; req_temp = req_temp->next) {
                if (req_temp->matched_with != nullptr) {
                    node_v.push_back(req_temp);
                }
            }
            qsort(&node_v[0], node_v.size(), sizeof(node_t *), cmp);

            cost_t = -1.0;
            for (unsigned long i = 0; i < node_v.size() - CCLO_THRESHOLD; ++i) {
                node_t *req_temp = node_v[i];
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
                    double value = dist(req_temp, work_temp) + (time_step - req_temp->appear_time);
                    cost_t = cost_t > value ? cost_t : value;
                }
            }
            c_t = cost_t > c_t ? cost_t : c_t;
        }
        tick(context, time_step, 1);
        time_step++;
    }

    opt4online(context, time_step, CCLO, false);
    cost_t = context->ext.mmd;
    c_t = cost_t > c_t ? cost_t : c_t;

    GetCurTime(&end);
    GetTime(&start, &end, &usertime, &systime);

    delete context;
    cclo_res.bott_v = c_t;
    cclo_res.running_time = usertime + systime;
}
