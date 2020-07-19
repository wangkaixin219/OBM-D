
#include "variable-h.h"
#include <iostream>
#include <unistd.h>

std::list<node*> available_workers;
std::list<node*> idle_requests;
std::list<node*> holder_requests;
bool updated = false;
int time_step = 0;
extern res_t mmdh_res;
int cnt = 0;

void new_request(struct node* r) {
    if (available_workers.empty()) idle_requests.push_back(r);
    else {
        struct node* min_w = available_workers.front();
        double min_dist = dist(r, min_w);
        for (auto & available_worker : available_workers) {
            double temp_dist = dist(r, available_worker);
            if (temp_dist < min_dist) {
                min_w = available_worker;
                min_dist = temp_dist;
            }
        }
        available_workers.remove(min_w);
        hold(r, min_w);
    }
}

void new_worker(struct node* w) {
    cnt++;
    for (auto & holder_request : holder_requests) {
        struct node* matched_w = holder_request->matched_with;
        double temp_dist = dist(holder_request, w);
        if (holder_request->timer > temp_dist) {
            holder_request->matched_with = w;
            w->matched_with = holder_request;
            holder_request->timer = temp_dist;
            matched_w->matched_with = nullptr;
            new_worker(matched_w);
            return;
        }
    }
    if (!idle_requests.empty()) {
        struct node* fir_r = idle_requests.front();
        idle_requests.pop_front();
        hold(fir_r, w);
    }
    else 
        available_workers.push_back(w);
}

void reduce_timer() {
    for (auto it = holder_requests.begin(); it != holder_requests.end(); ) {
        (*it)->timer--;
        if ((*it)->timer <= 0) {
            double delay = time_step - (*it)->appear_time + dist((*it), (*it)->matched_with);
            if (!updated) {
                mmdh_res.bott_v = delay;
                updated = true;
            }
            else mmdh_res.bott_v = delay > mmdh_res.bott_v ? delay : mmdh_res.bott_v;
            it = holder_requests.erase(it);
        }
        else it++;

    }
}

void hold(struct node* r, struct node* w) {
    r->matched_with = w;
    w->matched_with = r;
    holder_requests.push_back(r);
    r->timer = dist(r, w);
}

void mmdh(data_t* gen_data) {
    struct rusage start, end;
    double usertime, systime;
    context_t *context;

    context = new context_t;
    context->req_head = gen_data->queue_r[0];
    context->work_head = gen_data->queue_w[0];

    GetCurTime(&start);
    
    while (true) {
        if (context->req_head == nullptr && context->work_head == nullptr) break;
        while (context->work_head && context->work_head->appear_time == time_step) {
            new_worker(context->work_head);
            context->work_head = context->work_head->next;
        }
        while (context->req_head && context->req_head->appear_time == time_step) {
            new_request(context->req_head);
            context->req_head = context->req_head->next;
        }

        time_step++;
        reduce_timer();
    }

    while (true) {
        if (holder_requests.empty()) break;
        time_step++;
        reduce_timer();
    }

    GetCurTime(&end);
    GetTime(&start, &end, &usertime, &systime);
    
    delete context;
    mmdh_res.running_time = usertime + systime;
    mmdh_res.memory_m = mmdh_res.memory_r = 0;
}


