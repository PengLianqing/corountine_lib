/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       wrap.h
  * @brief      socket封装
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-1-2021      Peng            1. 完成
  *
  @verbatim
  ==============================================================================
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Peng****************************
  */
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

namespace msocket
{
    void perr_exit(const char *s)
    {
        perror(s);
        exit(-1);
    }

    int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
    {
        int n;

    again:
        if ((n = accept(fd, sa, salenptr)) < 0) {
            if ((errno == ECONNABORTED) || (errno == EINTR))
                goto again;
            else
                perr_exit("accept error");
        }
        return n;
    }

    int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
    {
        int n;

        if ((n = bind(fd, sa, salen)) < 0)
            perr_exit("bind error");

        return n;
    }

    int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
    {
        int n;

        if ((n = connect(fd, sa, salen)) < 0)
            perr_exit("connect error");

        return n;
    }

    int Listen(int fd, int backlog)
    {
        int n;

        if ((n = listen(fd, backlog)) < 0)
            perr_exit("listen error");

        return n;
    }

    int Socket(int family, int type, int protocol)
    {
        int n;

        if ((n = socket(family, type, protocol)) < 0)
            perr_exit("socket error");

        return n;
    }

    ssize_t Read(int fd, void *ptr, size_t nbytes)
    {
        ssize_t n;

    again:
        if ( (n = read(fd, ptr, nbytes)) == -1) {
            if (errno == EINTR)
                goto again;
            else
                return -1;
        }
        return n;
    }

    ssize_t Write(int fd, const void *ptr, size_t nbytes)
    {
        ssize_t n;

    again:
        if ( (n = write(fd, ptr, nbytes)) == -1) {
            if (errno == EINTR)
                goto again;
            else
                return -1;
        }
        return n;
    }

    int Close(int fd)
    {
        int n;
        if ((n = close(fd)) == -1)
            perr_exit("close error");

        return n;
    }

}

