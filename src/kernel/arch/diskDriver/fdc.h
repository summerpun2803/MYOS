#include <stdint.h>
enum FloppyRegisters
{
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only
   DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum FLPYDSK_DOR_MASK {
 
	FLPYDSK_DOR_MASK_DRIVE0				=	0,	//00000000	= here for completeness sake
	FLPYDSK_DOR_MASK_DRIVE1				=	1,	//00000001
	FLPYDSK_DOR_MASK_DRIVE2				=	2,	//00000010
	FLPYDSK_DOR_MASK_DRIVE3				=	3,	//00000011
	FLPYDSK_DOR_MASK_RESET				=	4,	//00000100
	FLPYDSK_DOR_MASK_DMA				=	8,	//00001000
	FLPYDSK_DOR_MASK_DRIVE0_MOTOR		=	16,	//00010000
	FLPYDSK_DOR_MASK_DRIVE1_MOTOR		=	32,	//00100000
	FLPYDSK_DOR_MASK_DRIVE2_MOTOR		=	64,	//01000000
	FLPYDSK_DOR_MASK_DRIVE3_MOTOR		=	128	//10000000
};

enum FLPYDSK_CMD {
	
	FDC_CMD_READ_TRACK		=	2,
	FDC_CMD_SPECIFY			=	3,
	FDC_CMD_CHECK_STAT		=	4,
	FDC_CMD_WRITE_SECT		=	5,
	FDC_CMD_READ_SECT		=	6,
	FDC_CMD_CALIBRATE		=	7,
	FDC_CMD_CHECK_INT		=	8,
	FDC_CMD_WRITE_DEL_S		=	9,
	FDC_CMD_READ_ID_S		=	0xa,
	FDC_CMD_READ_DEL_S		=	0xc,
	FDC_CMD_FORMAT_TRACK	=	0xd,
	FDC_CMD_SEEK			=	0xf
};

void DMA_Init();
void FDA_Init();
void flpydsk_check_int (uint32_t* st0, uint32_t* cyl);
int fdc_write(uint8_t cmd);
uint8_t flpydsk_read_data ();