#include <stdio.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int ttyfd = open(argv[1], O_RDWR);

    fd_set read_set;
    fd_set exc_set;

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 3;

    struct timespec tp;
    struct timespec last_tp;

    while (1) {
        FD_ZERO(&read_set);
        FD_SET(ttyfd, &read_set);
        FD_SET(ttyfd, &exc_set);
        const int n = select(ttyfd + 1, &read_set, NULL, &exc_set, &timeout);

        if (n == -1) {
            perror("select");
            return __LINE__;
        }

        if (n < 1) {
            continue;
        }

        if (FD_ISSET(ttyfd, &read_set)) {
            char b;
            int n = read(ttyfd, &b, 1);
            if (n == -1) {
                perror("read");
                return __LINE__;
            }
            if (n > 0 && b == 'U') {
                if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
                    perror("clock_gettime");
                    return __LINE__;
                }

                time_t diff_sec = tp.tv_sec - last_tp.tv_sec;
                long   diff_nsec = tp.tv_nsec - last_tp.tv_nsec;
                last_tp = tp;

                if (diff_nsec < 0) {
                    diff_sec -= 1;
                    diff_nsec += 1000000000L;
                }

                /* printf("tick tock: %lds, %ldns\n", tp.tv_sec, tp.tv_nsec); */
                printf("tick tock: %lds, %ldns\n", diff_sec, diff_nsec);

                b = ~b;
                write(ttyfd, &b, 1);
            }
        }

        if (FD_ISSET(ttyfd, &exc_set)) {
            fprintf(stderr, "exception on file descriptor");
            return __LINE__;
        }
    }

    return 0;
}
