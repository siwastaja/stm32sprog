#include "serial.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/** Data to track a serial device connection. */
struct SSerialDev {
    /** The file descriptor for the open serial device. */
    int fd;
};

static speed_t convertBaud(int baud) {
    switch(baud) {
    case 1200:   return B1200;
    case 1800:   return B1800;
    case 2400:   return B2400;
    case 4800:   return B4800;
    case 9600:   return B9600;
    case 19200:  return B19200;
    case 38400:  return B38400;
    case 57600:  return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default:     return B0;
    }
}

SerialDev *serialOpen(const char *devName, int baud) {
    assert(devName);

    speed_t localBaud = convertBaud(baud);
    if(localBaud == B0) {
        fprintf(stderr, "Baud rate \"%d\" is not supported.\n", baud);
        return NULL;
    }

    SerialDev *dev = malloc(sizeof(SerialDev));
    if(!dev) return NULL;

    dev->fd = open(devName, O_RDWR | O_NOCTTY);
    if(dev->fd == -1) {
        fprintf(stderr, "Unable to open device \"%s\"\n", devName);
        goto DeviceOpenError;
    }

    struct termios opts;
    cfmakeraw(&opts);
    opts.c_cflag &= ~CSTOPB;
    opts.c_cflag |= PARENB;
    opts.c_cflag &= ~PARODD;
    opts.c_cc[VMIN] = 0;
    opts.c_cc[VTIME] = 5;
    if(cfsetspeed(&opts, localBaud) == -1) {
        fprintf(stderr, "Unable to set baud rate.\n");
        goto DeviceConfigError;
    }
    if(tcsetattr(dev->fd, TCSANOW, &opts) == -1) {
        fprintf(stderr, "Unable to set serial device options.\n");
        goto DeviceConfigError;
    }

    return dev;

DeviceConfigError:
    close(dev->fd);
DeviceOpenError:
    free(dev);
    return NULL;
}

void serialClose(SerialDev *dev) {
    assert(dev);

    close(dev->fd);
    free(dev);
}

bool serialRead(SerialDev *dev, uint8_t *buffer, size_t n) {
    assert(dev);
    assert(buffer);

    while(n) {
        ssize_t result = read(dev->fd, buffer, n);
        if(result > 0) {
            buffer += result;
            n -= result;
        } else if(result < 0) {
            fprintf(stderr, "Read error.\n");
            return false;
        }
    }

    return true;
}

bool serialWrite(SerialDev *dev, const uint8_t *buffer, size_t n) {
    assert(dev);
    assert(buffer);

    while(n) {
        ssize_t result = write(dev->fd, buffer, n);
        if(result > 0) {
            buffer += result;
            n -= result;
        } else if(result < 0) {
            fprintf(stderr, "Write error.\n");
            return false;
        }
    }

    return true;
}

bool serialSetDtr(SerialDev *dev, bool dtr) {
    assert(dev);

    int status;
    if(ioctl(dev->fd, TIOCMGET, &status) != 0) return false;

    if(dtr) status |= TIOCM_DTR;
    else status &= ~TIOCM_DTR;

    return ioctl(dev->fd, TIOCMSET, &status) == 0;
}

