#ifndef THREADS_INTERRUPT_H
#define THREADS_INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>

/* Interrupts on or off? */
enum intr_level {
	INTR_OFF,             /* Interrupts disabled. */
	INTR_ON               /* Interrupts enabled. */
};

enum intr_level intr_get_level (void);
enum intr_level intr_set_level (enum intr_level);
enum intr_level intr_enable (void);
enum intr_level intr_disable (void);

/* Interrupt stack frame. */
/**
 * rax 반환값 저장, 시스템 호출 번호 전달
 * rbx 호출 규약에서 보존되어야 하는 레지스터
 * rcx 반복 연산의 카운터, 함수 호출 4번째 인자
 * rdx 함수 호출 3번째 인자
 * rbp 스택 프레임 기준
 * rsi 함수 호출 2번째 인자, 소스 주소
 * rdi 함수 호출 1번째 인자, 목적지 주소
 * r8~r15 고급 데이터 처리 및 임시 저장소
 */
struct gp_registers {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi; // source index: 문자열 조작과 같은 연산에서 소스 주소 저장, 함수 호출 시 두 번째 인자 전달 (시스템 호출 시 사용)
	uint64_t rdi; // destination index: 문자열 조작과 같은 연산에서 목적지 주소 저장, 함수 호출 시 첫 번재 인자 전달 (시스템 호출 시 사용)
	uint64_t rbp; // base pointer: 스택 프레임의 기준 주소 저장, 함수 호출 시 지역 변수와 함수 인자 참조에 사용
	uint64_t rdx; // data register: 곱셈 나눗셈 I/O 연산 등 사용, 함수 호출 시 세 번째 인자 전달
	uint64_t rcx; // count register: 반복 연산에서 카운터로 사용, 함수 호출 시 네 번째 인자 전달
	uint64_t rbx; // base register: 일반 데이터 저장, 호출 규약에서 보존되어야 하는 레지스터
	uint64_t rax; // accumulator: 함수 반환 값 저장, 시스템 호출 번호 전달에 사용
} __attribute__((packed));

/**
 * 인터럽트 발생 시 CPU 상태와 인터럽트 정보를 저장하는 데 사용되는 데이터 구조
 * - CPU 상태를 복원하여 실행을 재개.
 * - 인터럽트 핸들러가 필요한 정보를 활용.
 * - 사용자 모드와 커널 모드 간 전환이 안전하게 이루어짐.
 */
struct intr_frame {
	/* Pushed by intr_entry in intr-stubs.S.
	   These are the interrupted task's saved registers. */
	struct gp_registers R; // 일반 레지스터 상태
	uint16_t es; // extra segment register
	uint16_t __pad1;
	uint32_t __pad2;
	uint16_t ds; // data segment register
	uint16_t __pad3;
	uint32_t __pad4;
	/* Pushed by intrNN_stub in intr-stubs.S. */
	uint64_t vec_no; /* Interrupt vector number. */
/* Sometimes pushed by the CPU,
   otherwise for consistency pushed as 0 by intrNN_stub.
   The CPU puts it just under `eip', but we move it here. */
	uint64_t error_code;
/* Pushed by the CPU.
   These are the interrupted task's saved registers. */
	uintptr_t rip; // instruction pointer (이전 실행 위치)
	uint16_t cs; // code segment register 
	uint16_t __pad5;
	uint32_t __pad6;
	uint64_t eflags; // CPU flag register (EFLAGS) (인터럽트 발생 시 CPU 상태 저장)
	uintptr_t rsp; // stack pointer (인터럽트 발생 시점 스택 정보
	uint16_t ss; // stack segment    사용자 모드와 커널 모드 간 전환 시 중요한 역할)
	uint16_t __pad7;
	uint32_t __pad8;
} __attribute__((packed));

typedef void intr_handler_func (struct intr_frame *);

void intr_init (void);
void intr_register_ext (uint8_t vec, intr_handler_func *, const char *name);
void intr_register_int (uint8_t vec, int dpl, enum intr_level,
                        intr_handler_func *, const char *name);
bool intr_context (void);
void intr_yield_on_return (void);

void intr_dump_frame (const struct intr_frame *);
const char *intr_name (uint8_t vec);

#endif /* threads/interrupt.h */
