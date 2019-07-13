TARGET = test

DEBUG = 1
OPTIMIZIATION = -O1

MCU_TARGET = atmega328p
MCU_F_CPU  = 16000000UL

PROGRAMMER = arduino

PORT = /dev/ttyUSB0
BAUDRATE = 115200

BITCLOCK = 0.5 # ms 단위

BUILD_DIR = build

UPLOAD = avrdude -p$(MCU_TARGET) -c$@ -v -Uflash:w:$(BUILD_DIR)/$(TARGET).elf:e
# 플래시 자동 지우기 비활성화
# UPLOAD += -D

CROSS_COMPILE = avr-

CC = gcc
CP = objcopy
OD = objdump
SZ = size

CFLAGS  = -Wall -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_F_CPU)
# 최적화
CFLAGS += $(OPTIMIZIATION)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fshort-enums
CFLAGS += -fpack-struct
# 의존성
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

ifeq ($(DEBUG), 1)
CFLAGS += -g
endif

LDFLAGS  = -mmcu=$(MCU_TARGET) -Wl,-Map,$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -Wl,--gc-sections

#$(call rwildcard,directory,filetype) directory 하위에서 형식에 맞는 모든 파일을 나열
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# 소스 파일
# C_SOURCES = $(call rwildcard,,*.c)
C_SOURCES = \
main.c \

# 헤더 파일 경로
# C_INCLUDES = $(dir $(call rwildcard,,*.h))
C_INCLUDES = \

C_INCLUDES += $(sort $(dir $(C_SOURCES)))

.PHONY: clean upload clang

# 기본 실행
all : $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

clean: ;rm -rf $(BUILD_DIR)

clang: $(call rwildcard,,*.c) $(call rwildcard,,*.h)
	clang-format -style=file -i -verbose $^

upload: $(PROGRAMMER)

# 폴더 생성
$(BUILD_DIR):
	mkdir $@

# 목적 파일, 어샘블리어 파일 생성
vpath %.c $(sort $(dir $(C_SOURCES)))
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(addprefix -I,$(C_INCLUDES)) -Wa,-a,-ad,-alms=$(@:.o=.lst) -c -o $@ $<

# 목적 파일 목록
OBJECTS  = $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.c,%.o,$(C_SOURCES))))
OBJECTS += $(call rwildcard,obj,*.o)

# 실행 파일 생성
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $@ $^
	$(CROSS_COMPILE)$(OD) -h -S "$@" > "$(@:%.elf=%.lss)"
	$(CROSS_COMPILE)$(SZ) -C --mcu=$(MCU_TARGET) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)$(CP) -j .text -j .data -O ihex $< $@

# 의존성
-include $(wildcard $(BUILD_DIR)/*.d)

# 업로드
stk500: ;$(UPLOAD) -P$(PORT)
avrispmkII: ;$(UPLOAD) -B$(BITCLOCK)
arduino: ;$(UPLOAD) -P$(PORT) -b$(BAUDRATE)
wiring: ;$(UPLOAD) -P$(PORT) -b$(BAUDRATE)
atmelice_isp: ;$(UPLOAD)