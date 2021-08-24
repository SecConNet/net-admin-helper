/** Validation functions
 *
 * Validation functions validate a null-terminated string or the start of one,
 * and they all have similar signatures and work in the same way.
 *
 * The functions are passed a pointer `cur` (in)to a string to validate by
 * reference. They are also passed a pointer to a pointer `next` to optionally
 * return the location of the first character after the validated portion of the
 * string in. If `next` is NULL, validation is only successful if the entire
 * string matches (i.e. after matching, we have reached the end of the string).
 *
 * On success, if `next` is not NULL then the `*next` is set to point to the
 * first character after the validated portion of the string. The function then
 * returns 0;
 *
 * If validation fails, the return value will be nonzero and `*next` will be
 * unchanged.
 *
 * @param cur Pointer to the first character to validate (in).
 * @param next Pointer to set to the next character (out).
 * @return 0 on success, 1 on error.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>


/** At end of buffer.
 *
 * This validates that we are at the end of the buffer.
 *
 */
int validate_end(const char * cur);


/** Validate a specified literal character.
 *
 * Checks that the string starts with the given character.
 *
 * See validation functions above.
 */
int validate_literal(char c, const char * cur, const char ** next);


/** Validate an ASCII lower- or uppercase letter.
 *
 * See validation functions above.
 */
int validate_letter(const char * cur, const char ** next);


/** Validate an ASCII digit.
 *
 * See validation functions above.
 */
int validate_digit(const char * cur, const char ** next);


/** Validate a number.
 *
 * This validates a simple positive decimal number of at most `max_digits`
 * digits. `max_digits` must be at least one.
 *
 * See validation functions above.
 */
int validate_number(ptrdiff_t max_digits, const char * cur, const char ** next);


/** Validate a network device name.
 *
 * Network device names (eth0, tun1, etc.) must start with a lower- or
 * uppercase ASCII letter, optionally followed by lower- or uppercase
 * ASCII letters or digits.
 *
 * See validation functions above.
 */
int validate_dev(const char * cur, const char ** next);


/** Validate an IPv4 address in dotted-decimal notation.
 *
 * See validation functions above.
 */
int validate_ip(const char * cur, const char ** next);


/** Validate a network address range.
 *
 * This is an IPv4 address in dotted-decimal notation, followed by a slash and a
 * netmask expressed as a single integer.
 *
 * See validation functions above.
 */
int validate_network(const char * cur, const char ** next);


/** Validate an endpoint.
 *
 * This is an IPv4 address in dotted-decimal notation, followed by a colon and a
 * port expressed as a single integer.
 *
 * See validation functions above.
 */
int validate_endpoint(const char * cur, const char ** next);


/** Validate a WireGuard key.
 *
 * This is a 43-character Base-64 string, followed by an = literal.
 *
 * See validatefunctions above.
 */
int validate_wireguard_key(const char * cur, const char ** next);

