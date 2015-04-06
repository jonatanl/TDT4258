// EFM32GG register definitions and similar

// GPIO memory region
#define GPIO_PA_BASE 0x40006000

// GPIO register offsets
#define GPIO_Px_CTRL     0x00
#define GPIO_Px_MODEL    0x04
#define GPIO_Px_MODEH    0x08
#define GPIO_Px_DOUT     0x0c
#define GPIO_Px_DOUTSET  0x10
#define GPIO_Px_DOUTCLR  0x14
#define GPIO_Px_DOUTTGL  0x18
#define GPIO_Px_DIN      0x1c
#define GPIO_Px_PINLOCKN 0x20

// GPIO interrupt registers
#define GPIO_EXTIPSELL 0x100
#define GPIO_EXTIPSELH 0x104
#define GPIO_EXTIRISE  0x108
#define GPIO_EXTIFALL  0x10c
#define GPIO_IEN       0x110
#define GPIO_IF        0x114
#define GPIO_IFS       0x118
#define GPIO_IFC       0x11c

// GPIO memory region, port C
#define GPIO_PC_BASE 0x40006048
#define GPIO_PC_LENGTH 36

// GPIO memory region, interrupt control
#define GPIO_IRQ_BASE 0x40006100
#define GPIO_IRQ_LENGTH 32



// Platform IRQ numbers
#define GPIO_EVEN_IRQ 17
#define GPIO_ODD_IRQ  17
