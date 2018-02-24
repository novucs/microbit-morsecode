# Edit this line to where your micro:bit device is mounted:
MICROBIT_PATH = /run/media/billy/MICROBIT

# Uncomment the line below if you are using the supplied Ubuntu VM.
#MICROBIT_PATH = /media/student/MICROBIT

SOURCE_FILES += \
    source/main.cpp \
    source/MorseCode.cpp \
    source/MorseCode.h \
    source/SpeckCipher.cpp \
    source/SpeckCipher.h \
    source/WireReader.cpp \
    source/WireReader.h \
    source/WireWriter.cpp \
    source/WireWriter.h

OUTPUT_FILE = build/bbc-microbit-classic-gcc/source/microbit-morsecode-combined.hex

run: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}1

2: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}

1: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}1

${OUTPUT_FILE}: ${SOURCE_FILES}
	yt build
