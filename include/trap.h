#ifndef _TRAP_H
#define _TRAP_H

/**
 *	@brief Trap handler for kernel mode.
 *	@details Handles traps occured in kernel mode.
 */
void kerneltrap(void);

/**
 *	@brief Trap handler for user mode.
 *	@details Handles traps occured in user mode.
 */
void usertrap(void);

/**
 * @brief Returns to user code where trap occurs.
 * @details Returns to user code where trap occurs.
 */
extern void usertrapret(void);

/**
 *	@brief Low-level codes to return user code.
 *	@details Low-level codes to return user code.
 */
extern void userret(u64 satp);

#endif /* _TRAP_H */
