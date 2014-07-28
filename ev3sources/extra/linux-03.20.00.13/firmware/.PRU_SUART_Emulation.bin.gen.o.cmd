cmd_firmware/PRU_SUART_Emulation.bin.gen.o := arm-none-linux-gnueabi-gcc -Wp,-MD,firmware/.PRU_SUART_Emulation.bin.gen.o.d  -nostdinc -isystem /opt/CodeSourcery/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/maximilian/projects/git-clean/ev3sources/extra/linux-03.20.00.13/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-davinci/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float       -c -o firmware/PRU_SUART_Emulation.bin.gen.o firmware/PRU_SUART_Emulation.bin.gen.S

deps_firmware/PRU_SUART_Emulation.bin.gen.o := \
  firmware/PRU_SUART_Emulation.bin.gen.S \
  /home/maximilian/projects/git-clean/ev3sources/extra/linux-03.20.00.13/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

firmware/PRU_SUART_Emulation.bin.gen.o: $(deps_firmware/PRU_SUART_Emulation.bin.gen.o)

$(deps_firmware/PRU_SUART_Emulation.bin.gen.o):
