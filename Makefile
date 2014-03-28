CC = gcc
CFLAGS = -Wall -g
LIBS = 
INCLUDES=
TARGET = robotarm
SRCS = main.c robotarm.h keypad.c keypad.h 
OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all: $(TARGET)
	@echo Remember make final.

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -D _NO_LCD_ -D _NO_KEYPAD_ $(INCLUDES) -c $< -o $@

final: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) lcd.c lcd.h -o $(TARGET) $(OBJS) $(LIBS)
	
clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
