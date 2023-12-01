#include <stdio.h>
#include <stdlib.h>
#include "../utils/input.h"
#include "../utils/strtools.h"
#include "../utils/macros.h"
#include "../utils/timer.h"

int handle_on_found(const char *pos, const char *needle, int needle_index, void* ctx) {
    int* result = (int *) ctx;
    *result = (needle_index % 9) + 1;
    return 0;
}

int extract_digits(const char *line, const char** needles, int num_needles) {
    int start = -1, end = -1;
    find_forward(needles, num_needles, line, handle_on_found, &start);
    find_backward(needles, num_needles, line, handle_on_found, &end);

    ASSERT(start != -1, "Start not found");
    ASSERT(end != -1, "End not found");

    return start * 10 + end;
}

void handle_input_part1(const char *line, int line_num, void* ctx) {
    const char *needles[] = {
            "1","2","3","4","5","6","7","8","9"
    };
    int* result = (int *) ctx;
    *result += extract_digits(line, needles, NELEM(needles));
}

void handle_input_part2(const char *line, int line_num, void* ctx) {
    const char *needles[] = {
            "one","two","three","four","five","six","seven","eight","nine",
            "1","2","3","4","5","6","7","8","9"
    };
    int* result = (int *) ctx;
    *result +=  extract_digits(line, needles, NELEM(needles));
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    int result = 0;
    const char *sample = "1abc2\n"
                         "pqr3stu8vwx\n"
                         "a1b2c3d4e5f\n"
                         "treb7uchet";

    timer_start(&clock);
    sample_input(sample, handle_input_part1, &result);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %d\n", result);
    ASSERT_EQUAL(result, 142, "Part 1 sample answer did not match");

    result = 0;
    timer_start(&clock);
    read_input("../day1/input.txt", handle_input_part1, handle_error, &result);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %d\n", result);
    ASSERT_EQUAL(result, 55621, "Part 1 input answer did not match");

    result = 0;
    const char *sample_pt2 = "two1nine\n"
                             "eightwothree\n"
                             "abcone2threexyz\n"
                             "xtwone3four\n"
                             "4nineeightseven2\n"
                             "zoneight234\n"
                             "7pqrstsixteen";

    timer_start(&clock);
    sample_input(sample_pt2, handle_input_part2, &result);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %d\n", result);
    ASSERT_EQUAL(result,  281, "Part 2 sample answer did not match");

    result = 0;
    timer_start(&clock);
    read_input("../day1/input.txt", handle_input_part2, handle_error, &result);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %d\n", result);
    ASSERT_EQUAL(result,  53592, "Part 2 input answer did not match");

    return 0;
}
