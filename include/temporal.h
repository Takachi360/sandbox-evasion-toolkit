/* include/temporal.h - Time and Signal-Based Delay Routines */
#ifndef TEMPORAL_H
#define TEMPORAL_H

#define _GNU_SOURCE // Required for ppoll execution
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/time.h>

/**
 * Standard POSIX sleep call wrapper.
 *
 * @param seconds Target pause duration in seconds.
 */
static inline void sleep_standard(unsigned int seconds) {
    sleep(seconds);
}

/**
 * Delays execution using select() I/O multiplexing timeout on empty descriptor sets.
 *
 * @param seconds Target pause duration in fractional seconds.
 */
static inline void sleep_select(double seconds) {
    struct timeval tv;
    tv.tv_sec = (time_t)seconds;
    tv.tv_usec = (suseconds_t)((seconds - tv.tv_sec) * 1e6);

    select(0, NULL, NULL, NULL, &tv);
}

/**
 * High-resolution pause using direct kernel nanosleep system calls.
 *
 * @param seconds Target pause duration in fractional seconds.
 */
static inline void sleep_nanosleep(double seconds) {
    struct timespec req, rem;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - req.tv_sec) * 1e9);

    nanosleep(&req, &rem);
}

/**
 * Signal handler callback for SIGALRM reception.
 *
 * @param signum Signal number passed by signal dispatcher.
 */
static inline void _handler_sigalrm(int signum) {
    (void)signum; // Explicitly suppress unused parameter warning
}

/**
 * Suspends process execution using POSIX alarm signals and pause system calls.
 *
 * @param seconds Delay duration before SIGALRM issuance.
 */
static inline void sleep_signal(unsigned int seconds) {
    signal(SIGALRM, _handler_sigalrm);
    alarm(seconds);
    pause();
}

/**
 * Delegates delay execution by spawning an external system subshell process.
 *
 * @param seconds Duration parameter passed to the system binary executable.
 */
static inline void sleep_subprocess_wait(unsigned int seconds) {
    char command[32];
    snprintf(command, sizeof(command), "sleep %u", seconds);

    if (system(command) == -1) {
        // Fallback sleep execution if process fork fails
        sleep(seconds);
    }
}

/**
 * Delays execution using Linux-specific ppoll high-precision timer mechanism.
 *
 * @param seconds Target pause duration in fractional seconds.
 */
static inline void sleep_ppoll(double seconds) {
    struct timespec timeout;
    timeout.tv_sec = (time_t)seconds;
    timeout.tv_nsec = (long)((seconds - timeout.tv_sec) * 1e9);

    ppoll(NULL, 0, &timeout, NULL);
}

/**
 * Configures a real-time interval timer (setitimer) to interrupt and wake thread.
 *
 * @param seconds Delay duration in fractional seconds prior to timer fire.
 */
static inline void sleep_itimer(double seconds) {
    signal(SIGALRM, _handler_sigalrm);

    struct itimerval val;
    val.it_interval.tv_sec = 0;
    val.it_interval.tv_usec = 0;

    val.it_value.tv_sec = (time_t)seconds;
    val.it_value.tv_usec = (suseconds_t)((seconds - val.it_value.tv_sec) * 1e6);

    setitimer(ITIMER_REAL, &val, NULL);
    pause();
}

#endif // TEMPORAL_H
