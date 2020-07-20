
#include "data.h"
#include <random>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

void copy_from_config(config_t* config, data_t* syn_data) {
    int cardinality = config->cardinality;
    syn_data->cardinality = cardinality;

    syn_data->queue_r = (node_t **) malloc(cardinality * sizeof(node_t *));
    for (int i = 0; i < cardinality; ++i) 
        syn_data->queue_r[i] = (node_t *) malloc(sizeof(node_t));
    
    syn_data->queue_w = (node_t **) malloc(cardinality * sizeof(node_t *));
    for (int i = 0; i < cardinality; ++i) 
        syn_data->queue_w[i] = (node_t *) malloc(sizeof(node_t));

    syn_data->step_size = config->step_size;
}

int zipf(double alpha, int n)
{
//    srand((unsigned long)time(nullptr));
    static int first = 1;      // Static first time flag
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    int zipf_value = 0;        // Computed exponential value to be returned
    int    i;                     // Loop counter

    // Compute normalization constant on first call only
    if (first == 1)
    {
        for (i=1; i<=n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = 0;
    }

    // Pull a uniform random number (0 < z < 1)
    do
    {
        z = (double) rand() / RAND_MAX;
    }
    while ((z == 0) || (z == 1));

    // Map z to the value
    sum_prob = 0;
    for (i = 1; i <= n; i++)
    {
        sum_prob = sum_prob + c / pow((double) i, alpha);
        if (sum_prob >= z)
        {
            zipf_value = i;
            break;
        }
    }

    // Assert that zipf_value is between 1 and N
    assert((zipf_value >= 1) && (zipf_value <= n));
    return zipf_value;
}

data_t* generate_data(config_t *config) {

    int cardinality = config->cardinality, t_max = config->t_max, setting_type = config->setting_type;


    data_t *data = (data_t *) malloc(sizeof(data_t));
    copy_from_config(config, data);

    int x, y, app_r, app_w;
    random_device rd;
    
    vector<int> rand_r, rand_w;
    for (int i = 0; i < cardinality; i++) {

        if (setting_type == 0) {
            uniform_int_distribution<int> t_uni (0, t_max);
            app_r = t_uni(rd);
            app_w = t_uni(rd);
        }
        else if (setting_type == 1) {
            normal_distribution<> t_norm(t_max / 2.0, 200);
            int app_r_ = t_norm(rd);
            app_r = app_r_ > t_max ? t_max : app_r_ < 0 ? 0 : app_r_;
            uniform_int_distribution<int> t_uni (0, t_max);
            app_w = t_uni(rd);
        }
        else {
            app_r = zipf(2, t_max);
            uniform_int_distribution<int> t_uni(0, t_max);
            app_w = t_uni(rd);
        }
        rand_r.push_back(app_r);
        rand_w.push_back(app_w);
    }
    sort(rand_r.begin(), rand_r.end());
    sort(rand_w.begin(), rand_w.end());

    for (int i = 0; i < cardinality; i++) {

        if (setting_type == 0 || setting_type == 2) {
            uniform_int_distribution<int> sp_uni (0, 1000);
            x = sp_uni(rd);
            y = sp_uni(rd);
        }
        else {
            normal_distribution<> sp_norm(500, 50);
            int x_ = sp_norm(rd), y_ = sp_norm(rd);
            x = x_ > 500 ? 500 : x_ < 0 ? 0 : x_;
            y = y_ > 500 ? 500 : y_ < 0 ? 0 : y_;
        }


        if (i == cardinality - 1)  data->queue_r[i]->next = nullptr;
        else  data->queue_r[i]->next = data->queue_r[i + 1];
        if (i == 0)  data->queue_r[i]->pre = nullptr;
        else  data->queue_r[i]->pre = data->queue_r[i - 1];
        
        data->queue_r[i]->type = 0;
        data->queue_r[i]->id = i;
        data->queue_r[i]->appear_time = rand_r[i];
        data->queue_r[i]->match_time = -1;
        data->queue_r[i]->x = x;
        data->queue_r[i]->y = y;
        data->queue_r[i]->matched_with = nullptr;
    }
    
    for (int i = 0; i < cardinality; i++) {

        uniform_int_distribution<int> sp_uni (0, 500);
        x = sp_uni(rd);
        y = sp_uni(rd);

        if (i == cardinality - 1)  data->queue_w[i]->next = nullptr;
        else  data->queue_w[i]->next = data->queue_w[i + 1];
        if (i == 0) data->queue_w[i]->pre = nullptr;
        else  data->queue_w[i]->pre = data->queue_w[i - 1];

        data->queue_w[i]->type = 1;
        data->queue_w[i]->id = i;
        data->queue_w[i]->appear_time = rand_w[i];
        data->queue_w[i]->match_time = -1;
        data->queue_w[i]->x = x;
        data->queue_w[i]->y = y;
        data->queue_w[i]->matched_with = nullptr;
    }

    return data;
}

void reset_data(data_t *gen_data) {
    for (int i = 0; i < gen_data->cardinality; ++i) {
        if (i == 0) {
            gen_data->queue_r[i]->pre = nullptr;
            gen_data->queue_w[i]->pre = nullptr;
        }
        else {
            gen_data->queue_r[i]->pre = gen_data->queue_r[i - 1];
            gen_data->queue_w[i]->pre = gen_data->queue_w[i - 1];
        }
        if (i == gen_data->cardinality - 1) {
            gen_data->queue_r[i]->next = nullptr;
            gen_data->queue_w[i]->next = nullptr;
        }
        else {
            gen_data->queue_r[i]->next = gen_data->queue_r[i + 1];
            gen_data->queue_w[i]->next = gen_data->queue_w[i + 1];
        }
        gen_data->queue_r[i]->matched_with = nullptr;
        gen_data->queue_w[i]->matched_with = nullptr;
    }
}

void free_data_mem(data_t *gen_data) {
    for (int i = 0; i < gen_data->cardinality; ++i) {
        free(gen_data->queue_r[i]);
        gen_data->queue_r[i] = nullptr;
    }
    free(gen_data->queue_r);
    gen_data->queue_r = nullptr;
    for (int i = 0; i < gen_data->cardinality; ++i) {
        free(gen_data->queue_w[i]);
        gen_data->queue_w[i] = nullptr;
    }
    free(gen_data->queue_w);
    gen_data->queue_w = nullptr;

    free(gen_data);
    gen_data = nullptr;
}

data_t* read_data_from_file(const char *filename) {
    data_t *real_data = (data_t *) malloc(sizeof(data_t));
    FILE* fp = fopen(filename, "r");
    int id = 0, i;
    long req_appear, work_appear;
    double req_lng, req_lat, work_lng, work_lat, min_lat, max_lat, min_lng, max_lng;
    real_data->cardinality = 0;
    node_t *p_req_head, *p_req_tail, *p_work_head, *p_work_tail, *p_tmp;


    while(fscanf(fp, "%ld%lf%lf%ld%lf%lf", &req_appear, &req_lng, &req_lat, &work_appear, &work_lng, &work_lat) == 6) {
#ifdef DIDI
        if (req_lat < 19.32 || req_lat > 20.05 || work_lat < 19.32 || work_lat > 20.05 ||
                req_lng < 110.10 || req_lng > 110.41 || work_lng < 110.10 || work_lng > 110.41)
            continue;
#endif
        node_t *p_req_tmp, *p_work_tmp;
        p_req_tmp = (node_t *) malloc(sizeof(node_t));
        p_req_tmp->type = 0;
        p_req_tmp->id = id;
        p_req_tmp->appear_time = req_appear;
        p_req_tmp->match_time = -1;
        p_req_tmp->x = req_lat;
        p_req_tmp->y = req_lng;
        p_req_tmp->matched_with = nullptr;
        p_req_tmp->next = nullptr;

        p_work_tmp = (node_t *) malloc(sizeof(node_t));
        p_work_tmp->type = 1;
        p_work_tmp->id = id;
        p_work_tmp->appear_time = work_appear;
        p_work_tmp->match_time = -1;
        p_work_tmp->x = work_lat;
        p_work_tmp->y = work_lng;
        p_work_tmp->matched_with = nullptr;
        p_work_tmp->next = nullptr;

        if (id == 0) {
            min_lat = min(req_lat, work_lat);
            max_lat = max(req_lat, work_lat);
            min_lng = min(req_lng, work_lng);
            max_lng = max(req_lng, work_lng);

            p_req_tmp->pre = nullptr;
            p_req_head = p_req_tmp;

            p_work_tmp->pre = nullptr;
            p_work_head = p_work_tmp;
        }

        else {
            min_lat = min(min(req_lat, work_lat), min_lat);
            max_lat = max(max(req_lat, work_lat), max_lat);
            min_lng = min(min(req_lng, work_lng), min_lng);
            max_lng = max(max(req_lng, work_lng), max_lng);

            p_req_tmp->pre = p_req_tail;
            p_req_tail->next = p_req_tmp;

            p_work_tmp->pre = p_work_tail;
            p_work_tail->next = p_work_tmp;
        }
        p_req_tail = p_req_tmp;
        p_work_tail = p_work_tmp;

        id++;
    }

    real_data->cardinality = id;
    real_data->queue_r = (node_t **) malloc(id * sizeof(node_t *));

    for (i = 0, p_tmp = p_req_head; p_tmp; p_tmp = p_tmp->next, i++)
        real_data->queue_r[i] = p_tmp;

    real_data->queue_w = (node_t **) malloc(id * sizeof(node_t *));
    for (i = 0, p_tmp = p_work_head; p_tmp; p_tmp = p_tmp->next, i++)
        real_data->queue_w[i] = p_tmp;

    fclose(fp);

    return real_data;
}
