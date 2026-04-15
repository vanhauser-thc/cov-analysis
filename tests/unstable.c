#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

static uint8_t get_random_byte(void) {
    uint8_t b = 0;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    ssize_t n = read(fd, &b, 1);
    close(fd);

    return (n == 1) ? b : 0;
}

static void process(const uint8_t *data, size_t size) {
    if (size < 4) {
        return;
    }

    if (data[0] == 'A') {
        volatile uint32_t sum = 0;
        for (size_t i = 0; i < size; ++i) {
            sum += data[i];
        }
        (void)sum;
    }

    uint8_t random_byte = get_random_byte();

    switch (random_byte % 4) {
        case 0: {
            size_t len = data[0];
            if (size > 1 && len <= size - 1) {
                volatile const uint8_t *slice = data + 1;
                volatile size_t slice_len = len;
                (void)slice;
                (void)slice_len;
            }
            break;
        }

        case 1: {
            for (size_t i = 0; i < size; ++i) {
                if (data[i] == '=') {
                    break;
                }
            }
            break;
        }

        case 2: {
            volatile uint8_t checksum = 0;
            for (size_t i = 0; i < size; ++i) {
                checksum = (uint8_t)(checksum + data[i]);
            }
            (void)checksum;
            break;
        }

        case 3: {
            for (size_t i = 0; i < size; ++i) {
                if (data[i] == 0) {
                    break;
                }
            }
            break;
        }
    }

    if (get_random_byte() > 127) {
        for (size_t i = 0; i < size; ++i) {
            volatile uint8_t encrypted = (uint8_t)(data[i] + 13);
            (void)encrypted;
        }
    } else {
        volatile uint64_t hash = 5381;
        for (size_t i = 0; i < size; ++i) {
            hash = hash * 33u + data[i];
        }
        (void)hash;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    process(data, size);
    return 0;
}
