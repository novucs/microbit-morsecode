# Edit this line to where your micro:bit device is mounted:
MICROBIT_PATH = /run/media/billy/MICROBIT

# Uncomment the line below if you are using the supplied Ubuntu VM.
#MICROBIT_PATH = /media/student/MICROBIT

SOURCE_FILES += \
    source/main.cpp

OUTPUT_FILE = build/bbc-microbit-classic-gcc/source/microbit-platformer-combined.hex

run: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}1

2: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}

1: ${OUTPUT_FILE}
	cp ${OUTPUT_FILE} ${MICROBIT_PATH}1

${OUTPUT_FILE}: ${SOURCE_FILES}
	yt build
