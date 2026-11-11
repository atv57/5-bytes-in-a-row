#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_alg.h>

int main(void)
{
    struct sockaddr_alg sa = {
        .salg_family = AF_ALG,
        .salg_type = "rng",
        .salg_name = "jitterentropy_rng",
    };
    unsigned char buf[32];

    int tfm = socket(AF_ALG, SOCK_SEQPACKET, 0);
    if (tfm < 0) {
        perror("socket");
        return 1;
    }
    if (bind(tfm, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind");
        return 1;
    }

    int fd = accept(tfm, NULL, 0);
    if (fd < 0) {
        perror("accept");
        return 1;
    }

    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        printf("jent_read: error %d (%s)\n", errno, strerror(errno));
        return 1;
    }

    printf("jent_read: ok, got %zd bytes:", n);
    for (int i = 0; i < 8; i++)
        printf(" %02x", buf[i]);
    printf(" ...\n");
    return 0;
}
