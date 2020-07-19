#include <iostream>
#include "data.h"
#include "q_learning.h"
#include "opt.h"
 #include "restricted_q_learning.h"
#include "cclo.h"
#include "mmdh.h"

res_t q_res;
res_t restricted_q_res;
res_t opt_res;
res_t cclo_res;
res_t mmdh_res;

config_t* parse_config(int argc, const char **argv) {

    config_t * config = (config_t *) malloc(sizeof(config_t));

#if !defined(DIDI) && !defined(OLIST)
    if (argc != 5) {
        fprintf(stderr, "Usage error.\n");
        exit(0);
    }
    config->cardinality = atoi(argv[1]);
    config->t_max = atoi(argv[2]);
    config->setting_type = atoi(argv[3]);
    if (config->setting_type != 0 && config->setting_type != 1 && config->setting_type != 2) {
        fprintf(stderr, "Usage error.\n");
        fprintf(stderr, "Wrong distribution type.\n");
        exit(0);
    }
#else
    if (argc != 2) {
        fprintf(stderr, "Usage error.\n");
        exit(0);
    }
    config->dir_name = argv[1];
#endif

    return config;
}

void free_config(config_t *config) {
    delete config;
    config = nullptr;
}

void train_q_values_syn(config_t *config) {
    for (int episode = 0; episode < 10000; ++episode) {
        data_t *syn = generate_data(config);
        train_q_values(syn, episode, 0);
        free_data_mem(syn);
    }
}

void train_restricted_q_values_syn(config_t *config) {
    for (int episode = 0; episode < 10000; ++episode) {
        data_t *syn = generate_data(config);
        train_restricted_q_values(syn, episode);
        free_data_mem(syn);
    }
}

void train_q_values_real(const char* train_file) {
    for (int episode = 0; episode < 10000; ++episode) {
        data *train_data = read_data_from_file(train_file);
        train_q_values(train_data, episode, 0);
        free_data_mem(train_data);
    }
}

void train_restricted_q_values_real(const char* train_file) {
    for (int episode = 0; episode < 10000; ++episode) {
        data *train_data = read_data_from_file(train_file);
        train_restricted_q_values(train_data, episode);
        free_data_mem(train_data);
    }
}

int main(int argc, const char **argv) {

    config_t *configs = parse_config(argc, argv);
    data_t *test_data;
    init_q_values(configs);
    init_restricted_q_values();
    
    if (configs->dataset_type == 0) {
        train_q_values_syn(configs);
        train_restricted_q_values_syn(configs);
        test_data = generate_data(configs);
    }
    else {
        string train_file = configs->dir_name + "/train.txt";
        string test_file = configs->dir_name + "/test.txt";
        train_q_values_real(train_file.c_str());
        train_restricted_q_values_real(train_file.c_str());
        test_data = read_data_from_file(test_file.c_str());
    }

    q_learning(test_data);
    printf("q_learning: %lf, %lf ms, %lf MB\n", q_res.bott_v, (double) q_res.running_time, (double) q_res.memory_m / (1 << 20));
    reset_data(test_data);

    restricted_q_learning(test_data);
    printf("restricted q_learning: %lf, %lf ms, %lf MB\n", restricted_q_res.bott_v, (double) restricted_q_res.running_time, (double) restricted_q_res.memory_m / (1 << 20));
    reset_data(test_data);

    cclo(test_data);
    printf("cclo: %lf, %lf ms, %lf MB\n", cclo_res.bott_v, (double) cclo_res.running_time, (double) cclo_res.memory_m / (1 << 20));
    reset_data(test_data);

    mmdh(test_data);
    printf("mmdh: %lf, %lf ms, %lf MB\n", mmdh_res.bott_v, (double) mmdh_res.running_time, (double) mmdh_res.memory_m / (1 << 20));
    reset_data(test_data);

    opt(test_data);
    printf("opt: %lf, %lf ms, %lf MB\n", opt_res.bott_v, (double) (opt_res.running_time), (double) opt_res.memory_m / (1 << 20));
    reset_data(test_data);

    free_data_mem(test_data);
    release_q_values();
    free_config(configs);
    
    return 0;
}
