// EFM32GG register definitions and similar

// GPIO port C memory region
#define GPIO_PC_BASE     0x40006048
#define GPIO_PC_LENGTH   36

// GPIO port C register offsets
#define GPIO_PC_CTRL     0x00
#define GPIO_PC_MODEL    0x04
#define GPIO_PC_MODEH    0x08
#define GPIO_PC_DOUT     0x0c
#define GPIO_PC_DOUTSET  0x10
#define GPIO_PC_DOUTCLR  0x14
#define GPIO_PC_DOUTTGL  0x18
#define GPIO_PC_DIN      0x1c
#define GPIO_PC_PINLOCKN 0x20



// GPIO interrupt control memory region
#define GPIO_IRQ_BASE    0x40006100
#define GPIO_IRQ_LENGTH  32

// GPIO interrupt control register offsets
#define GPIO_EXTIPSELL   0x00
#define GPIO_EXTIPSELH   0x04
#define GPIO_EXTIRISE    0x08
#define GPIO_EXTIFALL    0x0c
#define GPIO_IEN         0x10
#define GPIO_IF          0x14
#define GPIO_IFS         0x18
#define GPIO_IFC         0x1c
