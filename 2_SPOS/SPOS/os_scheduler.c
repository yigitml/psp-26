/*! \file
 *  \brief Scheduling module for the OS.
 *
 * Contains everything needed to realise the scheduling between multiple processes.
 * Also contains functions to start the execution of programs.
 *
 *  \author Lehrstuhl Informatik 11 - RWTH Aachen
 */

#include "os_scheduler.h"

#include "lcd.h"
#include "os_core.h"
#include "os_input.h"
#include "os_scheduling_strategies.h"
#include "os_taskman.h"
#include "util.h"

#include <avr/interrupt.h>
#include <stdbool.h>

//----------------------------------------------------------------------------
// Private Types
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//! Array of states for every possible process
Process os_processes[MAX_NUMBER_OF_PROCESSES];

//! Index of process that is currently executed (default: idle)
ProcessID currentProc = 0;

//----------------------------------------------------------------------------
// Private variables
//----------------------------------------------------------------------------

//! Currently active scheduling strategy
SchedulingStrategy currentStrategy = OS_SS_EVEN;

//! Count of currently nested critical sections
uint8_t criticalSectionCount = 0;

//----------------------------------------------------------------------------
// Private function declarations
//----------------------------------------------------------------------------

//! ISR for timer compare match (scheduler)
ISR(TIMER2_COMPA_vect)
__attribute__((naked));

//----------------------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------------------

/*!
 *  Timer interrupt that implements our scheduler. Execution of the running
 *  process is suspended and the context saved to the stack. Then the periphery
 *  is scanned for any input events. If everything is in order, the next process
 *  for execution is derived with an exchangeable strategy. Finally the
 *  scheduler restores the next process for execution and releases control over
 *  the processor to that process.
 */
ISR(TIMER2_COMPA_vect) {
	saveContext();
	os_processes[currentProc].sp.as_int = SP;
	SP = BOTTOM_OF_ISR_STACK;
	
	 if ((os_getInput() & 0b00001001) == 0b00001001) {
		 os_waitForNoInput();
		 os_taskManMain();
	 }
	 
	 if (os_processes[currentProc].state == OS_PS_RUNNING) {
		 os_processes[currentProc].state = OS_PS_READY;
	 }
	
	switch (currentStrategy) {
		case OS_SS_EVEN:
			currentProc = os_Scheduler_Even(os_processes, currentProc);
			break;
		case OS_SS_INACTIVE_AGING:
			currentProc = os_Scheduler_InactiveAging(os_processes, currentProc);
			break;
		case OS_SS_RANDOM:
			currentProc = os_Scheduler_Random(os_processes, currentProc);
			break;
		case OS_SS_ROUND_ROBIN:
			currentProc = os_Scheduler_RoundRobin(os_processes, currentProc);
			break;
		case OS_SS_RUN_TO_COMPLETION:
			currentProc = os_Scheduler_RunToCompletion(os_processes, currentProc);
			break;
		default:
			break;
	}
	
	 if (os_processes[currentProc].checksum != os_getStackChecksum(currentProc)) {
		 os_error("Stack inconsistent");
	 }
	
	os_processes[currentProc].state = OS_PS_RUNNING;
	SP = os_processes[currentProc].sp.as_int;
	restoreContext();
}

/*!
 *  This is the idle program. The idle process owns all the memory
 *  and processor time no other process wants to have.
 */
void idle(void){
	while(1){
		lcd_writeChar('.');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

/*!
 *  This function is used to register the given program for execution.
 *  A stack will be provided if the process limit has not yet been reached.
 *  This function is multitasking safe. That means that programs can repost
 *  themselves, simulating TinyOS 2 scheduling (just kick off interrupts ;) ).
 *
 *  \param program  The function of the program to start.
 *  \param priority A priority ranging 0..255 for the new process:
 *                   - 0 means least favourable
 *                   - 255 means most favourable
 *                  Note that the priority may be ignored by certain scheduling
 *                  strategies.
 *  \return The index of the new process or INVALID_PROCESS as specified in
 *          defines.h on failure
 */
ProcessID os_exec(Program *program, Priority priority) {
	os_enterCriticalSection();
	
	
    if (program == NULL){ 
		os_leaveCriticalSection();
		return INVALID_PROCESS;
	} 
	
    ProcessID nextEmptyIndex = 0;
    while (nextEmptyIndex < MAX_NUMBER_OF_PROCESSES && os_processes[nextEmptyIndex].state != OS_PS_UNUSED){
		nextEmptyIndex++;
	}
	
	if (nextEmptyIndex == MAX_NUMBER_OF_PROCESSES) {
		os_leaveCriticalSection();
		return INVALID_PROCESS;
	}
		
            os_processes[nextEmptyIndex].program = program;
            os_processes[nextEmptyIndex].priority = priority;

			
			os_processes[nextEmptyIndex].sp.as_int = PROCESS_STACK_BOTTOM(nextEmptyIndex);
			uint16_t progAdress = (uint16_t) program;
			
			 // Jede Speicherstelle nur 8 Bits => in zwei teilen
			 uint8_t lowBits = (uint8_t) progAdress;
			 uint8_t highBits = (uint8_t) (progAdress >> 8);
			 
			 *os_processes[nextEmptyIndex].sp.as_ptr = lowBits;
			 os_processes[nextEmptyIndex].sp.as_ptr--;
			 *os_processes[nextEmptyIndex].sp.as_ptr = highBits;
			 os_processes[nextEmptyIndex].sp.as_ptr--;
			 
			 for (int i = 1; i <= 33; i++) {
				 *os_processes[nextEmptyIndex].sp.as_ptr = 0;
				 os_processes[nextEmptyIndex].sp.as_ptr--;
			 }

			os_processes[nextEmptyIndex].checksum = os_getStackChecksum(nextEmptyIndex);
			os_processes[nextEmptyIndex].state = OS_PS_READY;
			os_leaveCriticalSection();
			return nextEmptyIndex;
			 
}

/*!
 *  If all processes have been registered for execution, the OS calls this
 *  function to start the idle program and the concurrent execution of the
 *  applications.
 */
void os_startScheduler(void) {
	currentProc = 0;
	os_processes[currentProc].state = OS_PS_RUNNING;
	//3. Setzen des Stackpointers auf den Prozessstack des Leerlaufprozesses.
	SP = os_processes[currentProc].sp.as_int;
	restoreContext();
	
}

/*!
 *  In order for the Scheduler to work properly, it must have the chance to
 *  initialize its internal data-structures and register.
 */
void os_initScheduler(void){
	for (ProcessID i = 0; i < MAX_NUMBER_OF_PROCESSES; i++){
		os_processes[i].state = OS_PS_UNUSED;
	}
	os_exec(idle, DEFAULT_PRIORITY);
	
	for (struct program_linked_list_node *node = autostart_head; node != NULL; node = node->next) {
		os_exec(node->program, DEFAULT_PRIORITY);
	}
	
}

/*!
 *  A simple getter for the slot of a specific process.
 *
 *  \param pid The processID of the process to be handled
 *  \return A pointer to the memory of the process at position pid in the os_processes array.
 */
Process *os_getProcessSlot(ProcessID pid) {
    return os_processes + pid;
}

/*!
 *  A simple getter to retrieve the currently active process.
 *
 *  \return The process id of the currently active process.
 */
ProcessID os_getCurrentProc(void) {
    return currentProc;
}

/*!
 *  Sets the current scheduling strategy.
 *
 *  \param strategy The strategy that will be used after the function finishes.
 */
void os_setSchedulingStrategy(SchedulingStrategy strategy){
	currentStrategy = strategy;
}

/*!
 *  This is a getter for retrieving the current scheduling strategy.
 *
 *  \return The current scheduling strategy.
 */
SchedulingStrategy os_getSchedulingStrategy(void) {
    return currentStrategy;
}

/*!
 *  Enters a critical code section by disabling the scheduler if needed.
 *  This function stores the nesting depth of critical sections of the current
 *  process (e.g. if a function with a critical section is called from another
 *  critical section) to ensure correct behaviour when leaving the section.
 *  This function supports up to 255 nested critical sections.
 */
void os_enterCriticalSection(void) {
	uint8_t gieb = (SREG & 0b10000000);
	SREG &= 0b01111111;
	if (criticalSectionCount == 255) { os_error("Overflow Fehler!");}
	else {criticalSectionCount++;}
	TIMSK2 &= ~(1 << (OCIE2A));
	SREG |= gieb;
	
}

/*!
 *  Leaves a critical code section by enabling the scheduler if needed.
 *  This function utilizes the nesting depth of critical sections
 *  stored by os_enterCriticalSection to check if the scheduler
 *  has to be reactivated.
 */
void os_leaveCriticalSection(void){
	uint8_t gieb = (SREG & 0b10000000);
	SREG &= 0b01111111;
	if (criticalSectionCount == 0) { os_error("Underflow Fehler!");}
	else {criticalSectionCount--;}
	if (criticalSectionCount == 0){TIMSK2 |= (1 << (OCIE2A));} 
	SREG |= gieb;
}

/*!
 *  Calculates the checksum of the stack for a certain process.
 *
 *  \param pid The ID of the process for which the stack's checksum has to be calculated.
 *  \return The checksum of the pid'th stack.
 */
StackChecksum os_getStackChecksum(ProcessID pid) {
	StackChecksum stackCheckSum = 0;
	uint8_t *localPtr = os_processes[pid].sp.as_ptr;
	localPtr++;
	
	for (; ((uint16_t) localPtr) <= PROCESS_STACK_BOTTOM(pid); localPtr++) {
		 //check Sum mit XOR
		stackCheckSum = stackCheckSum ^ *localPtr;

	}
    return stackCheckSum;
}
