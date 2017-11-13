#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define BCM2837_PERIPHERAL_BASE 0x3F000000 //
#define GPIO_BASE (BCM2837_PERIPHERAL_BASE + 0x00200000)

#define PIN_OUT 3
#define PIN_CLOCK 4
#define MODE_CLOCK 4
#define CONTROL_CLOCK (BCM2837_PERIPHERAL_BASE + 0x00101070)
#define DIV_CLOCK (BCM2837_PERIPHERAL_BASE + 0x00101070)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define GPIO_INPUT(pin) *(gpio + ((pin) / 10)) &= ~(7 << (((pin) % 10) * 3))
#define GPIO_OUTPUT(pin) *(gpio + ((pin) / 10)) |=  (1 << (((pin) % 10) * 3)) // Always use GPIO_INPUT first.
#define GPIO_ALTERNATE_MODE(pin, mode) *(gpio + (((pin) / 10))) |= (((mode) <= 3 ? (mode) + 4 : (mode) == 4 ? 3 : 2) << (((pin) % 10) * 3))
//                        ||||||||             ||    |||||
#define GPIO_CLOCK_MODE 0b01011010000000000000000000000000
//                        5   a   |  unused   |++ - <=>>>>
// + MASH
// - BUSY
// < KILL
// = ENAB
// > SRC

#define GPIO_CLOCK_DIV 0b00000000 00000000 00000000 00000000

#define GPIO_HIGH(pin) *(gpio + 7) |= 1 << (pin)
#define GPIO_LOW(pin) *(gpio + 10) |= 1 << (pin)

#define GPIO_GET(pin) *(gpio + 13) >> (pin) & 1

// #define GPIO_PULL *(gpio+37) // Pull up/pull down
// #define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock


int main(int argc, char **argv) {
    unsigned int interval = 1;
    unsigned int repetitions = 10;

    volatile unsigned int *gpio;
    int memory_file_descriptor;
    if ((memory_file_descriptor = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        printf("Error opening /dev/mem (don't forget to prepend 'sudo')");
        exit(-1);
    }
    gpio = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memory_file_descriptor, GPIO_BASE);
    close(memory_file_descriptor);
    if (gpio == MAP_FAILED) {
        printf("Error mapping gpio memory (don't forget to prepend 'sudo')");
        exit(-1);
    }
    GPIO_INPUT(PIN_CLOCK); // Always use GPIO_INPUT first.
    GPIO_OUTPUT(PIN_CLOCK);
    GPIO_ALTERNATE_MODE(PIN_CLOCK, MODE_CLOCK);


    GPIO_LOW(PIN_CLOCK); // Start at LOW
    for (unsigned int i = 0; i < repetitions; i++) {
        GPIO_HIGH(PIN_OUT);
        sleep(interval);
        GPIO_LOW(PIN_OUT);
        sleep(interval);
    }
    return 0;
}