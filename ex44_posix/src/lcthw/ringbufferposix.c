//
// @Author ZhaoZelong
// @Date 2024/07/26
// @Email zhaozelong@leagsoft.com
// @Copyright (c) 2024 LeagSoft Inc.
// @Description
//

#include "ringbufferposix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>

#include "dbg.h"


RingBuffer *RingBuffer_create(unsigned long order) {
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));

    char path[] = "/tmp/ring-buffer-XXXXXX";
    int fd;
    int state;
    void *addr;

    fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }
    state = unlink(path);

    if (state < 0) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    buffer->length = 1UL << order;
    buffer->start = 0;
    buffer->end = 0;

    state = ftruncate(fd, buffer->length);
    if (state < 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    buffer->buffer = mmap(NULL, buffer->length << 1,PROT_NONE,MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (buffer->buffer == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    addr = mmap(buffer->buffer, buffer->length, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    if (addr != buffer->buffer) {
        perror("mmap1");
        exit(EXIT_FAILURE);
    }

    addr = mmap(buffer->buffer + buffer->length,
                buffer->length, PROT_READ | PROT_WRITE,
                MAP_FIXED | MAP_SHARED, fd, 0);

    if (addr != buffer->buffer + buffer->length) {
        perror("mmap2");
        exit(EXIT_FAILURE);
    }

    state = close(fd);
    if (state < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    return buffer;
}

void RingBuffer_destroy(RingBuffer *buffer) {
    int state = munmap(buffer->buffer, buffer->length << 1);
    if (state == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
}

void RingBuffer_commit_read(RingBuffer *buffer,unsigned long count) {
    buffer->start += count;
    if(buffer->start >= buffer->length){
        buffer->start -= buffer->length;
        buffer->end -= buffer->length;
    }
}

void RingBuffer_commit_write(RingBuffer *buffer,unsigned long count) {
    buffer->end += count;
}

void RingBuffer_clear(RingBuffer *buffer) {
    buffer->start = 0;
    buffer->end = 0;
}

unsigned long RingBuffer_count_bytes(RingBuffer *buffer) {
    return buffer->end - buffer->start;
}

unsigned long RingBuffer_count_free_bytes(RingBuffer *buffer) {
    return buffer->length - RingBuffer_count_bytes (buffer);
}

unsigned long RingBuffer_read(RingBuffer *buffer, char *target, unsigned long amount) {
    unsigned long countBytes = RingBuffer_count_bytes(buffer);
    if(countBytes >= amount){
        void *result = memcpy(target,RingBuffer_starts_at(buffer), amount);
        check(result != NULL, "Failed to write buffer into data.");
        RingBuffer_commit_read(buffer, amount);
        return amount;
    }else if(countBytes > 0){
        void *result = memcpy(target, RingBuffer_starts_at(buffer), countBytes);
        check(result != NULL, "Failed to write buffer into data.");
        RingBuffer_commit_read(buffer, countBytes);
        return countBytes;
    }
    error:
    return -1;
}

unsigned long RingBuffer_write(RingBuffer *buffer, char *data, unsigned long length) {
    unsigned long freeBytes = RingBuffer_count_free_bytes(buffer);

    check(freeBytes >= length,"Failed to write into the buffer due to no space");
    void *result = memcpy(RingBuffer_ends_at(buffer), data, length);
    check(result != NULL, "Failed to write data into buffer.");

    RingBuffer_commit_write(buffer, length);
    return length;

    error:
    return -1;
}

bstring RingBuffer_gets(RingBuffer *buffer, unsigned long amount)
{
    check(amount > 0, "Need more than 0 for gets, you gave: %lu ", amount);
    check_debug(amount <= RingBuffer_count_bytes(buffer),
                "Not enough in the buffer.");

    bstring result = blk2bstr(RingBuffer_starts_at(buffer), (int)amount);
    check(result != NULL, "Failed to create gets result.");
    check(blength(result) == (int)amount, "Wrong result length.");

    RingBuffer_commit_read(buffer, amount);
    assert(RingBuffer_count_free_bytes(buffer) >= 0 && "Error in read commit.");

    return result;
    error:
    return NULL;
}