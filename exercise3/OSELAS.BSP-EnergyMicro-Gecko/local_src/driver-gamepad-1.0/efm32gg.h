// EFM32GG register definitions and similar

// GPIO memory region, port C
#define GPIO_PC_BASE 0x40006048
#define GPIO_PC_LENGTH 0x24

// GPIO registers, port C
#define GPIO_PC_CTRL     ((void*)(GPIO_PC_BASE + 0x00))
#define GPIO_PC_MODEL    ((void*)(GPIO_PC_BASE + 0x04))
#define GPIO_PC_MODEH    ((void*)(GPIO_PC_BASE + 0x08))
#define GPIO_PC_DOUT     ((void*)(GPIO_PC_BASE + 0x0c))
#define GPIO_PC_DOUTSET  ((void*)(GPIO_PC_BASE + 0x10))
#define GPIO_PC_DOUTCLR  ((void*)(GPIO_PC_BASE + 0x14))
#define GPIO_PC_DOUTTGL  ((void*)(GPIO_PC_BASE + 0x18))
#define GPIO_PC_DIN      ((void*)(GPIO_PC_BASE + 0x1c))
#define GPIO_PC_PINLOCKN ((void*)(GPIO_PC_BASE + 0x20))
