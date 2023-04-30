#include <stdio.h>
#include <mess_coder.h>

int main(void) {
    fprintf(stdout, "Start byte = 0x%02hhX\n", MESS_CODER_START_B);
    return 0;
}
