#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_RUN 8

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s BLOCK_SIZE NUM_BLOCKS\n", argv[0]);
        return 1;
    }

    long block_size = atol(argv[1]);
    long num_blocks = atol(argv[2]);
    if (block_size <= 0 || num_blocks <= 0) {
        printf("Both arguments must be greater than 0.\n");
        return 1;
    }

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/urandom");
        return 1;
    }

    unsigned char *buf = malloc(block_size);
    if (!buf) {
        printf("Out of memory.\n");
        return 1;
    }

    // Runs overlap: "aaaa" counts as two runs of 3.
    unsigned long long count[MAX_RUN + 1] = {0};
    unsigned long long total = 0;
    int prev = -1;
    int run = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (long b = 0; b < num_blocks; b++) {
        long got = 0;
        while (got < block_size) {
            ssize_t n = read(fd, buf + got, block_size - got);
            if (n <= 0) {
                perror("read");
                return 1;
            }
            got += n;
        }

        for (long i = 0; i < block_size; i++) {
            if (buf[i] == prev) {
                run++;
            } else {
                prev = buf[i];
                run = 1;
            }
            for (int k = 2; k <= MAX_RUN && k <= run; k++)
                count[k]++;
        }
        total += block_size;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double secs = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Read %llu bytes in %.2f s (%.0f MiB/s)\n\n",
           total, secs, total / secs / 1048576);
    printf("Run  Seen        Expected\n");
    for (int k = 2; k <= MAX_RUN; k++) {
        double expected = (double)total;
        for (int j = 1; j < k; j++)
            expected /= 256;
        printf("%-4d %-11llu %.2f\n", k, count[k], expected);
    }

    free(buf);
    close(fd);
    return 0;
}
