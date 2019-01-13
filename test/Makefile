# C언어 AVR Makefile
TARGET = test
MCU_TARGET = atmega328p
MCU_F_CPU  = 16000000UL

PROGRAMMER = arduino

# PORT = /dev/cu.usbserial-DN042E96
PORT = /dev/ttyUSB0

# BAUDRATE = 57600
BAUDRATE = 115200

BITCLOCK = 0.5 # ms 단위

BUILD_DIR = build

UPLOAD = avrdude -p$(MCU_TARGET) -c$@ -v -Uflash:w:$(BUILD_DIR)/$(TARGET).elf:e
# 플래시 자동 지우기 비활성화
UPLOAD += -D

CC = avr-gcc
CP = avr-objcopy
OD = avr-objdump
SZ = avr-size

CFLAGS  = -g -Wall -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_F_CPU)
# 최적화
CFLAGS += -O1
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fshort-enums
CFLAGS += -fpack-struct
# 의존성
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LDFLAGS  = -g -mmcu=$(MCU_TARGET) -Wl,-Map,$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -Wl,--gc-sections

#$(call rwildcard,directory,filetype) directory 하위에서 형식에 맞는 모든 파일을 나열
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# 소스 파일
# C_SRCS = $(call rwildcard,,*.c)
C_SRCS = \
src/main.c \
../LOT_uart/LOT_uart0/LOT_uart0.c \

# 헤더파일 경로 추가
# C_INCS = $(dir $(call rwildcard,,*.h))
C_INCS = \
../LOT_uart/LOT_uart0 \

.PHONY: clean upload

# 기본 실행
all : $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

clean: ;rm -rf $(BUILD_DIR)

# 폴더 생성
$(BUILD_DIR):
	mkdir $@

# 목적 파일, 어샘블리어 파일 생성
vpath %.c $(sort $(dir $(C_SRCS)))
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(addprefix -I,$(C_INCS)) -Wa,-a,-ad,-alms=$(@:.o=.lst) -c -o $@ $<

# 목적 파일 목록
OBJS  = $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.c,%.o,$(C_SRCS))))

# 실행 파일 생성
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OD) -h -S "$@" > "$(@:%.elf=%.lss)"
	$(SZ) -C --mcu=$(MCU_TARGET) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -j .text -j .data -O ihex $< $@

# 의존성
-include $(wildcard $(BUILD_DIR)/*.d)

# 업로드
upload: $(PROGRAMMER)
stk500: ;$(UPLOAD) -P$(PORT)
avrispmkII: ;$(UPLOAD) -B$(BITCLOCK)
arduino: ;$(UPLOAD) -P$(PORT) -b$(BAUDRATE)
wiring: ;$(UPLOAD) -P$(PORT) -b$(BAUDRATE)
atmelice_isp: ;$(UPLOAD)
