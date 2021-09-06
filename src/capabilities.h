#pragma once


/** Set ambient capabilities.
 *
 * This sets up ambient capabilities for use by an about-to-be execve()'d
 * program.
 *
 * @return 0 on success, -1 on failure.
 */
int set_ambient_capabilities();


/** Enable CAP_SYS_PTRACE so we can access a container's network namespace.
 *
 * @return 0 on success, -1 on failure.
 */
int enable_cap_sys_ptrace();


/** Disable CAP_SYS_PTRACE again.
 *
 * @return 0 on success, -1 on failure.
 */
int disable_cap_sys_ptrace();


/** Enable CAP_SYS_ADMIN so we can switch namespaces.
 *
 * @return 0 on success, -1 on failure.
 */
int enable_cap_sys_admin();


/** Disable CAP_SYS_ADMIN again.
 *
 * @return 0 on success, -1 on failure.
 */
int disable_cap_sys_admin();

