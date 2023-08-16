#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>

#include <stdbool.h>

#include "uart_io_api.h"
#include <stdio.h>

#if !defined(_SERIAL_DATAGRAM_DEVICE_NAME)
#define _SERIAL_DATAGRAM_DEVICE_NAME "/dev/ttyHS1"
#endif

static int device_fd = -1;

// init the UART
// parameters: NONE
// return value:
//   true if done; otherwise failed
bool init_uart_io_api(void)
{
    struct termios tty_attr;

    device_fd = open(_SERIAL_DATAGRAM_DEVICE_NAME, O_RDWR);

    memset(&tty_attr, 0, sizeof(struct termios));
    tty_attr.c_iflag = IGNPAR;
    tty_attr.c_cflag = B115200 | HUPCL | CS8 | CREAD | CLOCAL;
    tty_attr.c_cc[VMIN] = 1;

    if (tcsetattr(device_fd, TCSANOW, &tty_attr) < 0) {
        close(device_fd);
        device_fd = -1;
    }

    tcflush(device_fd, TCIOFLUSH);

    return device_fd >= 0;

}

// receive one byte from UART
// parameters:
//  [out]byte, ptr to store the received byte
//  [in] delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_recv_byte(uint8_t *byte, const uint32_t delay)
{
    (void)delay;
    for(;;) {
        ssize_t read_ret = read(device_fd, byte, 1);
        if (read_ret == 1) {
            return true;
        }
        perror("read uart");
        if (errno == EINTR || errno == EAGAIN) {
            usleep(1000);
            continue;
        }
        return false;
    }
}

// send data into UART
// parameters:
//  [in]buf, ptr to store the data
//  [in]size, size of the 'buf'
//  [in]delay, max time tick before the operation
// return value:
//   true if done; otherwise failed
bool uart_send_data(const uint8_t *buf, size_t size, const uint32_t delay)
{
    (void)delay;

    if (device_fd < 0) {
        return false;
    }

    size_t written = 0;
    for(;;) {
        ssize_t written_once = write(device_fd, buf + written, size - written);
        if (written < 0) {
            return false;
        }
        written += written_once;
        if (written == size) {
            return true;
        }
    }
}
