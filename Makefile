CC := aarch64-linux-gnu-gcc
SRC := $(shell find src -name "*.c")
#CC += -g
INCLUDE := ./inc	\
			./3rd/usr/local/include	\
			./3rd/usr//include	\
			./3rd/usr/include/python3.10 \
			./3rd/usr/aarch64-linux-gnu \
			./3rd/usr/aarch64-linux-gnu/python3.10

OBJ := $(subst src/,obj/, $(SRC:.c=.o))

TARGET := obj/smartHome
CFLAGS := $(foreach item, $(INCLUDE), -I $(item))

LIBS_PATH := ./3rd/usr/local/lib	\
				./3rd/lib/aarch64-linux-gnu	\
				./3rd/usr/lib/aarch64-linux-gnu \
				./3rd/usr/lib/python3.10
				
LDFLAGS := $(foreach item, $(LIBS_PATH), -L $(item))	# -L./3rd/usr/local/lib
LIBS :=-lpython3.10 -lwiringPi -lz -lpthread -lexpat -lcrypt

obj/%.o:src/%.c
	mkdir -p obj
	$(CC) -o $@ -c $^ $(CFLAGS)
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)
    
complie : $(TARGET)
	scp ./obj/smartHome ./ini/gdevice.ini orangepi@192.168.10.36:/home/orangepi/smartHome_project


clean : 
	rm -rf $(TARGET) $(OBJ) obj

debug:
	@echo $(SRC)
	@echo $(OBJ)
	@echo $(TARGET)
	@echo $(CFLAGS)
	@echo $(LDFLAGS)

.PHONY: clean complie debug