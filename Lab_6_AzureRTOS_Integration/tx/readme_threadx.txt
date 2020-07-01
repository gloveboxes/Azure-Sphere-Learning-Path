                        Express Logic's ThreadX for Cortex-M4 

                              Using the GNU Tools

1.  Installation

ThreadX for the Cortex-M4 is delivered on a single CD-ROM compatible disk. 
The entire distribution can be found in the sub-directory:

\threadx

To install ThreadX to your hard-disk, either run the supplied installer 
program Setup.exe or copy the distribution from the CD manually. 

To copy the ThreadX distribution manually, make a ThreadX directory on your 
hard-disk (we recommend C:\threadx\cortex-m4\gnu) and copy all the contents 
of the ThreadX sub-directory on the distribution disk. The following 
is an example MS-DOS copy command from the distribution directory
(assuming source is d: and c: is your hard-drive):


d:\threadx> xcopy /S *.* c:\threadx\cortex-m4\gnu


2.  Building the ThreadX run-time Library

First make sure you are in the ThreadX directory you have created on your 
hard-drive. Also, make sure that you have setup your path and other 
environment variables necessary for the ARM gnu (GNU) compiler. 
At this point you may run the build_threadx.bat batch file. This will 
build the ThreadX run-time environment in the ThreadX directory.

C:\threadx\cortex-m4\gnu> build_threadx

You should observe assembly and compilation of a series of ThreadX source 
files. At the end of the batch file, they are all combined into the 
run-time library file: tx.a. This file must be linked with your 
application in order to use ThreadX.


3.  Demonstration System for Cortex-M4

The ThreadX demonstration is designed to execute on Cortex-M4 evaluation boards
or on a dedicated simulator.

Building the demonstration is easy, simply execute the build_threadx_demo.bat 
batch file while inside your ThreadX directory on your hard-disk.

C:\threadx\cortex-m4\gnu> build_threadx_demo 

You should observe the compilation of demo_threadx.c (which is the demonstration 
application) and linking with tx.a. The resulting file demo_threadx.out is a binary 
file that can be downloaded and executed on the a simulator, or downloaded to a board.


4.  System Initialization

The entry point in ThreadX for the Cortex-M4 using gnu tools uses the standard GNU 
Cortex-M4 reset sequence. From the reset vector the C runtime will be initialized.

The ThreadX tx_initialize_low_level.S file is responsible for setting up 
various system data structures, the vector area, and a periodic timer interrupt 
source. 

In addition, _tx_initialize_low_level determines the first available 
address for use by the application, which is supplied as the sole input 
parameter to your application definition function, tx_application_define.


5.  Register Usage and Stack Frames

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have the same stack frame in the Cortex-M4 version of
ThreadX. The top of the suspended thread's stack is pointed to by 
tx_thread_stack_ptr in the associated thread control block TX_THREAD.

Non-FPU Stack Frame:

  Stack Offset     Stack Contents 

     0x00               LR          Interrupted LR (LR at time of PENDSV)
     0x04               r4          
     0x08               r5          
     0x0C               r6          
     0x10               r7          
     0x14               r8          
     0x18               r9          
     0x1C               r10 (sl)    
     0x20               r11         
     0x24               r0          (Hardware stack starts here!!)
     0x28               r1          
     0x2C               r2          
     0x30               r3          
     0x34               r12         
     0x38               lr          
     0x3C               pc          
     0x40               xPSR        

FPU Stack Frame (only interrupted thread with FPU enabled):

  Stack Offset     Stack Contents 

     0x00               LR          Interrupted LR (LR at time of PENDSV)
     0x04               s0
     0x08               s1
     0x0C               s2
     0x10               s3
     0x14               s4
     0x18               s5
     0x1C               s6
     0x20               s7
     0x24               s8
     0x28               s9
     0x2C               s10
     0x30               s11
     0x34               s12
     0x38               s13
     0x3C               s14
     0x40               s15
     0x44               s16
     0x48               s17
     0x4C               s18
     0x50               s19
     0x54               s20
     0x58               s21
     0x5C               s22
     0x60               s23
     0x64               s24
     0x68               s25
     0x6C               s26
     0x70               s27
     0x74               s28
     0x78               s29
     0x7C               s30
     0x80               s31
     0x84               fpscr
     0x88               r4          
     0x8C               r5          
     0x90               r6          
     0x94               r7          
     0x98               r8          
     0x9C               r9          
     0xA0               r10 (sl)    
     0xA4               r11         
     0xA8               r0          (Hardware stack starts here!!)
     0xAC               r1          
     0xB0               r2          
     0xB4               r3          
     0xB8               r12         
     0xBC               lr          
     0xC0               pc          
     0xC4               xPSR        


6.  Improving Performance

The distribution version of ThreadX is built without any compiler optimizations. 
This makes it easy to debug because you can trace or set breakpoints inside of 
ThreadX itself. Of course, this costs some performance. To make it run faster, 
you can change the build_threadx.bat file to remove the -g option and enable 
all compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


7.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-M4
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


7.1  Vector Area

The Cortex-M4 vectors start at the label __tx_vectors or similar. The application may modify
the vector area according to its needs. There is code in tx_initialize_low_level() that will 
configure the vector base register. 


7.2 Managed Interrupts

From Cortex-M4/GNU version 5.3 going forward, ISRs can be written completely in C (or assembly 
language) without any calls to _tx_thread_context_save or _tx_thread_context_restore. These ISRs 
are allowed access to the ThreadX API that is available to ISRs.

ISRs written in C will take the form (where "your_C_isr" is an entry in the vector table):

void    your_C_isr(void)
{

    /* ISR processing goes here, including any needed function calls.  */
}

ISRs written in assembly language will take the form:


        .global  your_assembly_isr
        .thumb_func
your_assembly_isr:
; VOID your_assembly_isr(VOID)
; {
        PUSH    {lr}
;       
;    /* Do interrupt handler work here */
;    /* BL <your interrupt routine in C> */

        POP     {r0}
        MOV     lr, r0
        BX      lr
; }

Backward compatibility to the previous style assembly ISRs is maintained, which was of 
the form:

        .global  your_assembly_isr
        .thumb_func
your_assembly_isr:
; VOID your_assembly_isr(VOID)
; {
        PUSH    {lr}
        BL  _tx_thread_context_save
        
;    /* Do interrupt handler work here */
;    /* BL <your interrupt routine in C> */

        B       _tx_thread_context_restore
; }

Note: the Cortex-M4 requires exception handlers to be thumb labels, this implies bit 0 set.
To accomplish this, the declaration of the label has to be preceded by the assembler directive
.thumb_func to instruct the linker to create thumb labels. The label __tx_IntHandler needs to 
be inserted in the correct location in the interrupt vector table. This table is typically 
located in either your runtime startup file or in the tx_initialize_low_level.S file.


8. FPU Support

ThreadX for Cortex-M4 supports automatic ("lazy") VFP support, which means that applications threads 
can simply use the VFP and ThreadX automatically maintains the VFP registers as part of the thread 
context. If saving the context of the FPU registers is needed, the ThreadX library should be re-built 
with TX_ENABLE_FPU_SUPPORT defined.

In previous versions, the application was also required to explicitly enable VFP support on a 
thread-by-thread basis. This is no longer required. However, these APIs are maintained for backward 
compatibility:

void    tx_thread_fpu_enable(void);
void    tx_thread_fpu_disable(void);


9.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

09/01/2018  ThreadX update of Cortex-M4/GNU port. The following files were 
            changed/added for port specific version 5.4:

            tx_port.h                       Added macros to handle VFP flush when thread completes 
                                            or terminates, optimized lowest set bit macro, and 
                                            updated version string.
            tx_thread_schedule.S            Avoid unnecessary VFP stacking, and modified WFI 
                                            processing to use DSB and to keep interrupts locked 
                                            out until after WFI completes.
            *.S                             Modified comments.

04/01/2018  ThreadX update of Cortex-M4/GNU port. The following files were 
            changed/added for port specific version 5.3:

            tx_port.h                       Added logic to remove the need for context 
                                            save/restore calls in ISRs, and changed 
                                            version ID.
            tx_initialize_low_level.S       Removed context save/restore calls from ISRs.
            tx_thread_context_restore.S     Changed the code to simply return since this 
                                            function is no longer needed.
            tx_thread_context_save.S        Changed the code to simply return since this 
                                            function is no longer needed.
            tx_thread_schedule.S            Added automatic VFP save/restore, added 
                                            optimizations, and changed logic to use 
                                            PendSV instead of SVC.
            tx_thread_stack_build.S         Added LR to initial stack.
            tx_thread_system_return.S       Changed logic to use PendSV instead of SVC 
                                            to return to the scheduler.
            tx_timer_interrupt.S            Added logic to set PendSV if preemption is 
                                            necessary.
            *.s                             Modified comments.

01/01/2014  ThreadX update of Cortex-M4/GNU port. The following files were 
            changed/added for port specific version 5.2:

            tx_port.h                       Added FPU enable/disable struct 
                                              member, and changed version ID.
            tx_thread_schedule.S            Removed unnecessary code, moved FPU 
                                              control field, added interrupt protection 
                                              over optional thread exit call and placed 
                                              conditional around WFI instruction.
            *.S                             Modified comments.

04/15/2012  ThreadX update of Cortex-M4/GNU port. The following files were 
            changed/added for port specific version 5.1:

            tx_port.h                       Changed version ID and added FPU enable/disable
                                            function prototypes.
            tx_initialize_low_level.S       Added FPU support.
            tx_thread_schedule.S            Added FPU support. 
            *.S                             Modified comments.

03/11/2012  Initial ThreadX 5.0 version for Cortex-M4 using GNU tools.


Copyright(c) 1996-2018 Express Logic, Inc.


Express Logic, Inc.
11423 West Bernardo Court
San Diego, CA  92127

www.expresslogic.com

