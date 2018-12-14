TARGET = main
MCU_TARGET =
MCU_F_CPU  =

PROGRAMMER = arduino
# PORT = /dev/cu.usbserial-DN042E96
PORT = /dev/cu.wchusbserial146340
BITCLOCK = 0.5 # ms 단위
BAUDRATE = 115200
UPLOAD = avrdude -p$(MCU_TARGET) -c$@ -v -Uflash:w:$(TARGET).elf:e

CC = avr-gcc
CXX = avr-g++
CFLAGS  = -g -Wall -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_F_CPU)
CFLAGS += -O1
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fshort-enums
CFLAGS += -fpack-struct
CFLAGS += -funsigned-char
LDFLAGS  = -g -mmcu=$(MCU_TARGET) -Wl,-Map,$(TARGET).map
LDFLAGS += -Wl,--gc-sections

# $(call rwildcard,directory,filetype) directory 하위에서 형식에 맞는 모든 파일을 나열
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

C_SRCS  = $(call rwildcard,,*.c)
CXX_SRCS = $(call rwildcard,,*.cpp)
OBJS  = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(C_SRCS))))
OBJS += $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CXX_SRCS))))

OBJDIR = obj

# 경로 추가
CFLAGS += $(addprefix -I,$(dir $(C_SRCS)))
CFLAGS += $(addprefix -I,$(dir $(CXX_SRCS)))

BUILD = $(TARGET)

.PHONY: clean upload

build: $(BUILD)
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	avr-objdump -h -S "$(TARGET).elf" > "$(TARGET).lss"
	avr-size -C --mcu=$(MCU_TARGET) $(TARGET).elf

all: clean build upload
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

vpath %.c $(sort $(dir $(C_SRCS)))

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

vpath %.cpp $(sort $(dir $(CXX_SRCS)))

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir obj