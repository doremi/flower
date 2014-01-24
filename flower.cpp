#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <stdint.h>
#include <pthread.h>

#define UART_DEV "/dev/ttyUSB0"

int Uart_fd = -1;
fd_set fds;

void setup_uart(speed_t baud_rate)
{
    int ret = 0;
    struct termios termios_now;
    struct termios termios_new;
    struct termios termios_chk;

    if (Uart_fd > 0) {
        memset(&termios_now, 0x0, sizeof(struct termios));
        memset(&termios_new, 0x0, sizeof(struct termios));
        memset(&termios_chk, 0x0, sizeof(struct termios));
    }

    tcflush(Uart_fd, TCIOFLUSH);
    tcgetattr(Uart_fd, &termios_new);
    termios_new.c_iflag &=
            ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL |
            IXON);
    termios_new.c_oflag &= ~OPOST;
    termios_new.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    termios_new.c_cflag &= ~(CSIZE | PARENB);
    termios_new.c_cflag |= CS8;
    termios_new.c_cflag &= ~CRTSCTS;
    termios_new.c_cc[VTIME] = 0;        /* Over than VTIME*0.1 second(s), read operation will return from blocked */
    termios_new.c_cc[VMIN] = 1; /* Over than VMIN bytes received, read operation will return from blocked */
    tcsetattr(Uart_fd, TCSANOW, &termios_new);
    tcflush(Uart_fd, TCIOFLUSH);
    tcsetattr(Uart_fd, TCSANOW, &termios_new);
    tcflush(Uart_fd, TCIOFLUSH);
    tcflush(Uart_fd, TCIOFLUSH);
    cfsetospeed(&termios_new, baud_rate);
    cfsetispeed(&termios_new, baud_rate);
    tcsetattr(Uart_fd, TCSANOW, &termios_new);

    if (ret == 0) {
        /* Write back finished, so read the settings again to check the content */
        ret = tcgetattr(Uart_fd, &termios_chk);
        if (ret == 0) {
            if (termios_new.c_cflag != termios_chk.c_cflag) {
                printf("Failed to set the parameters of termios\n");
            } else {
                printf("Set the paremeters successfully\n");
                ret = 0;
            }
        }
    }
}

int open_uart()
{
    Uart_fd = open(UART_DEV, O_RDWR);
    if (Uart_fd < 0) {
        printf("Failed(%d:%s) to open uart(%s).", errno, strerror(errno), UART_DEV);
        return -1;
    }
    FD_ZERO(&fds);
    FD_SET(Uart_fd, &fds);
    return 0;
}

void close_uart()
{
    close(Uart_fd);
}

void *recvLoop(void *arg)
{
    int ret = 0;
    uint8_t buffer[4096];
    ssize_t read_size = 0;

    while (true) {
        ret = select(Uart_fd+1, &fds, NULL, NULL, NULL);
        if (ret == -1) {
            printf("recv: %s", strerror(errno));
            break;
        } else if (ret) {
            read_size = read(Uart_fd, buffer, sizeof(buffer));
            if (read_size < 0) {
                printf("Failed(%d:%s) to read from uart\n", errno,
                         strerror(errno));
            }
            buffer[read_size] = '\0';
            printf("%s", buffer);
        } else {
            printf("no data\n");
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    char cmd[2] = "";
    pthread_t tid;
    int duration = 1;

    if (argc < 2) {
        printf("Usage: %s [0-7] [duration, default 1]\n", argv[0]);
        exit(-1);
    }

    cmd[0] = argv[1][0];

    if (argc == 3) {
        duration = atoi(argv[2]);
    }

    open_uart();
    setup_uart(B9600);

    pthread_create(&tid, NULL, recvLoop, NULL);

    sleep(2);
    write(Uart_fd, &cmd[0], 1);
    sleep(duration);

    pthread_cancel(tid);

    close_uart();
    return 0;
}
