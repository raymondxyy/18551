# 18551 hw2 Makefile

PROG = hw2.cpp
CC = g++
CFLAGS = -I/opt/local/include
LIBS = -L/opt/local/lib -lopencv_calib3d -lopencv_contrib -lopencv_core \
		-lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui \
		-lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree \
		-lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres \
		-lopencv_ts -lopencv_video -lopencv_videostab
EXEC = hw2

${EXEC}: ${PROG}
		${CC} ${PROG} ${CFLAGS} ${LIBS} -o ${EXEC}

clean:
		rm -f *~ *.o ${EXEC}

