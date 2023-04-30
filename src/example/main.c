#include <stdio.h>
#include <stdint.h>
#include <mess_coder.h>

int main(void) {
    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x04};
    fprintf(stdout, "req len = %d\n", messcoder_comp_enc_size(buf, 4));
    return 0;
}
