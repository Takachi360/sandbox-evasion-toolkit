/* include/synchronization.h - Thread and Process Synchronization Delay Routines */
#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>

/**
 * Auxiliary structure for passing synchronization parameters to worker threads.
 */
typedef struct {
    int seconds;
    sem_t *sem;
    pthread_cond_t *cv;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    int *ready;
} thread_args_t;

// --- WORKER ROUTINES ---

static inline void* _worker_join(void* arg) {
    int sec = *(int*)arg;
    sleep(sec);
    return NULL;
}

static inline void* _queue_producer_worker(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    sleep(args->seconds);
    pthread_mutex_lock(args->mutex);
    *(args->ready) = 1;
    pthread_cond_signal(args->cv);
    pthread_mutex_unlock(args->mutex);
    return NULL;
}

static inline void* _releaser(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    for (int i = 0; i < args->seconds; i++) {
        sleep(1);
        sem_post(args->sem);
    }
    return NULL;
}

static inline void* _reach_barrier(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    sleep(1);
    pthread_barrier_wait(args->barrier);
    return NULL;
}

static inline void* _notifier(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    sleep(args->seconds);
    pthread_mutex_lock(args->mutex);
    *(args->ready) = 1;
    pthread_cond_broadcast(args->cv);
    pthread_mutex_unlock(args->mutex);
    return NULL;
}

// --- SYNCHRONIZATION DELAY TECHNIQUES ---

/**
 * Delays execution by creating a POSIX thread and waiting for its termination via pthread_join.
 *
 * @param seconds Target sleep duration in seconds for the child thread.
 */
static inline void delay_thread_join(int seconds) {
    pthread_t t;
    pthread_create(&t, NULL, _worker_join, &seconds);
    pthread_join(t, NULL);
}

/**
 * Simulates producer-consumer queue blocking using a mutex condition variable.
 *
 * @param seconds Duration in seconds before the producer signals the condition variable.
 */
static inline void delay_queue_blocking(int seconds) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int data_ready = 0;
    pthread_t t;
    thread_args_t args = { .seconds = seconds, .mutex = &mutex, .cv = &cond, .ready = &data_ready };

    pthread_create(&t, NULL, _queue_producer_worker, &args);

    pthread_mutex_lock(&mutex);
    while (!data_ready) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_join(t, NULL);
}

/**
 * Delays execution by spawning a child process that sleeps before writing to an IPC pipe.
 *
 * @param seconds Duration in seconds the child process sleeps prior to signaling the pipe.
 */
static inline void delay_pipe_wait(int seconds) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return;
    pid_t pid = fork();

    if (pid == 0) { // Child process
        close(pipefd[0]);
        sleep(seconds);
        if (write(pipefd[1], "DONE", 4) < 0) {
            // Write failure fallback
        }
        close(pipefd[1]);
        exit(0);
    } else if (pid > 0) { // Parent process
        close(pipefd[1]);
        char buf[5];
        if (read(pipefd[0], buf, 4) < 0) {
            // Read failure fallback
        }
        close(pipefd[0]);
        wait(NULL); // Reap child process
    }
}

/**
 * Delays execution by consuming semaphore permits posted periodically by a background worker thread.
 *
 * @param count Number of semaphore post/wait cycles to process.
 */
static inline void delay_semaphore_race(int count) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    pthread_t t;
    thread_args_t args = { .seconds = count, .sem = &sem };

    pthread_create(&t, NULL, _releaser, &args);

    for (int i = 0; i < count; i++) {
        sem_wait(&sem);
    }
    pthread_join(t, NULL);
    sem_destroy(&sem);
}

/**
 * Blocks the current thread using timedwait on a condition variable until the absolute timeout expires.
 *
 * @param seconds Timeout delay in seconds.
 */
static inline void delay_event_wait(int seconds) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;

    pthread_mutex_lock(&mutex);
    pthread_cond_timedwait(&cond, &mutex, &ts);
    pthread_mutex_unlock(&mutex);
}

/**
 * Coordinates multiple worker threads to synchronize at a POSIX execution barrier.
 *
 * @param num_threads Total number of threads participating in the barrier wait.
 */
static inline void delay_barrier(int num_threads) {
    if (num_threads < 2) return;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_threads);
    pthread_t *threads = (pthread_t*)malloc((num_threads - 1) * sizeof(pthread_t));
    if (!threads) {
        pthread_barrier_destroy(&barrier);
        return;
    }

    thread_args_t args = { .barrier = &barrier };

    for (int i = 0; i < num_threads - 1; i++) {
        pthread_create(&threads[i], NULL, _reach_barrier, &args);
    }

    pthread_barrier_wait(&barrier);

    for (int i = 0; i < num_threads - 1; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    pthread_barrier_destroy(&barrier);
}

/**
 * Blocks execution on a condition variable until a delayed broadcast signal is dispatched.
 *
 * @param seconds Delay duration in seconds before the worker triggers condition broadcast.
 */
static inline void delay_condition_var(int seconds) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    int ready = 0;
    pthread_t t;
    thread_args_t args = { .seconds = seconds, .mutex = &mutex, .cv = &cv, .ready = &ready };

    pthread_create(&t, NULL, _notifier, &args);

    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cv, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_join(t, NULL);
}

#endif // SYNCHRONIZATION_H
