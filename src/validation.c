#include "validation.h"


int validate_end(const char * cur) {
    return *cur != '\0';
}


int validate_literal(char c, const char * cur, const char ** next) {
    if (*cur == c) {
        if (next) {
            *next = cur + 1;
            return 0;
        }
        return *(cur + 1) != '\0';
    }
    return 1;
}


int validate_letter(const char * cur, const char ** next) {
    if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z')) {
        if (next) {
            *next = cur + 1;
            return 0;
        }
        return *(cur + 1) != '\0';
    }
    return 1;
}


int validate_digit(const char * cur, const char ** next) {
    if (*cur >= '0' && *cur <= '9') {
        if (next) {
            *next = cur + 1;
            return 0;
        }
        return *(cur + 1) != '\0';
    }
    return 1;
}


int validate_number(
        ptrdiff_t max_digits, const char * cur, const char ** next)
{
    const char * start = cur;
    int err = 0;

    if (validate_digit(cur, &cur)) return 1;

    // success guaranteed here, get rest of digits
    while (!err && (cur - start) < max_digits) {
        err = validate_digit(cur, &cur);
    }

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}


int validate_dev(const char * cur, const char ** next) {
    int err = 0;

    // required initial letter
    err = validate_letter(cur, &cur);
    if (err) return err;

    // success guaranteed here, include optional rest of characters
    while (!err) {
        err = validate_letter(cur, &cur);
        if (err)
            err = validate_digit(cur, &cur);
        if (err)
            err = validate_literal('-', cur, &cur);
        if (err)
            err = validate_literal('@', cur, &cur);
        // Linux seems to allow some other characters, but these are the ones
        // in common use, so we'll keep it at this for now.
    }

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}


int validate_ip(const char * cur, const char ** next) {
    int i;

    if (validate_number(3, cur, &cur)) return 1;

    for (i = 0; i < 3; i++) {
        if (validate_literal('.', cur, &cur)) return 1;
        if (validate_number(3, cur, &cur)) return 1;
    }

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}


int validate_network(const char * cur, const char ** next) {
    if (validate_ip(cur, &cur)) return 1;
    if (validate_literal('/', cur, &cur)) return 1;
    if (validate_number(2, cur, &cur)) return 1;

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}


int validate_endpoint(const char * cur, const char ** next) {
    if (validate_ip(cur, &cur)) return 1;
    if (validate_literal(':', cur, &cur)) return 1;
    if (validate_number(5, cur, &cur)) return 1;

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}


int validate_wireguard_key(const char * cur, const char ** next) {
    int count = 0;

    while (*cur != 0 && count < 43) {
        if (
                (*cur >= 'a' && *cur <= 'z') ||
                (*cur >= 'A' && *cur <= 'Z') ||
                (*cur >= '0' && *cur <= '9') ||
                (*cur == '+') || (*cur == '/')) {
            cur++;
            count++;
        }
        else return 1;
    }
    if (count < 43) return 1;
    if (*cur != '=') return 1;
    cur++;

    if (next) {
        *next = cur;
        return 0;
    }
    return *cur != '\0';
}

