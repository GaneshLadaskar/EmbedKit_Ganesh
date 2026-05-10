#include <stdio.h>
#include <stdint.h>

#define BUF_SIZE 8
#define BUF_MASK (BUF_SIZE - 1)

#define RB_OK 0
#define RB_FULL -1
#define RB_EMPTY -2

typedef struct {
    uint8_t buf[BUF_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} RingBuf;

void rb_init(RingBuf *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

uint8_t rb_full(RingBuf *rb)
{
    return rb->count == BUF_SIZE;
}

uint8_t rb_empty(RingBuf *rb)
{
    return rb->count == 0;
}

uint8_t rb_count(RingBuf *rb)
{
    return rb->count;
}

int rb_write(RingBuf *rb, uint8_t byte)
{
    if (rb_full(rb)) {
        return RB_FULL;
    }

    rb->buf[rb->head] = byte;
    /*
     * This works because BUF_SIZE is a power of 2. On small MCUs, bitwise AND
     * is usually faster than modulo because it avoids a division operation.
     */
    rb->head = (rb->head + 1) & BUF_MASK;
    rb->count++;

    return RB_OK;
}

int rb_read(RingBuf *rb, uint8_t *out)
{
    if (rb_empty(rb)) {
        return RB_EMPTY;
    }

    *out = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) & BUF_MASK;
    rb->count--;

    return RB_OK;
}

int main(void)
{
    RingBuf rb;
    uint8_t byte;
    int ret;
    uint8_t i;

    rb_init(&rb);

    for (i = 0; i < BUF_SIZE; i++) {
        uint8_t value = 0x41 + i;

        ret = rb_write(&rb, value);
        printf("[WRITE] 0x%02X -> %s (count=%u)%s\n",
               value,
               ret == RB_OK ? "OK" : "FAIL",
               rb_count(&rb),
               rb_full(&rb) ? " FULL" : "");
    }

    printf("Buffer full: %s, count=%u\n",
           rb_full(&rb) ? "yes" : "no",
           rb_count(&rb));

    ret = rb_write(&rb, 0x99);
    if (ret == RB_FULL) {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    for (i = 0; i < 3; i++) {
        ret = rb_read(&rb, &byte);
        if (ret == RB_OK) {
            printf("[READ]  -> 0x%02X (count=%u)\n", byte, rb_count(&rb));
        }
    }

    printf("Count after 3 reads: %u\n", rb_count(&rb));

    for (i = 0; i < 3; i++) {
        uint8_t value = 0x49 + i;

        ret = rb_write(&rb, value);
        if (ret == RB_OK) {
            printf("[WRITE] 0x%02X -> OK (count=%u)%s\n",
                   value,
                   rb_count(&rb),
                   rb_full(&rb) ? " FULL" : "");
        }
    }

    printf("Count after new writes: %u\n", rb_count(&rb));

    while (!rb_empty(&rb)) {
        ret = rb_read(&rb, &byte);
        if (ret == RB_OK) {
            printf("[READ]  -> 0x%02X (count=%u)\n", byte, rb_count(&rb));
        }
    }
    printf("Buffer empty: %s\n", rb_empty(&rb) ? "yes" : "no");

    ret = rb_read(&rb, &byte);
    if (ret == RB_EMPTY) {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
