/**
 * @file fcntl.h
 * @brief File control options.
 * @author ckoyama(koyamanX)
 */

#ifndef _FCNTL_H
#define _FCNTL_H

#define O_CREAT 00100
#define O_TRUNC 01000
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

#include <sys/types.h>

/**
 * @brief Open a file.
 * @details Open a file and establish the connection between a file and file
 * descriptor.
 * @param[in] pathname Path to open.
 * @param[in] flags Flags set to file.
 * @param[in] mode Mode to set.
 * @return File descriptor to open file on success or -1 on failure.
 * @attention Supported flags is the following:
 * 	- O_RDONLY
 * 	- O_WRONLY
 * 	- O_RDWR
 * 	- O_APPEND
 * 	- O_CREAT
 * 	- O_TRUNC
 */
int open(const char *pathname, int flags, mode_t mode);

/**
 * @brief Create a new file or rewrite an existing one.
 * @details shall be equvivalent to: open(path, O_WRONLY|O_CREAT|O_TRUNC, mode).
 * @param[in] pathname Path to create file to.
 * @param[in] lode New mode.
 * @return File descriptor to newly created file on success or -1 on failure.
 */
int creat(const char *pathname, mode_t mode);

#endif  // _FCNTL_H
