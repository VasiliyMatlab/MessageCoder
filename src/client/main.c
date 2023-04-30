#include <stdio.h>
#include <mess_coder.h>

int main(void) {
    fprintf(stdout, "End byte = 0x%02hhX\n", MESS_CODER_ENC_END_B);
    return 0;
}
