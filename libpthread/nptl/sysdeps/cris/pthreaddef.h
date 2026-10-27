/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* Required stack pointer alignment at beginning.  */
#define STACK_ALIGN		16

/* Minimal stack size after allocating thread descriptor and guard size.  */
#define MINIMAL_REST_STACK	2048

/* The thread pointer is the thread descriptor and lives in the upper 24
   bits of $pid, so the descriptor must be 256-byte aligned.  */
#define TCB_ALIGNMENT		256

/* Location of current stack frame.  */
#define CURRENT_STACK_FRAME	__builtin_frame_address (0)

/* XXX Until we have a better place keep the definitions here.  */
#define __exit_thread_inline(val) \
  INLINE_SYSCALL (exit, 1, (val))
