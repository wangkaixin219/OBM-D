
#ifndef DELAY_MATCHING_V2_DEF_H
#define DELAY_MATCHING_V2_DEF_H

#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>

#define OPT     0
#define QL      1
#define RQL     2
#define CCLO    3

#ifdef OLIST
#define rad(x)  (x) * 3.1415926 / 180
#define R       6378.137
#endif


using namespace std;

typedef struct config {
    int dataset_type, setting_type;
    int map_size, cardinality, t_max;
    int bin_size, step_size, act_size;
    string dir_name;
} config_t;

typedef struct node {
    int type, id;
    long appear_time, match_time, timer;
    double x, y;
    node *pre, *next, *matched_with;
    bool checked;
} node_t;

typedef struct data {
    int cardinality, step_size;
    node_t **queue_r, **queue_w;
} data_t;

typedef struct extreme_pair {
    node_t *r, *w;
    double mmd;
} ext_p;

typedef struct queue_node {
    node_t *q_node;
    int prev;
} queue_node_t;

typedef struct context {
    int req_num, work_num, step_size;
    node_t *req_head, *req_tail, *work_head, *work_tail;
    ext_p ext;
} context_t;

typedef struct result {
    double bott_v, running_time, memory_r, memory_m;
} res_t;

inline double dist(node_t* req_node, node_t* work_node) {

    double x_diff, y_diff;
    x_diff = req_node->x - work_node->x;
    y_diff = req_node->y - work_node->y;
    return sqrt(x_diff * x_diff + y_diff * y_diff);

#ifdef DIDI
    double req_lat = rad(req_node->x), work_lat = rad(work_node->x);
    double req_lng = rad(req_node->y), work_lng = rad(work_node->y);

    double lat_diff = req_lat - work_lat, lng_diff = req_lng - work_lng;
    double theta = sqrt(sin(lat_diff / 2.0) * sin(lat_diff / 2.0) + cos(req_lat) * cos(work_lat) * sin(lng_diff / 2.0) * sin(lng_diff / 2.0));
    return R * 2 * asin(theta) * 90;
#endif

#ifdef OLIST
    double req_lat = rad(req_node->x), work_lat = rad(work_node->x);
    double req_lng = rad(req_node->y), work_lng = rad(work_node->y);

    double lat_diff = req_lat - work_lat, lng_diff = req_lng - work_lng;
    double theta = sqrt(sin(lat_diff / 2.0) * sin(lat_diff / 2.0) + cos(req_lat) * cos(work_lat) * sin(lng_diff / 2.0) * sin(lng_diff / 2.0));
    return R * 2 * asin(theta) * 360;
#endif

}

inline void tick(context_t *context, long time_step, long duration) {
    while (context->req_tail != nullptr && context->req_tail->appear_time < (long) (time_step + duration)) {
        context->req_num++;
        context->req_tail = context->req_tail->next;
    }
    while (context->work_tail != nullptr && context->work_tail->appear_time < (long) (time_step + duration)) {
        context->work_num++;
        context->work_tail = context->work_tail->next;
    }
}


inline void GetCurTime(struct rusage* curTime) {
    int ret = getrusage(RUSAGE_SELF, curTime);
    if (ret != 0) {
        fprintf(stderr, "The running time info couldn't be collected successfully.\n");
        exit(0);
    }
}

// Unit: ms
inline void GetTime(struct rusage* timeStart, struct rusage* timeEnd, double* userTime, double* sysTime) {
    *userTime = ((float)(timeEnd->ru_utime.tv_sec - timeStart->ru_utime.tv_sec)) * 1e3 + 
        ((float)(timeEnd->ru_utime.tv_usec - timeStart->ru_utime.tv_usec)) * 1e-3;
    *sysTime = ((float)(timeEnd->ru_stime.tv_sec - timeStart->ru_stime.tv_sec)) * 1e3 + 
        ((float)(timeEnd->ru_stime.tv_usec - timeStart->ru_stime.tv_usec)) * 1e-3;
}


#endif //DELAY_MATCHING_V2_DEF_H
