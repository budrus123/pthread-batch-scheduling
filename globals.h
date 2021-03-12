// #include "performance.h"

#define JOB_BUF_SIZE 20

int test_mode = 0;

int head = 0;
int tail = 0;

struct Perf_info performance_metrics;

struct job job_queue[JOB_BUF_SIZE];
struct job completed_jobs[JOB_BUF_SIZE * 10];

struct job new_job;
struct job running_job;

int job_q_index_location = -1;
int completed_job_index = 0;
int currently_executing = 0;

pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_mutex_t completed_job_queue_lock;  /* Lock for critical sections */
pthread_mutex_t new_job_job_lock;  /* Lock for critical sections */
pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */