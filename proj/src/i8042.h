#ifndef __I8042_H
#define __I8042_H


#define BIT(n) 		 (0x01<<(n))
#define KBC_IRQ		 1
#define DELAY_US     20000
#define OUT_BUF		 0x60
#define OUT_BUF_FULL 0x01
#define IN_BUF		 0x60
#define ESC			 0x81
#define ESPACO		 0x39
#define ENTER		 0x1C
#define SETA_CIMA	 0x48
#define SETA_BAIXO	 0x50
#define TWO_BYTE	 0xE0
#define STAT_REG     0x64
#define KBC_CMD_REG  0x64
#define PAR_ERR		 BIT(7)
#define TO_ERR  	 BIT(5)
#define OBF 		 BIT(0)
#define AUX			 BIT(5)
#define MOUSE_IRQ	 12
#define IBF			 BIT(1)
#define SET_STREAM_MODE 0xEA
#define D_DATA_REPORT 0xF5
#define E_DATA_REPORT 0xF4


#endif
