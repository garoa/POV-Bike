PROGNAME=POV
GCCFLAGS=-g -Os -Wall -mcall-prologues -mmcu=attiny44 -DF_CPU=8000000
LINKFLAGS=-Wl,-Map,${PROGNAME}.map
AVRDUDEFLAGS=-c usbtiny -p attiny44
LFUSE = 0xe2
HFUSE = 0xdf
EFUSE = 0xff
LOCK  = 0xff

all:	${PROGNAME}.hex

program: ${PROGNAME}-upload

fuse:
	avrdude ${AVRDUDEFLAGS} -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m \
    -U efuse:w:$(EFUSE):m -U lock:w:$(LOCK):m

${PROGNAME}.hex: ${PROGNAME}.obj
	avr-objcopy -R .eeprom -O ihex ${PROGNAME}.obj ${PROGNAME}.hex

${PROGNAME}.obj: ${PROGNAME}.c
	avr-gcc ${GCCFLAGS} ${LINKFLAGS} -o ${PROGNAME}.obj ${PROGNAME}.c
   
${PROGNAME}-upload:	${PROGNAME}.hex
	avrdude ${AVRDUDEFLAGS} -U flash:w:${PROGNAME}.hex:i
