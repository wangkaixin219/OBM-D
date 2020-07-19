//
// Created by 王恺忻 on 2019-02-20.
//

#ifndef BOTTLENECK_MMDH_H
#define BOTTLENECK_MMDH_H

#include <list>


#include "def.h"
#include "hst.h"


void load_hst(string path, data_t* data, int len, int wid);

void new_requset(struct node* r);

void new_worker(struct node* w);

void reduce_timer();

void hold(struct node* r, struct node* w);

void pop_holder();

void mmdh(data_t*);

/*
class MMDH {
public:
    HSTree          tree;
    VVI             worker_free_list, request_idle_list;
    VVI             paired_list;
    VVI             seq;
    VVI             request_list, worker_list;
    VI              Lmate, Rmate;
    PII             btnk_pair;
    MMDH(string, VVI&, int, int);
    double solve();

private:
    void new_request_comes(VI);
    void new_worker_comes(VI);
    double hst_dist(VI, VI);
    void min_dis_to_request(VI, VI&);
    void hold(VI, VI);
    void pop_holder(int);

}; */


#endif //BOTTLENECK_MMDH_H