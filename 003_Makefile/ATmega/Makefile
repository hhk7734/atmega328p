TARGET = main
MCU_TARGET = 
MCU_F_CPU  = 

PROGRAMMER = avrispmkII
PORT = /dev/cu.usbserial-DN042E96
BITCLOCK = 0.5 # ms 단위
BAUDRATE = 115200
UPLOAD = avrdude -p$(MCU_TARGET) -c$@ -v -Uflash:w:$(TARGET).elf:e

CC = avr-gcc
CFLAGS  = -g -Wall -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_F_CPU)
CFLAGS += -O1
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fshort-enums
CFLAGS += -fpack-struct
LDFLAGS  = -g -mmcu=$(MCU_TARGET) -Wl,-Map,$(TARGET).map
LDFLAGS += -Wl,--gc-sections

# $(call rwildcard,directory,filetype) directory 하위에서 형식에 맞는 모든 파일을 나열
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS  = $(call rwildcard,,*.c)
OBJS  = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(SRCS))))
OBJDIR = obj

# 경로 추가
CFLAGS += $(addprefix -I,$(dir $(SRCS)))

BUILD = $(TARGET)

.PHONY: clean upload

build: $(BUILD)
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
all: clean $(BUILD) upload
clean: ;rm -rf $(TARGET).* $(OBJDIR)

# 업로드
upload: $(PROGRAMMER)
	avr-size -C --mcu=$(MCU_TARGET) $(TARGET).elf
stk500: ;$(UPLOAD) -P$(PORT)
avrispmkII: ;$(UPLOAD) -B$(BITCLOCK)
arduino: ;$(UPLOAD) -P$(PORT) -b$(BAUDRATE)
atmelice_isp: ;$(UPLOAD)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf $^

vpath %.c $(sort $(dir $(SRCS)))

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir obj