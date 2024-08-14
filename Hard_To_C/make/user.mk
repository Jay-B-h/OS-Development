.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h \

user/comhand.o: user/comhand.c include/sys_req.h include/string.h \
  include/mpx/comhand.h include/stdlib.h include/mpx/rtc.h \
  include/mpx/pcb.h include/mpx/io.h include/mpx/alarm.h \
  include/mpx/mem.h

kernel/r3_commands.o: user/r3_commands.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h

USER_OBJECTS=\
	user/core.o\
  user/r3_commands.o\
	user/comhand.o
		