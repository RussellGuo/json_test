#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <linux/mtkfb.h>

#define SEND_CMD_NUM 15

typedef struct __baudrate_mpping{
    unsigned int ul_baud_rate;
    speed_t linux_baud_rate;
}BAUD_RATE_SETTING;

static BAUD_RATE_SETTING speeds_mapping[] = {
    {0      ,B0      },
    {50     ,B50     },
    {75     ,B75     },
    {110    ,B110    },
    {134    ,B134    },
    {150    ,B150    },
    {200    ,B200    },
    {300    ,B300    },
    {600    ,B600    },
    {1200   ,B1200   },
    {1800   ,B1800   },
    {2400   ,B2400   },
    {4800   ,B4800   },
    {9600   ,B9600   },
    {19200  ,B19200  },
    {38400  ,B38400  },
    {57600  ,B57600  },
    {115200 ,B115200 },
    {230400 ,B230400 },
    {460800 ,B460800 },
    {500000 ,B500000 },
    {576000 ,B576000 },
    {921600 ,B921600 },
    {1000000,B1000000},
    {1152000,B1152000},
    {1500000,B1500000},
    {2000000,B2000000},
    {2500000,B2500000},
    {3000000,B3000000},
    {3500000,B3500000},
    {4000000,B4000000},
};

static speed_t get_speed(unsigned int baudrate)
{
    unsigned int idx;
    for (idx = 0; idx < sizeof(speeds_mapping)/sizeof(speeds_mapping[0]); idx++){
        if (baudrate == (unsigned int)speeds_mapping[idx].ul_baud_rate){
            return speeds_mapping[idx].linux_baud_rate;
        }
    }
    return CBAUDEX;
}

int set_uart_parm(int fd, unsigned int new_baudrate, int length, char parity_c, int stopbits)
{
    struct termios uart_cfg_opt;
    speed_t speed;
    char  using_custom_speed = 0;

    printf("[uart_debug] set_uart_parm\n");

    if(-1==fd)
        return -1;

    /* Get current uart configure option */
    if(-1 == tcgetattr(fd, &uart_cfg_opt))
        return -1;

    tcflush(fd, TCIOFLUSH);
    (void)new_baudrate;
    /* Baud rate setting section */
    speed = get_speed(new_baudrate);
    if(CBAUDEX != speed){
        /*set standard buadrate setting*/
        cfsetospeed(&uart_cfg_opt, speed);
        cfsetispeed(&uart_cfg_opt, speed);
        printf("[uart_debug] Standard baud\r\n");
    }else{
        printf("[uart_debug] Custom baud\r\n");
        using_custom_speed = 1;
    }
    /* Apply baudrate settings */
    if(-1==tcsetattr(fd, TCSANOW, &uart_cfg_opt))
        return -1;

    /* Set time out */
    uart_cfg_opt.c_cc[VTIME] = 1;
    uart_cfg_opt.c_cc[VMIN] = 0;

    /* Data length setting section */
    uart_cfg_opt.c_cflag &= ~CSIZE;
    switch(length)
    {
        default:
        case 8:
            uart_cfg_opt.c_cflag |= CS8;
            break;
        case 5:
            uart_cfg_opt.c_cflag |= CS5;
            break;
        case 6:
            uart_cfg_opt.c_cflag |= CS6;
            break;
        case 7:
            uart_cfg_opt.c_cflag |= CS7;
            break;
    }

    /* Parity setting section */
    uart_cfg_opt.c_cflag &= ~(PARENB|PARODD);
    switch(parity_c)
    {
        default:
        case 'N':
        case 'n':
            uart_cfg_opt.c_iflag &= ~INPCK;
            break;
        case 'O':
        case 'o':
            uart_cfg_opt.c_cflag |= (PARENB|PARODD);
            uart_cfg_opt.c_iflag |= INPCK;
            break;
        case 'E':
        case 'e':
            uart_cfg_opt.c_cflag |= PARENB;
            uart_cfg_opt.c_iflag |= INPCK;
            break;
    }

    /* Stop bits setting section */
    if(2==stopbits)
        uart_cfg_opt.c_cflag |= CSTOPB;
    else
        uart_cfg_opt.c_cflag &= ~CSTOPB;

    /* Using raw data mode */
    uart_cfg_opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    uart_cfg_opt.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    uart_cfg_opt.c_oflag &=~(INLCR|IGNCR|ICRNL);
    uart_cfg_opt.c_oflag &=~(ONLCR|OCRNL);

    /* Apply new settings */
    if(-1==tcsetattr(fd, TCSANOW, &uart_cfg_opt))
        return -1;

    tcflush(fd,TCIOFLUSH);

    /* All setting applied successful */
    printf("[uart_debug] setting apply done\n");
    printf("[uart_debug] nSpeed = %d,nBits = %d,nEvent = %c,nStop = %d\n",new_baudrate,length,parity_c,stopbits); 
    return 0;
}


int main(int argc, char **argv)
{
    int Fd = 0,i = 0;
    int length = 0,ret = -1;
    unsigned char send_test_cmd[SEND_CMD_NUM] = {0};
    unsigned char recv_resp[SEND_CMD_NUM] = {0};
    send_test_cmd[0] = 0xa0;
    send_test_cmd[1] = 0x03;
    send_test_cmd[2] = 0x70;
    send_test_cmd[3] = 0xea;
    printf("[uart_debug] Start\n");

    Fd = open( "/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if (Fd == -1){
        printf("[uart_debug] open /dev/ttyS1 fail\n");
        return -1;
    }
    printf("[uart_debug] open /dev/ttyS1 success\n");

    ret = set_uart_parm(Fd, 115200, 8, 'N', 1);
    if(ret == -1){
       printf("[uart_debug] set uart parm fail\n");
       return -1;
    }
    printf("[uart_debug] set uart parm success\n");

    length = strlen(send_test_cmd);
    ret = write(Fd, send_test_cmd, length);
    if (ret < 0){
        printf("[uart_debug] send cmd fail,ret = %d\n",ret);
    }else
        printf("[uart_debug] send cmd successful,ret = %d\n",ret);

    memset(recv_resp, 0, sizeof(recv_resp));
    while(1){

        printf("[uart_debug] recv read\n");
        ret = read(Fd, &recv_resp[0], SEND_CMD_NUM);
        usleep(500000);
        if(ret > 0){
            printf("[uart_debug] %s,ret = %d\n",recv_resp,ret);
            break;
        }
    }
    close(Fd);
    printf("[uart_debug] end\n");
    return 0;
}
