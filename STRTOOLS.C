#include "strtools.h"
#include "macros.h"

int compare(const char* s1_start, const char* s1_end, const char* s2_start, const char* s2_end) {
    int s1_dir = MAX(MIN(s1_end - s1_start, 1), -1);
    int s2_dir = MAX(MIN(s2_end - s2_start, 1), -1);
    const char* s1_ptr = s1_start;
    const char* s2_ptr = s2_start;
    if (s1_dir == 0 || s2_dir == 0) return -1;
    while(1) {
        if (*s1_ptr != *s2_ptr) return -1;
        s1_ptr += s1_dir;
        s2_ptr += s2_dir;
        if (s1_ptr == s1_end) break;
        if (s2_ptr == s2_end) return -1;
    }
    return 0;
}

void find_forward(const char** needles, size_t needle_size, const char* haystack, cb_on_found on_found, void* ctx) {
    const char *ptr = haystack;
    int i;
    while(*ptr != '\0') {
        for(i = 0; i < needle_size; i++) {
            if (compare(needles[i], ENDSTRPTR(needles[i]), ptr, ENDSTRPTR(haystack)) == 0) {
                if (!on_found(ptr, needles[i], i, ctx)) {
                    return;
                }
            }
        }
        ptr++;
    }
}

void find_backward(const char** needles, size_t needle_size, const char* haystack, cb_on_found on_found, void* ctx) {
    const char *ptr = ENDSTRPTR(haystack) - 1;
    int i;
    while(ptr >= haystack) {
        for(i = 0; i < needle_size; i++) {
            if (compare(ENDSTRPTR(needles[i]) - 1, needles[i] - 1, ptr, haystack - 1) == 0) {
                if (!on_found(ptr, needles[i], i, ctx)) {
                    return;
                }
            }
        }
        ptr--;
    }
}
