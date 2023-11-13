#include <stdio.h>

int main(int argc, char **argv) {
    char inbuf[64 * 128];
    char outbuf[8 * 128];

    fread(inbuf, 1, 128 * 64, stdin);

    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 128; ++i) {
            outbuf[j * 128 + i] = 0;
            for (int k = 0; k < 8; ++k) {
                outbuf[j * 128 + i] |= (inbuf[(j*8 + k) * 128 + i] ? 1 : 0) << k;
            }
        }
    }

    fwrite(outbuf, 1, 128 * 8, stdout);
}
