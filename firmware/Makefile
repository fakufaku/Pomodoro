
CC=avr-gcc
PROGRAMMER=usbtiny
CPU=attiny85

SOURCE=pomodoro.c
OBJECT=pomodoro.o
ELF=pomodoro.elf
HEX=pomodoro.hex

LFUSE=
HFUSE=
EFUSE=

object: ${SOURCE}
	${CC} -mmcu=${CPU} -c ${SOURCE}

elf: object
	${CC} -mmcu=${CPU} -o ${ELF} ${OBJECT}

hex: elf
	avr-objcopy -j .text -j .data -O ihex ${ELF} ${HEX}

avrdude: hex
	avrdude -c ${PROGRAMMER} -p ${CPU} -U flash:w:${HEX}:i

readfuse:
	avrdude -c ${PROGRAMMER} -p ${CPU} -U lfuse:r:-:h 2> /dev/null
	avrdude -c ${PROGRAMMER} -p ${CPU} -U hfuse:r:-:h 2> /dev/null
	avrdude -c ${PROGRAMMER} -p ${CPU} -U efuse:r:-:h 2> /dev/null
