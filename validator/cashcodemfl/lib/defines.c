#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CFG_FILE "/home/eff/eclws/unixterm/bin/validator/cashcodemfl/settings.cfg"
#define	WAITTIMEOUT	10000
#define	READ_TRIES	20

struct config {
	char	device_addr[50];
	int 	mode;
	int		fd;
	int		state;
};

struct config CFG;

#define OUT_ERR_UNKMODE				"80"
#define OUT_ERR_PARAMERROR			"81"
#define OUT_ERR_DEVICEOPENERROR		"82"
#define OUT_ERR_CONFIGERROR			"83"
#define OUT_OKR						"11"
#define OUT_OKE						"12"
#define OUT_NULLS					"00"
#define OUT_FAIL_EXCHANGE			"D0"
#define PARAM_RESET					"reset"
#define PARAM_STATE					"state"
#define PARAM_SCENDIS				"sc_en_dis"
#define	MODE_RESET					0x01
#define	MODE_STATE					0x02
#define	MODE_SCENDIS				0x03

typedef struct {
	char	sync[1];
	int		length;
	char	cmd[1];
	char	data[256];
	int 	datalen;
	char	crc[2];
	char	output[261];
} cmdstuct;

cmdstuct	command;

typedef struct {
	char 	sync[1];
	int 	length;
	char 	cmd[1];
	char	data[255];
	int 	datasize;
	char 	inbuf[261];
	int		bufsize;
} answerstruct;

answerstruct answer;

typedef struct {
	short int n1;
	short int n2;
	short int n3;
	short int n4;
	short int n5;
	short int n6;
	short int n7;
	short int n8;
	char 	buf[2];
} nomstruct;

nomstruct nominals;

#define	SIG_SYNC			0xFC
#define	CMD_STATE			0x11
#define	CMD_RESET			0x40
#define	CMD_STACK1			0x41
#define	CMD_STACK2			0x42
#define	CMD_RETURN			0x43
#define	CMD_HOLD			0x44
#define	CMD_WAIT			0x45
#define	CMD_ACK				0x50
#define	CMD_STREQENDIS		0x80
#define	CMD_STREQSECUR		0x81
#define	CMD_STREQINHIBIT	0x83
#define	CMD_STREQOPTFUNC	0x85
#define	CMD_STREQVERSION	0x88
#define	CMD_STREQBOOTVERS	0x89
#define	CMD_SETENDIS		0xC0
#define	CMD_SETSECUR		0xC1
#define	CMD_SETINHIBIT		0xC3
#define	CMD_SETDIRECT		0xC4
#define	CMD_SETOPTFUNC		0xC5


const unsigned commlen[0x100] = {
// 0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00 - 0x0f
   0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x10 - 0x1f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x20 - 0x2f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x30 - 0x3f
   5,  5,  5,  5,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x40 - 0x4f
   5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x50 - 0x5f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x60 - 0x6f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x70 - 0x7f
   7,  7,  0,  6,  6,  7,  0,  0,  5,  5,  0,  0,  0,  0,  0,  0, // 0x80 - 0x8f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x90 - 0x9f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xa0 - 0xaf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xb0 - 0xbf
   7,  7,  0,  6,  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xc0 - 0xcf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xd0 - 0xdf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xe0 - 0xef
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  // 0xf0 - 0xff
};

char * hextable[0x100] = {
// 0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
   "00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F", // 0x00 - 0x0f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x10 - 0x1f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x20 - 0x2f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x30 - 0x3f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x40 - 0x4f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x50 - 0x5f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x60 - 0x6f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x70 - 0x7f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x80 - 0x8f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0x90 - 0x9f
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0xa0 - 0xaf
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0xb0 - 0xbf
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0xc0 - 0xcf
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0xd0 - 0xdf
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F", // 0xe0 - 0xef
   "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F"  // 0xf0 - 0xff
};

#include "librs232.c"
#include "lib_crc.c"
#include "port.c"
#include "fs.c"
#include "cfg.c"
#include "command.c"
