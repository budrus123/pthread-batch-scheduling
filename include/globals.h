// #include "performance.h"

#define JOB_BUF_SIZE 50

int test_mode = 0;

struct Perf_info performance_metrics;



struct job new_job;
struct job running_job;

int job_q_index_location = -1;

pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_mutex_t completed_job_queue_lock;  /* Lock for critical sections */
pthread_mutex_t new_job_job_lock;  /* Lock for critical sections */
pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */