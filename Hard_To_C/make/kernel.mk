.POSIX:

kernel/serial.o: kernel/serial.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h

kernel/kmain.o: kernel/kmain.c include/mpx/gdt.h include/mpx/interrupts.h \
  include/mpx/serial.h include/mpx/device.h include/mpx/vm.h \
  include/sys_req.h include/string.h include/memory.h \
  include/mpx/comhand.h include/processes.h include/mpx/pcb.h \
  include/mpx/mem.h

kernel/core-c.o: kernel/core-c.c include/mpx/gdt.h include/mpx/panic.h \
  include/mpx/interrupts.h include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h include/string.h \

kernel/rtc.o: kernel/rtc.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h \
  include/mpx/vm.h include/mpx/rtc.h

kernel/pcb.o: kernel/pcb.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h \
  include/mpx/vm.h include/mpx/queue.h \
  include/sys_req.h

kernel/alarm.o: kernel/alarm.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h \
  include/mpx/vm.h include/mpx/queue.h \
  include/sys_req.h include/mpx/r3_commands.h

kernel/queue.o: kernel/queue.c include/stdlib.h include/mpx/pcb.h include/mpx/alarm.h

kernel/sys_call.o: kernel/sys_call.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h include/mpx/pcb.h

kernel/mem.o: kernel/mem.c include/mpx/mem.h include/mpx/vm.h \
  include/mpx/queue.h

KERNEL_OBJECTS=\
	kernel/core-asm.o\
	kernel/sys_call_isr.o\
	kernel/serial.o\
	kernel/kmain.o\
  kernel/rtc.o\
  kernel/pcb.o\
  kernel/queue.o\
  kernel/alarm.o\
  kernel/sys_call.o\
	kernel/core-c.o\
  kernel/serial_isr.o\
  kernel/mem.o
  