#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <mpx/comhand.h>
#include <mpx/queue.h>
#include <mpx/sys_call.h>
#include <mpx/r3_commands.h>
#include <processes.h>
#include <mpx/pcb.h>
#include <mpx/mem.h>

//Reminder on how to run kmain
//Make sure in Hard_To_C directory
//run make 
//run ./mpx.sh

//External event flag
int eventFlag;

static void klogv(device dev, const char *msg)
{
	char prefix[] = "klogv: ";
	serial_out(dev, prefix, 8);
	serial_out(dev, msg, strlen(msg));
	serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
	// 0) Serial I/O -- <mpx/serial.h>
	// If we don't initialize the serial port, we have no way of
	// performing I/O. So we need to do that before anything else so we
	// can at least get some output on the screen.
	// Note that here, you should call the function *before* the output
	// via klogv(), or the message won't print. In all other cases, the
	// output should come first as it describes what is about to happen.
	
	serial_init(COM1);
	klogv(COM1, "Initialized serial I/O on COM1 device...");

	// 1) Global Descriptor Table (GDT) -- <mpx/gdt.h>
	// Keeps track of the various memory segments (Code, Data, Stack, etc.)
	// required by the x86 architecture. This needs to be initialized before
	// interrupts can be configured.
	klogv(COM1, "Initializing Global Descriptor Table...");
	gdt_init();

	// 2) Interrupt Descriptor Table (IDT) -- <mpx/interrupts.h>
	// Keeps track of where the various Interrupt Vectors are stored. It
	// needs to be initialized before Interrupt Service Routines (ISRs) can
	// be installed.
	klogv(COM1, "Initializing Interrupt Descriptor Table...");
	idt_init();

	// 3) Disable Interrupts -- <mpx/interrupts.h>
	// You'll be modifying how interrupts work, so disable them to avoid
	// crashing.
	klogv(COM1, "Disabling interrupts...");
	cli();

	// 4) Interrupt Request (IRQ) -- <mpx/interrupts.h>
	// The x86 architecture requires ISRs for at least the first 32
	// Interrupt Request (IRQ) lines.
	klogv(COM1, "Initializing Interrupt Request routines...");
	irq_init();

	// 5) Programmable Interrupt Controller (PIC) -- <mpx/interrupts.h>
	// The x86 architecture uses a Programmable Interrupt Controller (PIC)
	// to map hardware interrupts to software interrupts that the CPU can
	// then handle via the IDT and its list of ISRs.
	klogv(COM1, "Initializing Programmable Interrupt Controller...");
	pic_init();

	// 6) Reenable interrupts -- <mpx/interrupts.h>
	// Now that interrupt routines are set up, allow interrupts to happen
	// again.
	klogv(COM1, "Enabling Interrupts...");
	sti();

	// 7) Virtual Memory (VM) -- <mpx/vm.h>
	// Virtual Memory (VM) allows the CPU to map logical addresses used by
	// programs to physical address in RAM. This allows each process to
	// behave as though it has exclusive access to memory. It also allows,
	// in more advanced systems, the kernel to swap programs between RAM and
	// storage (such as a hard drive or SSD), and to set permissions such as
	// Read, Write, or Execute for pages of memory. VM is managed through
	// Page Tables, data structures that describe the logical-to-physical
	// mapping as well as manage permissions and other metadata.
	klogv(COM1, "Initializing Virtual Memory...");
	vm_init();

	// 8) MPX Modules -- *headers vary*
	// Module specific initialization -- not all modules require this.
	klogv(COM1, "Initializing MPX modules...");
	
	// R5 PART TWO UNCOMMENT
	//initialize_heap(50000);
	// sys_set_heap_functions(allocate_memory(50000), free_memory());
	
	// R5 PART TWO UNCOMMENT
	// R5 PART TWO UNCOMMENT
	initialize_heap(50000);
	sys_set_heap_functions(allocate_memory, free_memory);
	eventFlag = NO;
	serial_open(COM1,19200);
	
	// R4: create commhand and idle processes
	// Command Handler Process
    char name1[12] = "Commhand";
    pcbStruct* commhandProc = pcb_setup(name1,0,0);
    commhandProc->suspendStatus = ACTIVE;
    commhandProc->execution_state = READY;

    // Set the context of process 1 to the corresponding PCBs stack pointer
    context* comCont1 = (context*)(commhandProc->stackPtr);
    memset(comCont1, 0, sizeof(context));

    // Set context register values
    comCont1->cs = 0x08;
    comCont1->ds = 0x10;
    comCont1->es = 0x10;
    comCont1->fs = 0x10;       
    comCont1->gs = 0x10;
    comCont1->ss = 0x10;
    
    comCont1->ebp = (int)commhandProc->stack;                
    comCont1->eip = (int) comhand;

    comCont1->eflags = 0x0202;
    comCont1->eax = 0x00;
    comCont1->ebx = 0x00;
    comCont1->ecx = 0x00;
    comCont1->edx = 0x00;
    comCont1->esi = 0x00;
    comCont1->edi = 0x00;

    //Insert process into into queue
    pcb_insert(commhandProc);

    // END OF COMMHAND

	// Idle Process
    char name2[12] = "Idle Proc";
    pcbStruct* Idleproc = pcb_setup(name2,1,9);
    Idleproc->suspendStatus = ACTIVE;
    Idleproc->execution_state = READY;

    // Set the context of process 1 to the corresponding PCBs stack pointer
    context* idleCont = (context*)(Idleproc->stackPtr);
    memset(idleCont, 0, sizeof(context));

    // Set context register values
    idleCont->cs = 0x08;
    idleCont->ds = 0x10;
    idleCont->es = 0x10;
    idleCont->fs = 0x10;       
    idleCont->gs = 0x10;
    idleCont->ss = 0x10;
    
    idleCont->ebp = (int)Idleproc->stack;                
    idleCont->eip = (int) sys_idle_process;

    idleCont->eflags = 0x0202;
    idleCont->eax = 0x00;
    idleCont->ebx = 0x00;
    idleCont->ecx = 0x00;
    idleCont->edx = 0x00;
    idleCont->esi = 0x00;
    idleCont->edi = 0x00;

    //Insert process into into queue
    pcb_insert(Idleproc);

    // END OF IDLE

	// 9) YOUR command handler -- *create and #include an appropriate .h file*
	// Pass execution to your command handler so the user can interact with
	// the system.
	klogv(COM1, "Transferring control to commhand...");
	// IMPLEMENT COMMAND HERE
	//titleCard();

	__asm__ volatile ("int $0x60" :: "a"(IDLE));

	// 10) System Shutdown -- *headers to be determined by your design*
	// After your command handler returns, take care of any clean up that
	// is necessary.
	klogv(COM1, "Starting system shutdown procedure...");

	// 11) Halt CPU -- *no headers necessary, no changes necessary*
	// Execution of kmain() will complete and return to where it was called
	// in boot.s, which will then attempt to power off Qemu or halt the CPU
	klogv(COM1, "Halting CPU...");
}
