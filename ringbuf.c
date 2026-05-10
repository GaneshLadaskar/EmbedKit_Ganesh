#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE     8U               
#define BUFFER_MASK     (BUFFER_SIZE - 1U) 

#define RB_OK           0
#define RB_ERR_FULL    -1
#define RB_ERR_EMPTY   -2

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint8_t head;
    uint8_t count;
    uint8_t tail;
} RingBuffer;

/* -----------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------- */

void ringbuf_init(RingBuffer *rb)
{
    rb->head  = 0U;
    rb->tail  = 0U;
    rb->count = 0U;
}

uint8_t ringbuf_is_full(const RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE) ? 1U : 0U;
}

uint8_t ringbuf_is_empty(const RingBuffer *rb)
{
    return (rb->count == 0U) ? 1U : 0U;
}

uint8_t ringbuf_count(const RingBuffer *rb)
{
    return rb->count;
}


int ringbuf_write(RingBuffer *rb, uint8_t byte)
{
    if (ringbuf_is_full(rb)) {
        return RB_ERR_FULL;
    }

    rb->data[rb->head] = byte;
    rb->head = (rb->head + 1U) & BUFFER_MASK;
    rb->count++;

    return RB_OK;
}


int ringbuf_read(RingBuffer *rb, uint8_t *byte_out)
{
    if (ringbuf_is_empty(rb)) {
        return RB_ERR_EMPTY;
    }

    *byte_out = rb->data[rb->tail];
    rb->tail  = (rb->tail + 1U) & BUFFER_MASK;
    rb->count--;

    return RB_OK;
}


int main(void)
{
    RingBuffer rb;
    uint8_t    byte_value;
    int        result;
    uint8_t    index;

    ringbuf_init(&rb);

    printf("============================================================\n");
    printf("  Ring Buffer Demo\n");
    printf("  Capacity: %u bytes\n", BUFFER_SIZE);
    printf("============================================================\n\n");


    printf("[ Step 1 ] Writing 8 bytes to fill the buffer\n");

    uint8_t fill_data[BUFFER_SIZE] = {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    for (index = 0U; index < BUFFER_SIZE; index++) {
        result = ringbuf_write(&rb, fill_data[index]);
        if (result == RB_OK) {
            printf("  [WRITE] 0x%02X -> OK      (count = %u)%s\n",
                fill_data[index],
                ringbuf_count(&rb),
                ringbuf_is_full(&rb) ? "  <-- FULL" : "");
        } else {
            printf("  [WRITE] 0x%02X -> FAIL    (buffer full)\n",
                fill_data[index]);
        }
    }
    printf("  Buffer full: %s  |  Count: %u\n\n",
        ringbuf_is_full(&rb) ? "YES" : "NO",
        ringbuf_count(&rb));

    printf("[ Step 2 ] Attempting to write 0x99 into a full buffer\n");

    result = ringbuf_write(&rb, 0x99U);
    if (result == RB_ERR_FULL) {
        printf("  [WRITE] 0x99 -> FAIL    (buffer full -- data protected)\n\n");
    }

    printf("[ Step 3 ] Reading 3 bytes out\n");

    uint8_t read_count = 3U;
    uint8_t i;
    for (i = 0U; i < read_count; i++) {
        result = ringbuf_read(&rb, &byte_value);
        if (result == RB_OK) {
            printf("  [READ]  -> 0x%02X         (count = %u)\n",
                byte_value, ringbuf_count(&rb));
        }
    }
    printf("  Count after 3 reads: %u\n\n", ringbuf_count(&rb));

    printf("[ Step 4 ] Writing 3 new bytes into the freed slots\n");

    uint8_t new_data[] = { 0x49U, 0x4AU, 0x4BU };
    uint8_t new_count  = 3U;
    uint8_t j;
    for (j = 0U; j < new_count; j++) {
        result = ringbuf_write(&rb, new_data[j]);
        if (result == RB_OK) {
            printf("  [WRITE] 0x%02X -> OK      (count = %u)%s\n",
                new_data[j],
                ringbuf_count(&rb),
                ringbuf_is_full(&rb) ? "  <-- FULL" : "");
        }
    }
    printf("  Count after 3 writes: %u\n\n", ringbuf_count(&rb));

    printf("[ Step 5 ] Draining all remaining bytes\n");

    while (!ringbuf_is_empty(&rb)) {
        result = ringbuf_read(&rb, &byte_value);
        if (result == RB_OK) {
            printf("  [READ]  -> 0x%02X         (count = %u)\n",
                byte_value, ringbuf_count(&rb));
        }
    }
    printf("  Buffer empty: %s\n\n",
        ringbuf_is_empty(&rb) ? "YES" : "NO");

    printf("[ Step 6 ] Attempting to read from an empty buffer\n");

    result = ringbuf_read(&rb, &byte_value);
    if (result == RB_ERR_EMPTY) {
        printf("  [READ]  -> FAIL         (buffer empty -- no garbage returned)\n\n");
    }

    printf("============================================================\n");
    printf("  Demo complete\n");
    printf("============================================================\n");

    return 0;
}