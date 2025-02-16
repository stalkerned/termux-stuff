/**
 * Seccomp Library
 *
 * Copyright (c) 2012,2013 Red Hat <pmoore@redhat.com>
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses>.
 */

#ifndef _SECCOMP_H
#define _SECCOMP_H

#include <elf.h>
#include <inttypes.h>
#include <asm/unistd.h>
#include <linux/audit.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * version information
 */

#define SCMP_VER_MAJOR		2
#define SCMP_VER_MINOR		3
#define SCMP_VER_MICRO		0

struct scmp_version {
	unsigned int major;
	unsigned int minor;
	unsigned int micro;
};

/*
 * types
 */

/**
 * Filter context/handle
 */
typedef void *scmp_filter_ctx;

/**
 * Filter attributes
 */
enum scmp_filter_attr {
	_SCMP_FLTATR_MIN = 0,
	SCMP_FLTATR_ACT_DEFAULT = 1,	/**< default filter action */
	SCMP_FLTATR_ACT_BADARCH = 2,	/**< bad architecture action */
	SCMP_FLTATR_CTL_NNP = 3,	/**< set NO_NEW_PRIVS on filter load */
	SCMP_FLTATR_CTL_TSYNC = 4,	/**< sync threads on filter load */
	_SCMP_FLTATR_MAX,
};

/**
 * Comparison operators
 */
enum scmp_compare {
	_SCMP_CMP_MIN = 0,
	SCMP_CMP_NE = 1,		/**< not equal */
	SCMP_CMP_LT = 2,		/**< less than */
	SCMP_CMP_LE = 3,		/**< less than or equal */
	SCMP_CMP_EQ = 4,		/**< equal */
	SCMP_CMP_GE = 5,		/**< greater than or equal */
	SCMP_CMP_GT = 6,		/**< greater than */
	SCMP_CMP_MASKED_EQ = 7,		/**< masked equality */
	_SCMP_CMP_MAX,
};

/**
 * Argument datum
 */
typedef uint64_t scmp_datum_t;

/**
 * Argument / Value comparison definition
 */
struct scmp_arg_cmp {
	unsigned int arg;	/**< argument number, starting at 0 */
	enum scmp_compare op;	/**< the comparison op, e.g. SCMP_CMP_* */
	scmp_datum_t datum_a;
	scmp_datum_t datum_b;
};

/*
 * macros/defines
 */

/**
 * The native architecture token
 */
#define SCMP_ARCH_NATIVE	0

/**
 * The x86 (32-bit) architecture token
 */
#define SCMP_ARCH_X86		AUDIT_ARCH_I386

/**
 * The x86-64 (64-bit) architecture token
 */
#define SCMP_ARCH_X86_64	AUDIT_ARCH_X86_64

/**
 * The x32 (32-bit x86_64) architecture token
 *
 * NOTE: this is different from the value used by the kernel because we need to
 * be able to distinguish between x32 and x86_64
 */
#define SCMP_ARCH_X32		(EM_X86_64|__AUDIT_ARCH_LE)

/**
 * The ARM architecture tokens
 */
#define SCMP_ARCH_ARM		AUDIT_ARCH_ARM
/* AArch64 support for audit was merged in 3.17-rc1 */
#ifndef AUDIT_ARCH_AARCH64
#ifndef EM_AARCH64
#define EM_AARCH64		183
#endif /* EM_AARCH64 */
#define AUDIT_ARCH_AARCH64	(EM_AARCH64|__AUDIT_ARCH_64BIT|__AUDIT_ARCH_LE)
#endif /* AUDIT_ARCH_AARCH64 */
#define SCMP_ARCH_AARCH64	AUDIT_ARCH_AARCH64

/**
 * The MIPS architecture tokens
 */
#ifndef __AUDIT_ARCH_CONVENTION_MIPS64_N32
#define __AUDIT_ARCH_CONVENTION_MIPS64_N32	0x20000000
#endif
#ifndef EM_MIPS
#define EM_MIPS			8
#endif
#ifndef AUDIT_ARCH_MIPS
#define AUDIT_ARCH_MIPS		(EM_MIPS)
#endif
#ifndef AUDIT_ARCH_MIPS64
#define AUDIT_ARCH_MIPS64	(EM_MIPS|__AUDIT_ARCH_64BIT)
#endif
/* MIPS64N32 support was merged in 3.15 */
#ifndef AUDIT_ARCH_MIPS64N32
#define AUDIT_ARCH_MIPS64N32	(EM_MIPS|__AUDIT_ARCH_64BIT|\
				 __AUDIT_ARCH_CONVENTION_MIPS64_N32)
#endif
/* MIPSEL64N32 support was merged in 3.15 */
#ifndef AUDIT_ARCH_MIPSEL64N32
#define AUDIT_ARCH_MIPSEL64N32	(EM_MIPS|__AUDIT_ARCH_64BIT|__AUDIT_ARCH_LE|\
				 __AUDIT_ARCH_CONVENTION_MIPS64_N32)
#endif
#define SCMP_ARCH_MIPS		AUDIT_ARCH_MIPS
#define SCMP_ARCH_MIPS64	AUDIT_ARCH_MIPS64
#define SCMP_ARCH_MIPS64N32	AUDIT_ARCH_MIPS64N32
#define SCMP_ARCH_MIPSEL	AUDIT_ARCH_MIPSEL
#define SCMP_ARCH_MIPSEL64	AUDIT_ARCH_MIPSEL64
#define SCMP_ARCH_MIPSEL64N32	AUDIT_ARCH_MIPSEL64N32

/**
 * The PowerPC architecture tokens
 */
#define SCMP_ARCH_PPC		AUDIT_ARCH_PPC
#define SCMP_ARCH_PPC64		AUDIT_ARCH_PPC64
#ifndef AUDIT_ARCH_PPC64LE
#define AUDIT_ARCH_PPC64LE	(EM_PPC64|__AUDIT_ARCH_64BIT|__AUDIT_ARCH_LE)
#endif
#define SCMP_ARCH_PPC64LE	AUDIT_ARCH_PPC64LE

/**
 * The S390 architecture tokens
 */
#define SCMP_ARCH_S390		AUDIT_ARCH_S390
#define SCMP_ARCH_S390X		AUDIT_ARCH_S390X

/**
 * Convert a syscall name into the associated syscall number
 * @param x the syscall name
 */
#define SCMP_SYS(x)		(__NR_##x)

/**
 * Specify an argument comparison struct for use in declaring rules
 * @param arg the argument number, starting at 0
 * @param op the comparison operator, e.g. SCMP_CMP_*
 * @param datum_a dependent on comparison
 * @param datum_b dependent on comparison, optional
 */
#define SCMP_CMP(...)		((struct scmp_arg_cmp){__VA_ARGS__})

/**
 * Specify an argument comparison struct for argument 0
 */
#define SCMP_A0(...)		SCMP_CMP(0, __VA_ARGS__)

/**
 * Specify an argument comparison struct for argument 1
 */
#define SCMP_A1(...)		SCMP_CMP(1, __VA_ARGS__)

/**
 * Specify an argument comparison struct for argument 2
 */
#define SCMP_A2(...)		SCMP_CMP(2, __VA_ARGS__)

/**
 * Specify an argument comparison struct for argument 3
 */
#define SCMP_A3(...)		SCMP_CMP(3, __VA_ARGS__)

/**
 * Specify an argument comparison struct for argument 4
 */
#define SCMP_A4(...)		SCMP_CMP(4, __VA_ARGS__)

/**
 * Specify an argument comparison struct for argument 5
 */
#define SCMP_A5(...)		SCMP_CMP(5, __VA_ARGS__)

/*
 * seccomp actions
 */

/**
 * Kill the process
 */
#define SCMP_ACT_KILL		0x00000000U
/**
 * Throw a SIGSYS signal
 */
#define SCMP_ACT_TRAP		0x00030000U
/**
 * Return the specified error code
 */
#define SCMP_ACT_ERRNO(x)	(0x00050000U | ((x) & 0x0000ffffU))
/**
 * Notify a tracing process with the specified value
 */
#define SCMP_ACT_TRACE(x)	(0x7ff00000U | ((x) & 0x0000ffffU))
/**
 * Allow the syscall to be executed
 */
#define SCMP_ACT_ALLOW		0x7fff0000U

/*
 * functions
 */

/**
 * Query the library version information
 *
 * This function returns a pointer to a populated scmp_version struct, the
 * caller does not need to free the structure when finished.
 *
 */
const struct scmp_version *seccomp_version(void);

/**
 * Initialize the filter state
 * @param def_action the default filter action
 *
 * This function initializes the internal seccomp filter state and should
 * be called before any other functions in this library to ensure the filter
 * state is initialized.  Returns a filter context on success, NULL on failure.
 *
 */
scmp_filter_ctx seccomp_init(uint32_t def_action);

/**
 * Reset the filter state
 * @param ctx the filter context
 * @param def_action the default filter action
 *
 * This function resets the given seccomp filter state and ensures the
 * filter state is reinitialized.  This function does not reset any seccomp
 * filters already loaded into the kernel.  Returns zero on success, negative
 * values on failure.
 *
 */
int seccomp_reset(scmp_filter_ctx ctx, uint32_t def_action);

/**
 * Destroys the filter state and releases any resources
 * @param ctx the filter context
 *
 * This functions destroys the given seccomp filter state and releases any
 * resources, including memory, associated with the filter state.  This
 * function does not reset any seccomp filters already loaded into the kernel.
 * The filter context can no longer be used after calling this function.
 *
 */
void seccomp_release(scmp_filter_ctx ctx);

/**
 * Merge two filters
 * @param ctx_dst the destination filter context
 * @param ctx_src the source filter context
 *
 * This function merges two filter contexts into a single filter context and
 * destroys the second filter context.  The two filter contexts must have the
 * same attribute values and not contain any of the same architectures; if they
 * do, the merge operation will fail.  On success, the source filter context
 * will be destroyed and should no longer be used; it is not necessary to
 * call seccomp_release() on the source filter context.  Returns zero on
 * success, negative values on failure.
 *
 */
int seccomp_merge(scmp_filter_ctx ctx_dst, scmp_filter_ctx ctx_src);

/**
 * Resolve the architecture name to a architecture token
 * @param arch_name the architecture name
 *
 * This function resolves the given architecture name to a token suitable for
 * use with libseccomp, returns zero on failure.
 *
 */
uint32_t seccomp_arch_resolve_name(const char *arch_name);

/**
 * Return the native architecture token
 *
 * This function returns the native architecture token value, e.g. SCMP_ARCH_*.
 *
 */
uint32_t seccomp_arch_native(void);

/**
 * Check to see if an existing architecture is present in the filter
 * @param ctx the filter context
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 *
 * This function tests to see if a given architecture is included in the filter
 * context.  If the architecture token is SCMP_ARCH_NATIVE then the native
 * architecture will be assumed.  Returns zero if the architecture exists in
 * the filter, -EEXIST if it is not present, and other negative values on
 * failure.
 *
 */
int seccomp_arch_exist(const scmp_filter_ctx ctx, uint32_t arch_token);

/**
 * Adds an architecture to the filter
 * @param ctx the filter context
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 *
 * This function adds a new architecture to the given seccomp filter context.
 * Any new rules added after this function successfully returns will be added
 * to this architecture but existing rules will not be added to this
 * architecture.  If the architecture token is SCMP_ARCH_NATIVE then the native
 * architecture will be assumed.  Returns zero on success, negative values on
 * failure.
 *
 */
int seccomp_arch_add(scmp_filter_ctx ctx, uint32_t arch_token);

/**
 * Removes an architecture from the filter
 * @param ctx the filter context
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 *
 * This function removes an architecture from the given seccomp filter context.
 * If the architecture token is SCMP_ARCH_NATIVE then the native architecture
 * will be assumed.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_arch_remove(scmp_filter_ctx ctx, uint32_t arch_token);

/**
 * Loads the filter into the kernel
 * @param ctx the filter context
 *
 * This function loads the given seccomp filter context into the kernel.  If
 * the filter was loaded correctly, the kernel will be enforcing the filter
 * when this function returns.  Returns zero on success, negative values on
 * error.
 *
 */
int seccomp_load(const scmp_filter_ctx ctx);

/**
 * Get the value of a filter attribute
 * @param ctx the filter context
 * @param attr the filter attribute name
 * @param value the filter attribute value
 *
 * This function fetches the value of the given attribute name and returns it
 * via @value.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_attr_get(const scmp_filter_ctx ctx,
		     enum scmp_filter_attr attr, uint32_t *value);

/**
 * Set the value of a filter attribute
 * @param ctx the filter context
 * @param attr the filter attribute name
 * @param value the filter attribute value
 *
 * This function sets the value of the given attribute.  Returns zero on
 * success, negative values on failure.
 *
 */
int seccomp_attr_set(scmp_filter_ctx ctx,
		     enum scmp_filter_attr attr, uint32_t value);

/**
 * Resolve a syscall number to a name
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 * @param num the syscall number
 *
 * Resolve the given syscall number to the syscall name for the given
 * architecture; it is up to the caller to free the returned string.  Returns
 * the syscall name on success, NULL on failure.
 *
 */
char *seccomp_syscall_resolve_num_arch(uint32_t arch_token, int num);

/**
 * Resolve a syscall name to a number
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 * @param name the syscall name
 *
 * Resolve the given syscall name to the syscall number for the given
 * architecture.  Returns the syscall number on success, including negative
 * pseudo syscall numbers (e.g. __PNR_*); returns __NR_SCMP_ERROR on failure.
 *
 */
int seccomp_syscall_resolve_name_arch(uint32_t arch_token, const char *name);

/**
 * Resolve a syscall name to a number and perform any rewriting necessary
 * @param arch_token the architecture token, e.g. SCMP_ARCH_*
 * @param name the syscall name
 *
 * Resolve the given syscall name to the syscall number for the given
 * architecture and do any necessary syscall rewriting needed by the
 * architecture.  Returns the syscall number on success, including negative
 * pseudo syscall numbers (e.g. __PNR_*); returns __NR_SCMP_ERROR on failure.
 *
 */
int seccomp_syscall_resolve_name_rewrite(uint32_t arch_token, const char *name);

/**
 * Resolve a syscall name to a number
 * @param name the syscall name
 *
 * Resolve the given syscall name to the syscall number.  Returns the syscall
 * number on success, including negative pseudo syscall numbers (e.g. __PNR_*);
 * returns __NR_SCMP_ERROR on failure.
 *
 */
int seccomp_syscall_resolve_name(const char *name);

/**
 * Set the priority of a given syscall
 * @param ctx the filter context
 * @param syscall the syscall number
 * @param priority priority value, higher value == higher priority
 *
 * This function sets the priority of the given syscall; this value is used
 * when generating the seccomp filter code such that higher priority syscalls
 * will incur less filter code overhead than the lower priority syscalls in the
 * filter.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_syscall_priority(scmp_filter_ctx ctx,
			     int syscall, uint8_t priority);

/**
 * Add a new rule to the filter
 * @param ctx the filter context
 * @param action the filter action
 * @param syscall the syscall number
 * @param arg_cnt the number of argument filters in the argument filter chain
 * @param ... scmp_arg_cmp structs (use of SCMP_ARG_CMP() recommended)
 *
 * This function adds a series of new argument/value checks to the seccomp
 * filter for the given syscall; multiple argument/value checks can be
 * specified and they will be chained together (AND'd together) in the filter.
 * If the specified rule needs to be adjusted due to architecture specifics it
 * will be adjusted without notification.  Returns zero on success, negative
 * values on failure.
 *
 */
int seccomp_rule_add(scmp_filter_ctx ctx,
		     uint32_t action, int syscall, unsigned int arg_cnt, ...);


/**
 * Add a new rule to the filter
 * @param ctx the filter context
 * @param action the filter action
 * @param syscall the syscall number
 * @param arg_cnt the number of elements in the arg_array parameter
 * @param arg_array array of scmp_arg_cmp structs
 *
 * This function adds a series of new argument/value checks to the seccomp
 * filter for the given syscall; multiple argument/value checks can be
 * specified and they will be chained together (AND'd together) in the filter.
 * If the specified rule needs to be adjusted due to architecture specifics it
 * will be adjusted without notification.  Returns zero on success, negative
 * values on failure.
 *
 */
int seccomp_rule_add_array(scmp_filter_ctx ctx,
			   uint32_t action, int syscall, unsigned int arg_cnt,
			   const struct scmp_arg_cmp *arg_array);

/**
 * Add a new rule to the filter
 * @param ctx the filter context
 * @param action the filter action
 * @param syscall the syscall number
 * @param arg_cnt the number of argument filters in the argument filter chain
 * @param ... scmp_arg_cmp structs (use of SCMP_ARG_CMP() recommended)
 *
 * This function adds a series of new argument/value checks to the seccomp
 * filter for the given syscall; multiple argument/value checks can be
 * specified and they will be chained together (AND'd together) in the filter.
 * If the specified rule can not be represented on the architecture the
 * function will fail.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_rule_add_exact(scmp_filter_ctx ctx, uint32_t action,
			   int syscall, unsigned int arg_cnt, ...);

/**
 * Add a new rule to the filter
 * @param ctx the filter context
 * @param action the filter action
 * @param syscall the syscall number
 * @param arg_cnt  the number of elements in the arg_array parameter
 * @param arg_array array of scmp_arg_cmp structs
 *
 * This function adds a series of new argument/value checks to the seccomp
 * filter for the given syscall; multiple argument/value checks can be
 * specified and they will be chained together (AND'd together) in the filter.
 * If the specified rule can not be represented on the architecture the
 * function will fail.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_rule_add_exact_array(scmp_filter_ctx ctx,
				 uint32_t action, int syscall,
				 unsigned int arg_cnt,
				 const struct scmp_arg_cmp *arg_array);

/**
 * Generate seccomp Pseudo Filter Code (PFC) and export it to a file
 * @param ctx the filter context
 * @param fd the destination fd
 *
 * This function generates seccomp Pseudo Filter Code (PFC) and writes it to
 * the given fd.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_export_pfc(const scmp_filter_ctx ctx, int fd);

/**
 * Generate seccomp Berkley Packet Filter (BPF) code and export it to a file
 * @param ctx the filter context
 * @param fd the destination fd
 *
 * This function generates seccomp Berkley Packer Filter (BPF) code and writes
 * it to the given fd.  Returns zero on success, negative values on failure.
 *
 */
int seccomp_export_bpf(const scmp_filter_ctx ctx, int fd);

/*
 * pseudo syscall definitions
 */

/* NOTE - pseudo syscall values {-1..-99} are reserved */
#define __NR_SCMP_ERROR		-1
#define __NR_SCMP_UNDEF		-2

/* socket syscalls */

#define __PNR_socket		-101
#ifndef __NR_socket
#define __NR_socket		__PNR_socket
#endif /* __NR_socket */

#define __PNR_bind		-102
#ifndef __NR_bind
#define __NR_bind		__PNR_bind
#endif /* __NR_bind */

#define __PNR_connect		-103
#ifndef __NR_connect
#define __NR_connect		__PNR_connect
#endif /* __NR_connect */

#define __PNR_listen		-104
#ifndef __NR_listen
#define __NR_listen		__PNR_listen
#endif /* __NR_listen */

#define __PNR_accept		-105
#ifndef __NR_accept
#define __NR_accept		__PNR_accept
#endif /* __NR_accept */

#define __PNR_getsockname	-106
#ifndef __NR_getsockname
#define __NR_getsockname	__PNR_getsockname
#endif /* __NR_getsockname */

#define __PNR_getpeername	-107
#ifndef __NR_getpeername
#define __NR_getpeername	__PNR_getpeername
#endif /* __NR_getpeername */

#define __PNR_socketpair	-108
#ifndef __NR_socketpair
#define __NR_socketpair		__PNR_socketpair
#endif /* __NR_socketpair */

#define __PNR_send		-109
#ifndef __NR_send
#define __NR_send		__PNR_send
#endif /* __NR_send */

#define __PNR_recv		-110
#ifndef __NR_recv
#define __NR_recv		__PNR_recv
#endif /* __NR_recv */

#define __PNR_sendto		-111
#ifndef __NR_sendto
#define __NR_sendto		__PNR_sendto
#endif /* __NR_sendto */

#define __PNR_recvfrom		-112
#ifndef __NR_recvfrom
#define __NR_recvfrom		__PNR_recvfrom
#endif /* __NR_recvfrom */

#define __PNR_shutdown		-113
#ifndef __NR_shutdown
#define __NR_shutdown		__PNR_shutdown
#endif /* __NR_shutdown */

#define __PNR_setsockopt	-114
#ifndef __NR_setsockopt
#define __NR_setsockopt		__PNR_setsockopt
#endif /* __NR_getsockopt */

#define __PNR_getsockopt	-115
#ifndef __NR_getsockopt
#define __NR_getsockopt		__PNR_getsockopt
#endif /* __NR_getsockopt */

#define __PNR_sendmsg		-116
#ifndef __NR_sendmsg
#define __NR_sendmsg		__PNR_sendmsg
#endif /* __NR_sendmsg */

#define __PNR_recvmsg		-117
#ifndef __NR_recvmsg
#define __NR_recvmsg		__PNR_recvmsg
#endif /* __NR_recvmsg */

#define __PNR_accept4		-118
#ifndef __NR_accept4
#define __NR_accept4		__PNR_accept4
#endif /* __NR_accept4 */

#define __PNR_recvmmsg		-119
#ifndef __NR_recvmmsg
#define __NR_recvmmsg		__PNR_recvmmsg
#endif /* __NR_recvmmsg */

#define __PNR_sendmmsg		-120
#ifndef __NR_sendmmsg
#define __NR_sendmmsg		__PNR_sendmmsg
#endif /* __NR_sendmmsg */

/* ipc syscalls */

#define __PNR_semop		-201
#ifndef __NR_semop
#define __NR_semop		__PNR_semop
#endif /* __NR_semop */

#define __PNR_semget		-202
#ifndef __NR_semget
#define __NR_semget		__PNR_semget
#endif /* __NR_semget */

#define __PNR_semctl		-203
#ifndef __NR_semctl
#define __NR_semctl		__PNR_semctl
#endif /* __NR_semctl */

#define __PNR_semtimedop	-204
#ifndef __NR_semtimedop
#define __NR_semtimedop		__PNR_semtimedop
#endif /* __NR_semtime */

#define __PNR_msgsnd		-211
#ifndef __NR_msgsnd
#define __NR_msgsnd		__PNR_msgsnd
#endif /* __NR_msgsnd */

#define __PNR_msgrcv		-212
#ifndef __NR_msgrcv
#define __NR_msgrcv		__PNR_msgrcv
#endif /* __NR_msgrcv */

#define __PNR_msgget		-213
#ifndef __NR_msgget
#define __NR_msgget		__PNR_msgget
#endif /* __NR_msgget */

#define __PNR_msgctl		-214
#ifndef __NR_msgctl
#define __NR_msgctl		__PNR_msgctl
#endif /* __NR_msgctl */

#define __PNR_shmat		-221
#ifndef __NR_shmat
#define __NR_shmat		__PNR_shmat
#endif /* __NR_shmat */

#define __PNR_shmdt		-222
#ifndef __NR_shmdt
#define __NR_shmdt		__PNR_shmdt
#endif /* __NR_shmdt */

#define __PNR_shmget		-223
#ifndef __NR_shmget
#define __NR_shmget		__PNR_shmget
#endif /* __NR_shmget */

#define __PNR_shmctl		-224
#ifndef __NR_shmctl
#define __NR_shmctl		__PNR_shmctl
#endif /* __NR_shmctl */

/* single syscalls */

#define __PNR_arch_prctl	-10001
#ifndef __NR_arch_prctl
#define __NR_arch_prctl		__PNR_arch_prctl
#endif /* __NR_arch_prctl */

#define __PNR_bdflush		-10002
#ifndef __NR_bdflush
#define __NR_bdflush		__PNR_bdflush
#endif /* __NR_bdflush */

#define __PNR_break		-10003
#ifndef __NR_break
#define __NR_break		__PNR_break
#endif /* __NR_break */

#define __PNR_chown32		-10004
#ifndef __NR_chown32
#define __NR_chown32		__PNR_chown32
#endif /* __NR_chown32 */

#define __PNR_epoll_ctl_old	-10005
#ifndef __NR_epoll_ctl_old
#define __NR_epoll_ctl_old	__PNR_epoll_ctl_old
#endif /* __NR_epoll_ctl_old */

#define __PNR_epoll_wait_old	-10006
#ifndef __NR_epoll_wait_old
#define __NR_epoll_wait_old	__PNR_epoll_wait_old
#endif /* __NR_epoll_wait_old */

#define __PNR_fadvise64_64	-10007
#ifndef __NR_fadvise64_64
#define __NR_fadvise64_64	__PNR_fadvise64_64
#endif /* __NR_fadvise64_64 */

#define __PNR_fchown32		-10008
#ifndef __NR_fchown32
#define __NR_fchown32		__PNR_fchown32
#endif /* __NR_fchown32 */

#define __PNR_fcntl64		-10009
#ifndef __NR_fcntl64
#define __NR_fcntl64		__PNR_fcntl64
#endif /* __NR_fcntl64 */

#define __PNR_fstat64		-10010
#ifndef __NR_fstat64
#define __NR_fstat64		__PNR_fstat64
#endif /* __NR_fstat64 */

#define __PNR_fstatat64		-10011
#ifndef __NR_fstatat64
#define __NR_fstatat64		__PNR_fstatat64
#endif /* __NR_fstatat64 */

#define __PNR_fstatfs64		-10012
#ifndef __NR_fstatfs64
#define __NR_fstatfs64		__PNR_fstatfs64
#endif /* __NR_fstatfs64 */

#define __PNR_ftime		-10013
#ifndef __NR_ftime
#define __NR_ftime		__PNR_ftime
#endif /* __NR_ftime */

#define __PNR_ftruncate64	-10014
#ifndef __NR_ftruncate64
#define __NR_ftruncate64	__PNR_ftruncate64
#endif /* __NR_ftruncate64 */

#define __PNR_getegid32		-10015
#ifndef __NR_getegid32
#define __NR_getegid32		__PNR_getegid32
#endif /* __NR_getegid32 */

#define __PNR_geteuid32		-10016
#ifndef __NR_geteuid32
#define __NR_geteuid32		__PNR_geteuid32
#endif /* __NR_geteuid32 */

#define __PNR_getgid32		-10017
#ifndef __NR_getgid32
#define __NR_getgid32		__PNR_getgid32
#endif /* __NR_getgid32 */

#define __PNR_getgroups32	-10018
#ifndef __NR_getgroups32
#define __NR_getgroups32	__PNR_getgroups32
#endif /* __NR_getgroups32 */

#define __PNR_getresgid32	-10019
#ifndef __NR_getresgid32
#define __NR_getresgid32	__PNR_getresgid32
#endif /* __NR_getresgid32 */

#define __PNR_getresuid32	-10020
#ifndef __NR_getresuid32
#define __NR_getresuid32	__PNR_getresuid32
#endif /* __NR_getresuid32 */

#define __PNR_getuid32		-10021
#ifndef __NR_getuid32
#define __NR_getuid32		__PNR_getuid32
#endif /* __NR_getuid32 */

#define __PNR_gtty		-10022
#ifndef __NR_gtty
#define __NR_gtty		__PNR_gtty
#endif /* __NR_gtty */

#define __PNR_idle		-10023
#ifndef __NR_idle
#define __NR_idle		__PNR_idle
#endif /* __NR_idle */

#define __PNR_ipc		-10024
#ifndef __NR_ipc
#define __NR_ipc		__PNR_ipc
#endif /* __NR_ipc */

#define __PNR_lchown32		-10025
#ifndef __NR_lchown32
#define __NR_lchown32		__PNR_lchown32
#endif /* __NR_lchown32 */

#define __PNR__llseek		-10026
#ifndef __NR__llseek
#define __NR__llseek		__PNR__llseek
#endif /* __NR__llseek */

#define __PNR_lock		-10027
#ifndef __NR_lock
#define __NR_lock		__PNR_lock
#endif /* __NR_lock */

#define __PNR_lstat64		-10028
#ifndef __NR_lstat64
#define __NR_lstat64		__PNR_lstat64
#endif /* __NR_lstat64 */

#define __PNR_mmap2		-10029
#ifndef __NR_mmap2
#define __NR_mmap2		__PNR_mmap2
#endif /* __NR_mmap2 */

#define __PNR_mpx		-10030
#ifndef __NR_mpx
#define __NR_mpx		__PNR_mpx
#endif /* __NR_mpx */

#define __PNR_newfstatat	-10031
#ifndef __NR_newfstatat
#define __NR_newfstatat		__PNR_newfstatat
#endif /* __NR_newfstatat */

#define __PNR__newselect	-10032
#ifndef __NR__newselect
#define __NR__newselect		__PNR__newselect
#endif /* __NR__newselect */

#define __PNR_nice		-10033
#ifndef __NR_nice
#define __NR_nice		__PNR_nice
#endif /* __NR_nice */

#define __PNR_oldfstat		-10034
#ifndef __NR_oldfstat
#define __NR_oldfstat		__PNR_oldfstat
#endif /* __NR_oldfstat */

#define __PNR_oldlstat		-10035
#ifndef __NR_oldlstat
#define __NR_oldlstat		__PNR_oldlstat
#endif /* __NR_oldlstat */

#define __PNR_oldolduname	-10036
#ifndef __NR_oldolduname
#define __NR_oldolduname	__PNR_oldolduname
#endif /* __NR_oldolduname */

#define __PNR_oldstat		-10037
#ifndef __NR_oldstat
#define __NR_oldstat		__PNR_oldstat
#endif /* __NR_oldstat */

#define __PNR_olduname		-10038
#ifndef __NR_olduname
#define __NR_olduname		__PNR_olduname
#endif /* __NR_olduname */

#define __PNR_prof		-10039
#ifndef __NR_prof
#define __NR_prof		__PNR_prof
#endif /* __NR_prof */

#define __PNR_profil		-10040
#ifndef __NR_profil
#define __NR_profil		__PNR_profil
#endif /* __NR_profil */

#define __PNR_readdir		-10041
#ifndef __NR_readdir
#define __NR_readdir		__PNR_readdir
#endif /* __NR_readdir */

#define __PNR_security		-10042
#ifndef __NR_security
#define __NR_security		__PNR_security
#endif /* __NR_security */

#define __PNR_sendfile64	-10043
#ifndef __NR_sendfile64
#define __NR_sendfile64		__PNR_sendfile64
#endif /* __NR_sendfile64 */

#define __PNR_setfsgid32	-10044
#ifndef __NR_setfsgid32
#define __NR_setfsgid32		__PNR_setfsgid32
#endif /* __NR_setfsgid32 */

#define __PNR_setfsuid32	-10045
#ifndef __NR_setfsuid32
#define __NR_setfsuid32		__PNR_setfsuid32
#endif /* __NR_setfsuid32 */

#define __PNR_setgid32		-10046
#ifndef __NR_setgid32
#define __NR_setgid32		__PNR_setgid32
#endif /* __NR_setgid32 */

#define __PNR_setgroups32	-10047
#ifndef __NR_setgroups32
#define __NR_setgroups32	__PNR_setgroups32
#endif /* __NR_setgroups32 */

#define __PNR_setregid32	-10048
#ifndef __NR_setregid32
#define __NR_setregid32		__PNR_setregid32
#endif /* __NR_setregid32 */

#define __PNR_setresgid32	-10049
#ifndef __NR_setresgid32
#define __NR_setresgid32	__PNR_setresgid32
#endif /* __NR_setresgid32 */

#define __PNR_setresuid32	-10050
#ifndef __NR_setresuid32
#define __NR_setresuid32	__PNR_setresuid32
#endif /* __NR_setresuid32 */

#define __PNR_setreuid32	-10051
#ifndef __NR_setreuid32
#define __NR_setreuid32		__PNR_setreuid32
#endif /* __NR_setreuid32 */

#define __PNR_setuid32		-10052
#ifndef __NR_setuid32
#define __NR_setuid32		__PNR_setuid32
#endif /* __NR_setuid32 */

#define __PNR_sgetmask		-10053
#ifndef __NR_sgetmask
#define __NR_sgetmask		__PNR_sgetmask
#endif /* __NR_sgetmask */

#define __PNR_sigaction		-10054
#ifndef __NR_sigaction
#define __NR_sigaction		__PNR_sigaction
#endif /* __NR_sigaction */

#define __PNR_signal		-10055
#ifndef __NR_signal
#define __NR_signal		__PNR_signal
#endif /* __NR_signal */

#define __PNR_sigpending	-10056
#ifndef __NR_sigpending
#define __NR_sigpending		__PNR_sigpending
#endif /* __NR_sigpending */

#define __PNR_sigprocmask	-10057
#ifndef __NR_sigprocmask
#define __NR_sigprocmask	__PNR_sigprocmask
#endif /* __NR_sigprocmask */

#define __PNR_sigreturn		-10058
#ifndef __NR_sigreturn
#define __NR_sigreturn		__PNR_sigreturn
#endif /* __NR_sigreturn */

#define __PNR_sigsuspend	-10059
#ifndef __NR_sigsuspend
#define __NR_sigsuspend		__PNR_sigsuspend
#endif /* __NR_sigsuspend */

#define __PNR_socketcall	-10060
#ifndef __NR_socketcall
#define __NR_socketcall		__PNR_socketcall
#endif /* __NR_socketcall */

#define __PNR_ssetmask		-10061
#ifndef __NR_ssetmask
#define __NR_ssetmask		__PNR_ssetmask
#endif /* __NR_ssetmask */

#define __PNR_stat64		-10062
#ifndef __NR_stat64
#define __NR_stat64		__PNR_stat64
#endif /* __NR_stat64 */

#define __PNR_statfs64		-10063
#ifndef __NR_statfs64
#define __NR_statfs64		__PNR_statfs64
#endif /* __NR_statfs64 */

#define __PNR_stime		-10064
#ifndef __NR_stime
#define __NR_stime		__PNR_stime
#endif /* __NR_stime */

#define __PNR_stty		-10065
#ifndef __NR_stty
#define __NR_stty		__PNR_stty
#endif /* __NR_stty */

#define __PNR_truncate64	-10066
#ifndef __NR_truncate64
#define __NR_truncate64		__PNR_truncate64
#endif /* __NR_truncate64 */

#define __PNR_tuxcall		-10067
#ifndef __NR_tuxcall
#define __NR_tuxcall		__PNR_tuxcall
#endif /* __NR_tuxcall */

#define __PNR_ugetrlimit	-10068
#ifndef __NR_ugetrlimit
#define __NR_ugetrlimit		__PNR_ugetrlimit
#endif /* __NR_ugetrlimit */

#define __PNR_ulimit		-10069
#ifndef __NR_ulimit
#define __NR_ulimit		__PNR_ulimit
#endif /* __NR_ulimit */

#define __PNR_umount		-10070
#ifndef __NR_umount
#define __NR_umount		__PNR_umount
#endif /* __NR_umount */

#define __PNR_vm86		-10071
#ifndef __NR_vm86
#define __NR_vm86		__PNR_vm86
#endif /* __NR_vm86 */

#define __PNR_vm86old		-10072
#ifndef __NR_vm86old
#define __NR_vm86old		__PNR_vm86old
#endif /* __NR_vm86old */

#define __PNR_waitpid		-10073
#ifndef __NR_waitpid
#define __NR_waitpid		__PNR_waitpid
#endif /* __NR_waitpid */

#define __PNR_create_module	-10074
#ifndef __NR_create_module
#define __NR_create_module	__PNR_create_module
#endif /* __NR_create_module */

#define __PNR_get_kernel_syms	-10075
#ifndef __NR_get_kernel_syms
#define __NR_get_kernel_syms	__PNR_get_kernel_syms
#endif /* __NR_get_kernel_syms */

#define __PNR_get_thread_area	-10076
#ifndef __NR_get_thread_area
#define __NR_get_thread_area	__PNR_get_thread_area
#endif /* __NR_get_thread_area */

#define __PNR_nfsservctl	-10077
#ifndef __NR_nfsservctl
#define __NR_nfsservctl		__PNR_nfsservctl
#endif /* __NR_nfsservctl */

#define __PNR_query_module	-10078
#ifndef __NR_query_module
#define __NR_query_module __PNR_query_module
#endif /* __NR_query_module */

#define __PNR_set_thread_area	-10079
#ifndef __NR_set_thread_area
#define __NR_set_thread_area	__PNR_set_thread_area
#endif /* __NR_set_thread_area */

#define __PNR__sysctl		-10080
#ifndef __NR__sysctl
#define __NR__sysctl		__PNR__sysctl
#endif /* __NR__sysctl */

#define __PNR_uselib		-10081
#ifndef __NR_uselib
#define __NR_uselib		__PNR_uselib
#endif /* __NR_uselib */

#define __PNR_vserver		-10082
#ifndef __NR_vserver
#define __NR_vserver		__PNR_vserver
#endif /* __NR_vserver */

#define __PNR_arm_fadvise64_64	-10083
#ifndef __NR_arm_fadvise64_64
#define __NR_arm_fadvise64_64	__PNR_arm_fadvise64_64
#endif /* __NR_arm_fadvise64_64 */

#define __PNR_arm_sync_file_range	-10084
#ifndef __NR_arm_sync_file_range
#define __NR_arm_sync_file_range	__PNR_arm_sync_file_range
#endif /* __NR_arm_sync_file_range */

#define __PNR_pciconfig_iobase	-10086
#ifndef __NR_pciconfig_iobase
#define __NR_pciconfig_iobase	__PNR_pciconfig_iobase
#endif /* __NR_pciconfig_iobase */

#define __PNR_pciconfig_read	-10087
#ifndef __NR_pciconfig_read
#define __NR_pciconfig_read	__PNR_pciconfig_read
#endif /* __NR_pciconfig_read */

#define __PNR_pciconfig_write	-10088
#ifndef __NR_pciconfig_write
#define __NR_pciconfig_write	__PNR_pciconfig_write
#endif /* __NR_pciconfig_write */

#define __PNR_sync_file_range2	-10089
#ifndef __NR_sync_file_range2
#define __NR_sync_file_range2	__PNR_sync_file_range2
#endif /* __NR_sync_file_range2 */

#define __PNR_syscall		-10090
#ifndef __NR_syscall
#define __NR_syscall		__PNR_syscall
#endif /* __NR_syscall */

#define __PNR_afs_syscall	-10091
#ifndef __NR_afs_syscall
#define __NR_afs_syscall	__PNR_afs_syscall
#endif /* __NR_afs_syscall */

#define __PNR_fadvise64		-10092
#ifndef __NR_fadvise64
#define __NR_fadvise64		__PNR_fadvise64
#endif /* __NR_fadvise64 */

#define __PNR_getpmsg		-10093
#ifndef __NR_getpmsg
#define __NR_getpmsg		__PNR_getpmsg
#endif /* __NR_getpmsg */

#define __PNR_ioperm		-10094
#ifndef __NR_ioperm
#define __NR_ioperm		__PNR_ioperm
#endif /* __NR_ioperm */

#define __PNR_iopl		-10095
#ifndef __NR_iopl
#define __NR_iopl		__PNR_iopl
#endif /* __NR_iopl */

#define __PNR_migrate_pages	-10097
#ifndef __NR_migrate_pages
#define __NR_migrate_pages	__PNR_migrate_pages
#endif /* __NR_migrate_pages */

#define __PNR_modify_ldt	-10098
#ifndef __NR_modify_ldt
#define __NR_modify_ldt		__PNR_modify_ldt
#endif /* __NR_modify_ldt */

#define __PNR_putpmsg		-10099
#ifndef __NR_putpmsg
#define __NR_putpmsg		__PNR_putpmsg
#endif /* __NR_putpmsg */

#define __PNR_sync_file_range	-10100
#ifndef __NR_sync_file_range
#define __NR_sync_file_range	__PNR_sync_file_range
#endif /* __NR_sync_file_range */

#define __PNR_select		-10101
#ifndef __NR_select
#define __NR_select		__PNR_select
#endif /* __NR_select */

#define __PNR_vfork		-10102
#ifndef __NR_vfork
#define __NR_vfork		__PNR_vfork
#endif /* __NR_vfork */

#define __PNR_cachectl		-10103
#ifndef __NR_cachectl
#define __NR_cachectl		__PNR_cachectl
#endif /* __NR_cachectl */

#define __PNR_cacheflush	-10104
#ifndef __NR_cacheflush
#ifdef __ARM_NR_cacheflush
#define __NR_cacheflush		__ARM_NR_cacheflush
#else
#define __NR_cacheflush		__PNR_cacheflush
#endif
#endif /* __NR_cacheflush */

#define __PNR_sysmips		-10106
#ifndef __NR_sysmips
#define __NR_sysmips		__PNR_sysmips
#endif /* __NR_sysmips */

#define __PNR_timerfd		-10107
#ifndef __NR_timerfd
#define __NR_timerfd		__PNR_timerfd
#endif /* __NR_timerfd */

#define __PNR_time		-10108
#ifndef __NR_time
#define __NR_time		__PNR_time
#endif /* __NR_time */

#define __PNR_getrandom		-10109
#ifndef __NR_getrandom
#define __NR_getrandom		__PNR_getrandom
#endif /* __NR_getrandom - NO LONGER NEEDED */

#define __PNR_memfd_create	-10110
#ifndef __NR_memfd_create
#define __NR_memfd_create	__PNR_memfd_create
#endif /* __NR_memfd_create - NO LONGER NEEDED */

#define __PNR_kexec_file_load	-10111
#ifndef __NR_kexec_file_load
#define __NR_kexec_file_load	__PNR_kexec_file_load
#endif /* __NR_kexec_file_load */

#define __PNR_sysfs		-10145
#ifndef __NR_sysfs
#define __NR_sysfs		__PNR_sysfs
#endif /* __NR_sysfs */

#define __PNR_oldwait4		-10146
#ifndef __NR_oldwait4
#define __NR_oldwait4		__PNR_oldwait4
#endif /* __NR_sysfs */

#define __PNR_access		-10147
#ifndef __NR_access
#define __NR_access		__PNR_access
#endif /* __NR_access */

#define __PNR_alarm		-10148
#ifndef __NR_alarm
#define __NR_alarm		__PNR_alarm
#endif /* __NR_alarm */

#define __PNR_chmod		-10149
#ifndef __NR_chmod
#define __NR_chmod		__PNR_chmod
#endif /* __NR_chmod */

#define __PNR_chown		-10150
#ifndef __NR_chown
#define __NR_chown		__PNR_chown
#endif /* __NR_chown */

#define __PNR_creat		-10151
#ifndef __NR_creat
#define __NR_creat		__PNR_creat
#endif /* __NR_creat */

#define __PNR_dup2		-10152
#ifndef __NR_dup2
#define __NR_dup2		__PNR_dup2
#endif /* __NR_dup2 */

#define __PNR_epoll_create	-10153
#ifndef __NR_epoll_create
#define __NR_epoll_create	__PNR_epoll_create
#endif /* __NR_epoll_create */

#define __PNR_epoll_wait	-10154
#ifndef __NR_epoll_wait
#define __NR_epoll_wait		__PNR_epoll_wait
#endif /* __NR_epoll_wait */

#define __PNR_eventfd		-10155
#ifndef __NR_eventfd
#define __NR_eventfd		__PNR_eventfd
#endif /* __NR_eventfd */

#define __PNR_fork		-10156
#ifndef __NR_fork
#define __NR_fork		__PNR_fork
#endif /* __NR_fork */

#define __PNR_futimesat		-10157
#ifndef __NR_futimesat
#define __NR_futimesat		__PNR_futimesat
#endif /* __NR_futimesat */

#define __PNR_getdents		-10158
#ifndef __NR_getdents
#define __NR_getdents		__PNR_getdents
#endif /* __NR_getdents */

#define __PNR_getpgrp		-10159
#ifndef __NR_getpgrp
#define __NR_getpgrp		__PNR_getpgrp
#endif /* __NR_getpgrp */

#define __PNR_inotify_init	-10160
#ifndef __NR_inotify_init
#define __NR_inotify_init	__PNR_inotify_init
#endif /* __NR_inotify_init */

#define __PNR_lchown		-10161
#ifndef __NR_lchown
#define __NR_lchown		__PNR_lchown
#endif /* __NR_lchown */

#define __PNR_link		-10162
#ifndef __NR_link
#define __NR_link		__PNR_link
#endif /* __NR_link */

#define __PNR_lstat		-10163
#ifndef __NR_lstat
#define __NR_lstat		__PNR_lstat
#endif /* __NR_lstat */

#define __PNR_mkdir		-10164
#ifndef __NR_mkdir
#define __NR_mkdir		__PNR_mkdir
#endif /* __NR_mkdir */

#define __PNR_mknod		-10165
#ifndef __NR_mknod
#define __NR_mknod		__PNR_mknod
#endif /* __NR_mknod */

#define __PNR_open		-10166
#ifndef __NR_open
#define __NR_open		__PNR_open
#endif /* __NR_open */

#define __PNR_pause		-10167
#ifndef __NR_pause
#define __NR_pause		__PNR_pause
#endif /* __NR_pause */

#define __PNR_pipe		-10168
#ifndef __NR_pipe
#define __NR_pipe		__PNR_pipe
#endif /* __NR_pipe */

#define __PNR_poll		-10169
#ifndef __NR_poll
#define __NR_poll		__PNR_poll
#endif /* __NR_poll */

#define __PNR_readlink		-10170
#ifndef __NR_readlink
#define __NR_readlink		__PNR_readlink
#endif /* __NR_readlink */

#define __PNR_rename		-10171
#ifndef __NR_rename
#define __NR_rename		__PNR_rename
#endif /* __NR_rename */

#define __PNR_rmdir		-10172
#ifndef __NR_rmdir
#define __NR_rmdir		__PNR_rmdir
#endif /* __NR_rmdir */

#define __PNR_signalfd		-10173
#ifndef __NR_signalfd
#define __NR_signalfd		__PNR_signalfd
#endif /* __NR_signalfd */

#define __PNR_stat		-10174
#ifndef __NR_stat
#define __NR_stat		__PNR_stat
#endif /* __NR_stat */

#define __PNR_symlink		-10175
#ifndef __NR_symlink
#define __NR_symlink		__PNR_symlink
#endif /* __NR_symlink */

#define __PNR_unlink		-10176
#ifndef __NR_unlink
#define __NR_unlink		__PNR_unlink
#endif /* __NR_unlink */

#define __PNR_ustat		-10177
#ifndef __NR_ustat
#define __NR_ustat		__PNR_ustat
#endif /* __NR_ustat */

#define __PNR_utime		-10178
#ifndef __NR_utime
#define __NR_utime		__PNR_utime
#endif /* __NR_utime */

#define __PNR_utimes		-10179
#ifndef __NR_utimes
#define __NR_utimes		__PNR_utimes
#endif /* __NR_utimes */

#define __PNR_getrlimit		-10180
#ifndef __NR_getrlimit
#define __NR_getrlimit		__PNR_getrlimit
#endif /* __NR_utimes */

#define __PNR_mmap		-10181
#ifndef __NR_mmap
#define __NR_mmap		__PNR_mmap
#endif /* __NR_utimes */

#define __PNR_breakpoint	-10182
#ifndef __NR_breakpoint
#ifdef __ARM_NR_breakpoint
#define __NR_breakpoint		__ARM_NR_breakpoint
#else
#define __NR_breakpoint		__PNR_breakpoint
#endif
#endif /* __NR_breakpoint */

#define __PNR_set_tls		-10183
#ifndef __NR_set_tls
#ifdef __ARM_NR_set_tls
#define __NR_set_tls		__ARM_NR_set_tls
#else
#define __NR_set_tls		__PNR_set_tls
#endif
#endif /* __NR_set_tls */

#define __PNR_usr26		-10184
#ifndef __NR_usr26
#ifdef __ARM_NR_usr26
#define __NR_usr26		__ARM_NR_usr26
#else
#define __NR_usr26		__PNR_usr26
#endif
#endif /* __NR_usr26 */

#define __PNR_usr32		-10185
#ifndef __NR_usr32
#ifdef __ARM_NR_usr32
#define __NR_usr32		__ARM_NR_usr32
#else
#define __NR_usr32		__PNR_usr32
#endif
#endif /* __NR_usr32 */

#define __PNR_multiplexer	-10186
#ifndef __NR_multiplexer
#define __NR_multiplexer	__PNR_multiplexer
#endif /* __NR_multiplexer */

#define __PNR_rtas		-10187
#ifndef __NR_rtas
#define __NR_rtas		__PNR_rtas
#endif /* __NR_rtas */

#define __PNR_spu_create	-10188
#ifndef __NR_spu_create
#define __NR_spu_create		__PNR_spu_create
#endif /* __NR_spu_create */

#define __PNR_spu_run		-10189
#ifndef __NR_spu_run
#define __NR_spu_run		__PNR_spu_run
#endif /* __NR_spu_run */

#define __PNR_subpage_prot	-10189
#ifndef __NR_subpage_prot
#define __NR_subpage_prot	__PNR_subpage_prot
#endif /* __NR_subpage_prot */

#define __PNR_swapcontext	-10190
#ifndef __NR_swapcontext
#define __NR_swapcontext	__PNR_swapcontext
#endif /* __NR_swapcontext */

#define __PNR_sys_debug_setcontext	-10191
#ifndef __NR_sys_debug_setcontext
#define __NR_sys_debug_setcontext	__PNR_sys_debug_setcontext
#endif /* __NR_sys_debug_setcontext */

#define __PNR_switch_endian	-10191
#ifndef __NR_switch_endian
#define __NR_switch_endian	__PNR_switch_endian
#endif /* __NR_switch_endian */

#define __PNR_get_mempolicy	-10192
#ifndef __NR_get_mempolicy
#define __NR_get_mempolicy	__PNR_get_mempolicy
#endif /* __NR_get_mempolicy */

#define __PNR_move_pages	-10193
#ifndef __NR_move_pages
#define __NR_move_pages		__PNR_move_pages
#endif /* __NR_move_pages */

#define __PNR_mbind		-10194
#ifndef __NR_mbind
#define __NR_mbind		__PNR_mbind
#endif /* __NR_mbind */

#define __PNR_set_mempolicy	-10195
#ifndef __NR_set_mempolicy
#define __NR_set_mempolicy	__PNR_set_mempolicy
#endif /* __NR_set_mempolicy */

#define __PNR_s390_runtime_instr	-10196
#ifndef __NR_s390_runtime_instr
#define __NR_s390_runtime_instr		__PNR_s390_runtime_instr
#endif /* __NR_s390_runtime_instr */

#define __PNR_s390_pci_mmio_read	-10197
#ifndef __NR_s390_pci_mmio_read
#define __NR_s390_pci_mmio_read		__PNR_s390_pci_mmio_read
#endif /* __NR_s390_pci_mmio_read */

#define __PNR_s390_pci_mmio_write	-10198
#ifndef __NR_s390_pci_mmio_write
#define __NR_s390_pci_mmio_write	__PNR_s390_pci_mmio_write
#endif /* __NR_s390_pci_mmio_write */

#define __PNR_membarrier	-10199
#ifndef __NR_membarrier
#define __NR_membarrier		__PNR_membarrier
#endif /* __NR_membarrier */

#define __PNR_userfaultfd	-10200
#ifndef __NR_userfaultfd
#define __NR_userfaultfd	__PNR_userfaultfd
#endif /* __NR_userfaultfd */

#ifdef __cplusplus
}
#endif

#endif
