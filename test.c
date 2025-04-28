#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

int serial_init(const char* dev_name){
    int fd = open(dev_name, O_RDWR | O_NOCTTY);
    if(fd == -1){
        perror("open serial port");
        return -1;
    }

    struct termios options;

    tcgetattr(fd, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE; 
    options.c_cflag |= CS8;     // 8 data bits
    options.c_cflag &= ~CRTSCTS; // No hardware flow control
    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, Ignore modem control lines

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    options.c_oflag &= ~OPOST; // Raw output

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int serial_write(int fd, const char* str, ssize_t size){
    int n = write(fd, str, size);
    if(n < 0){
        perror("write");
    }

    return n;
}

int serial_receive(int fd, char* receive_buffer, ssize_t size){
    char *buf_ptr = receive_buffer;
    int n = 0;
    ssize_t sum = 0;

    while(1){
        n = read(fd, buf_ptr, 10);
        buf_ptr += n;
        sum += n;

        // printf("received bytes: %d, sum:%ld \r\n", n,sum);
        // printf("current ptr: %c\r\n", *(buf_ptr-1));

        if(sum >= size)
        {
            break;
        }
        
        if(*(buf_ptr-1) == '\n' || *(buf_ptr-1) == '\r'){
            break;
        }
    }

    return sum;
}

int main() {
    int fd = serial_init("/dev/serial0");
    if(fd == -1){
        perror("init error");
    }
    // 发送数据
    const char *send_str = "Hello, serial loopback!\n";
    int count = serial_write(fd, send_str, strlen(send_str));
    printf("Sent %d bytes: %s\n", count, send_str);

    char buf[128] = {0};
    int sum = serial_receive(fd, buf, sizeof(buf)-1);
    printf("Received %d bytes: %s\n", sum, buf);

    close(fd);
    return 0;
}

