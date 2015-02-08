 	

      /////////////////////////////////////////////////////////////////////////////
      //
      // Various useful I/O addresses and definitions for EFM32GG
      //
      // TODO: Add more registers
      // TODO: Add even more registers
      //	
      /////////////////////////////////////////////////////////////////////////////
     
     
        //////////////////////////////////////////////////////////////////////
        // GPIO
	
        GPIO_PA_BASE = 0x40006000
        GPIO_PB_BASE = 0x40006024
        GPIO_PC_BASE = 0x40006048
        GPIO_PD_BASE = 0x4000606c
        GPIO_PE_BASE = 0x40006090
        GPIO_PF_BASE = 0x400060b4
     
        // register offsets from base address
        GPIO_CTRL     = 0x00
        GPIO_MODEL    = 0x04
        GPIO_MODEH    = 0x08
        GPIO_DOUT     = 0x0c
        GPIO_DOUTSET  = 0x10
        GPIO_DOUTCLR  = 0x14
        GPIO_DOUTTGL  = 0x18
        GPIO_DIN      = 0x1c
        GPIO_PINLOCKN = 0x20
     
        GPIO_BASE = 0x40006100
     
        // register offsets from base address
        GPIO_EXTIPSELL = 0x00
        GPIO_EXTIPSELH = 0x04
        GPIO_EXTIRISE  = 0x08
        GPIO_EXTIFALL  = 0x0c
        GPIO_IEN       = 0x10
        GPIO_IF        = 0x14
        GPIO_IFC       = 0x1c
	
        //////////////////////////////////////////////////////////////////////
        // CMU
     
        CMU_BASE = 0x400c8000
     
        CMU_HFCORECLKDIV	= 0x004        //HF Core clk division 
        CMU_HFROCTRL		= 0x00C            
        CMU_HFPERCLKDIV 	= 0x008
	CMU_HFCORECLKEN0	= 0x40
        CMU_HFPERCLKEN0 	= 0x044
        CMU_OSCENCMD 		= 0x020
        CMU_CMD 		= 0x024
        CMU_LFCLKSEL 		= 0x028
	CMU_LFACLKEN0 		= 0x58
           
     
        CMU_HFPERCLKEN0_GPIO = 13
     
        //////////////////////////////////////////////////////////////////////
        // NVIC
     
        ISER0 = 0xe000e100
        ISER1 = 0xe000e104
        ICER0 = 0xe000e180
        ICER1 = 0xe000e184
        ISPR0 = 0xe000e200
        ISPR1 = 0xe000e204
        ICPR0 = 0xe000e280
        ICPR1 = 0xe000e284
        IABR0 = 0xe000e300
        IABR1 = 0xe000e304
        IPR_BASE  = 0xe000e400
        IPR0 = 0x00
        IPR1 = 0x04
        IPR2 = 0x08
        IPR3 = 0x0c
	
        //////////////////////////////////////////////////////////////////////
        // EMU
	
        EMU_BASE = 0x400c6000
        EMU_MEM_CTRL = 0x004
        EMU_CTRL = 0x000
     
        //////////////////////////////////////////////////////////////////////
        // System Control Block
     
        SCR = 0xe000ed10
     
        //////////////////////////////////////////////////////////////////////
        // DMA_CONTROL_REGISTER
        // 

        DMA_BASE                = 0x400c2000		// DMA base register
        DMA_CTRL_BASE           = 0x008                 // Here we set the address to the data structure
	DMA_0_CCR               = 0x1100                // DMA Channel Control Register

	//////////////////////////////////////////////////////////////////////
        // DMA_SETTINGS	
        // 
	
        DMA_STRUCT_CTRL	        = 0xC0FEBFFA            // Channel settings (description in my scratchbook!)
	DMA_SRC_LEUART0		= 0x10			// Channel source
	DMA_SIG_XDATAV		= 0x0			// Channel signal
	
	//////////////////////////////////////////////////////////////////////
        // DMA_DATA_STRUC
	// LIVES IN SRAM
        // ONLY HAS CHANNEL 0

	DMA_STRUCTURE           = 0x20001000		// Data structure st
        DMA_SRC_PT              = 0x0                   // source pointer
        DMA_DEST_PT             = 0x4                   // End pointer
        DMA_CTRL_WORD 	        = 0x8                   // CTRL word
        DMA_USR_PT              = 0xC     		// Not used
	

        //////////////////////////////////////////////////////////////////////
        // PRS (pg 169)

	PRS_BASE 		= 0x400CC000
	PRS_CH0_CTRL 		= 0x10			// PRS channel ctrl, see page 171

	// CH0
	PRS_CH0_CTRL_WORD	= 0x11300007		// CH0 config. Is in hex, just looks like dec...


        //////////////////////////////////////////////////////////////////////
        // LEUART0 (pg 497) 

	LEUART0_BASE		= 0x40084000
	LEUART0_CTRL		= 0x0
	

	//////////////////////////////////////////////////////////////////////
        // LETIMER0

	LETIMER_BASE	= 0x40082000
	LETIMER_CTRL	= 0x0
	LETIMER_CMD	= 0x4
	LETIMER_IEN	= 0x2C
	LETIMER_IF	= 0x20
	LETIMER_IFS	= 0x24
	LETIMER_IFC	= 0x28
	LETIMER_COMP0	= 0x10

	//////////////////////////////////////////////////////////////////////
        // RTC

	RTC_BASE 	= 0x40080000
	RTC_CTRL	= 0x0