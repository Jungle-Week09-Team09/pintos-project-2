#ifndef THREADS_VADDR_H
#define THREADS_VADDR_H

#include <debug.h>
#include <stdint.h>
#include <stdbool.h>

#include "threads/loader.h"

/* Functions and macros for working with virtual addresses.
 *
 * See pte.h for functions and macros specifically for x86
 * hardware page tables. */

#define BITMASK(SHIFT, CNT) (((1ul << (CNT)) - 1) << (SHIFT))

/* Page offset (bits 0:12). */
// Index of virtual address offset bits -> 0
// 가상 주소 오프셋 비트 인덱스 0
#define PGSHIFT 0                          /* Index of first offset bit. */
// Number of virtual address offset bits -> 12
// 가상 주소 오프셋 비트 수 12
#define PGBITS  12                         /* Number of offset bits. */
// Page size 2^12 == 4096
// 페이지 크기 2^12 == 4096
#define PGSIZE  (1 << PGBITS)              /* Bytes in a page. */
// Set the bits of the page offset to 1, rest to 0
// 페이지 오프셋을 1로 설정하고 나머지는 0으로 설정
#define PGMASK  BITMASK(PGSHIFT, PGBITS)   /* Page offset bits (0:12). */

// Offset within a Virtual Address Page.
// 가상 주소의 페이지 오프셋 추출
#define pg_ofs(va) ((uint64_t) (va) & PGMASK)
// return virtual address page no
// 가상 주소의 페이지 번호 반환
#define pg_no(va) ((uint64_t) (va) >> PGBITS)

/* Round up to nearest page boundary. */
// 가장 가까운 페이지 경계까지 반올림을 해 가상 주소를 반환
#define pg_round_up(va) ((void *) (((uint64_t) (va) + PGSIZE - 1) & ~PGMASK))

/* Round down to nearest page boundary. */
// 페이지 오프셋이 0으로 설정된 상태에서 가상 주소가 가리키는 가상 페이지의 시작을 반환
#define pg_round_down(va) (void *) ((uint64_t) (va) & ~PGMASK)

/* Kernel virtual address start */
// base address of kernel virtual address
// 커널 가상 메모리의 기본 시작 주소
#define KERN_BASE LOADER_KERN_BASE

/* User stack start */
// 유저 스택 시작 주소
#define USER_STACK 0x47480000

/* Returns true if VADDR is a user virtual address. */
// 사용자 가상 주소인지
#define is_user_vaddr(vaddr) (!is_kernel_vaddr((vaddr)))

/* Returns true if VADDR is a kernel virtual address. */
// 커널 가상 주소인지?
#define is_kernel_vaddr(vaddr) ((uint64_t)(vaddr) >= KERN_BASE)

// FIXME: add checking
/* Returns kernel virtual address at which physical address PADDR
 *  is mapped. */
// 물리적인 주소 에 해당하는 커널 가상 주소를 반환 (범위 확인 x)
#define ptov(paddr) ((void *) (((uint64_t) paddr) + KERN_BASE))

/* Returns physical address at which kernel virtual address VADDR
 * is mapped. */
// 가상 주소 에 해당하는 물리적인 주소를 반환
#define vtop(vaddr) \
({ \
	ASSERT(is_kernel_vaddr(vaddr)); \
	((uint64_t) (vaddr) - (uint64_t) KERN_BASE);\
})

#endif /* threads/vaddr.h */
