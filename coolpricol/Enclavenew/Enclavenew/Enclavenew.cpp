
#include "Enclavenew_t.h"
#include <string.h>
#include "sgx_trts.h"

const char* testonms[] = { "Alex", "Vict", "Orio", "Leon" };

void foo(char* buffer, int len, int index) {
    if (index < 4) {
        memcpy(buffer, testonms[index], strlen(testonms[index] + 1));
    }
    else
    {
        memset(buffer, 0, strlen(testonms[0]));
    }
    return;
}
