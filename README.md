# Linux_Serial

## 遇到的资源管理bug

在串口外设的使用时，要注意**“什么时候open”，“什么时候close”**

不能每执行一次操作就open&close一次

举例说明原因：

1. 当init完成后，马上close，相当于白init一次，无效
2. 在init完成后close，进行read操作，将会导致“无法打开该文件”等报错

**正确操作：**

init函数的返回值应当是文件描述符fd，之后的函数就全程使用这个描述符fd进行读写操作

```c
int serial_init(const char* dev_name){
    int fd = open(dev_name, O_RDWR | O_NOCTTY);
    if(fd == -1){
        perror("open serial port");
        return -1;
    }

    // 配置termios...

    return fd;
}
```

之后函数便使用这个fd进行各类操作
