/** Functions for starting a subprocess and communicating with it. */
#pragma once

#include <sys/types.h>


/** Run a command and optionally communicate with it.
 *
 * @param filename The file to execute.
 * @param argv Arguments to pass (may be NULL).
 * @param env Environment variables to set (may be NULL).
 * @param in_buf Data to send to stdin (may be NULL).
 * @param in_size Length of input data.
 * @param exit_code (out) The exit code of the process run, 255 if there was an
 *              error starting it, or -signal if it was terminated with a
 *              signal.
 * @param out_buf (out) Pointer to a buffer with output received from the
 *              command. This buffer will be allocated by this function, and
 *              must be  freed using free() by the caller after use.
 * @param out_size (out) Pointer to a variable to store the number of chars in
 *              the output buffer in.
 * @return 0 on success, -1 on error.
 */
int run(
        const char * filename, const char * const argv[],
        const char * const env[],
        const char * in_buf, ssize_t in_size,
        int * exit_code, const char ** out_buf, ssize_t * out_size);


/** Print the output from a called program.
 *
 * @param out_buf The output buffer.
 * @param out_size Size of the output.
 */
void print_error_output(const char * out_buf, ssize_t out_size);


/** Run and check that it was successful.
 *
 * If either an error occurs when calling the process, or the process itself
 * exits with a non-zero exit code, an error message and the initial output will
 * be printed on stderr, and the output variables will not be assigned to.
 * Otherwise, the outputs are set and 0 is returned.
 *
 * @param filename The file to execute.
 * @param argv Arguments to pass (may be NULL).
 * @param env Environment variables to set (may be NULL).
 * @param in_buf Data to send to stdin (may be NULL).
 * @param in_size Length of input data.
 * @param exit_code (out) The exit code of the process run, 255 if there was an
 *              error starting it, or -signal if it was terminated with a
 *              signal.
 * @param out_buf (out) Pointer to a buffer with output received from the
 *              command. This buffer will be allocated by this function, and
 *              must be freed using free() by the caller after use. If NULL is
 *              passed, any output will be discarded.
 * @param out_size (out) Pointer to a variable to store the number of chars in
 *              the output buffer in. May bu NULL if out_buf is NULL.
 * @return 0 on success, 1 on error.
 */
int run_check(
        const char * filename, const char * const argv[],
        const char * const env[],
        const char * in_buf, ssize_t in_size,
        const char ** out_buf, ssize_t * out_size);


/** Run and check that it was successful.
 *
 * If either an error occurs when calling the process, or the process itself
 * exits with a non-zero exit code, an error message and the initial output will
 * be printed on stderr.
 *
 * This is a simplified version, it doesn't do input or output and always uses
 * an empty environment.
 *
 * @param filename The file to execute.
 * @param argv Arguments to pass (may be NULL).
 * @return 0 on success, 1 on error.
 */
int run_check2(const char * filename, const char * const argv[]);

