#pragma once

#include <sys/capability.h>


/** Set ambient capabilities.
 *
 * This sets up ambient capabilities for use by an about-to-be execve()'d
 * program.
 *
 * @return 0 on success, -1 on failure.
 */
int set_ambient_capabilities();


/** Enable the given capability.
 *
 * @return 0 on success, -1 on failure.
 */
int enable_cap(cap_value_t cap);


/** Disable the given capability.
 *
 * @return 0 on success, -1 on failure.
 */
int disable_cap(cap_value_t cap);


