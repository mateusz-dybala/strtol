#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <errno.h>

#define CHAR_DIFF ('a' - 10)

int is_valid(char, int);
int get_char_value(char);

long strtol(const char* nPtr, char** endPtr, int base) {
    long result = 0l;

    // TODO should it be like this ??
    if (endPtr == NULL) {
        return result;
    }

    if (base < 0 || base > 36 || base == 1) {
        errno = EINVAL;
        *endPtr = (char*) nPtr;
        return result;
    }

    int sign = 1;
    int start_index = 0;
    int char_number = 0;
    char next_char = nPtr[start_index];
    char* valid_str = malloc(sizeof(char) * (char_number + 1));
    if (valid_str == NULL) {
        free(valid_str);
        *endPtr = (char*) nPtr;
        return result;
    }

    // whitespace check
    while (isspace(next_char)) {
        start_index += 1;
        next_char = nPtr[start_index];
    }

    // sign check
    if (next_char == '-') {
        sign = -1;
        start_index += 1;
        next_char = nPtr[start_index];
    }else if (next_char == '+') {
        sign = 1;
        start_index += 1;
        next_char = nPtr[start_index];
    }

    // prefix check
    switch (base) {
        case 0:
            // detect base
            if (next_char == '0') {
                base = 8;
                start_index += 1;
                next_char = nPtr[start_index];
            }else {
                base = 10;
                break;
            }

            // check if base is 16
            if (tolower(next_char) == 'x') {
                base = 16;
                start_index += 1;
                next_char = nPtr[start_index];
            }

            if (base == 8 && !is_valid(next_char, base)) {
                goto empty_0_prefix;
            }
            if (base == 16 && !is_valid(next_char, base)) {
                goto empty_0x_prefix;
            }

            break;
        case 8:
            if (next_char == '0') {
                start_index += 1;
                next_char = nPtr[start_index];
            }
            if (!is_valid(next_char, base)) {
                empty_0_prefix:
                    free(valid_str);
                    valid_str = NULL;
                    *endPtr = (char*) nPtr + start_index;
                    return result;
            }
            break;
        case 16:
            if (next_char == '0' && tolower(nPtr[start_index+1]) == 'x') {
                start_index += 2;
                next_char = nPtr[start_index];
            }
            if (!is_valid(next_char, base)) {
                empty_0x_prefix:
                    free(valid_str);
                    valid_str = NULL;
                    *endPtr = (char*) nPtr + start_index - 1;
                    return result;
            }
            break;
    }

    while (next_char != '\0') {
        // base check
        if (! is_valid(next_char, base)) {
            break; // TODO fix break ??
        }

        // memory reallocation
        char* temp = realloc(valid_str, sizeof(char) * (char_number + 1));
        if (temp == NULL) {
            free(temp);
            free(valid_str);
            valid_str = NULL;
            *endPtr = (char*) nPtr + start_index + char_number;
            return result;
        }
        valid_str = temp;


        valid_str[char_number] = next_char;
        char_number++;
        next_char = nPtr[start_index + char_number];
    }

    // string was empty
    if (char_number == 0) {
        free(valid_str);
        *endPtr = (char*) nPtr;
        return result;
    }

    // calculate value
    for (int index = 0; index < char_number; index++) {
        int char_value = get_char_value(valid_str[index]);
        double power = pow(base, char_number - index - 1);
        long value = sign * char_value * power;

        // long overflow possible
        if (sign > 0 && result > LONG_MAX - value) {
            free(valid_str);
            valid_str = NULL;
            *endPtr = (char*) (nPtr + char_number + start_index);
            errno = ERANGE;
            return LONG_MAX;
        }else if (sign < 0 && result < LONG_MIN - value) {
            free(valid_str);
            valid_str = NULL;
            *endPtr = (char*) (nPtr + char_number + start_index);
            errno = ERANGE;
            return LONG_MIN;
        }

        result += value;
    }

    free(valid_str);
    valid_str = NULL;
    *endPtr = (char*) (nPtr + start_index + char_number);
    return result;
}


int is_valid(char next_char, int base) {
    if (next_char >= '0' && next_char <= '9' && next_char - '0' < base) {
        return 1;
    }

    next_char = (char) tolower(next_char);
    if (next_char - CHAR_DIFF < base && next_char >= 'a') {
        return 1;
    }

    return 0;
}

int get_char_value(char char_value) {
    if (char_value >= '0' && char_value <= '9') {
        return char_value - '0';
    }

    char_value = (char) tolower(char_value);
    return char_value - CHAR_DIFF;
}






























