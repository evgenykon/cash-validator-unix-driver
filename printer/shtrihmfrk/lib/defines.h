/* ----------------------------------
 * HEADER FILE
 * ------------------------------- */

// ******* Includes ********
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <math.h>
#include <iconv.h>


// ******** Defines **********
#define 	ERR_LOADCONFFAIL 	0x01
#define		ERR_UNKNOWNMODE		0x02
#define		ERR_ILLEGALMODE		0x03
#define		ERR_OPENDEVICEFAIL	0x04
#define		ERR_CONNECTFAIL		0x05
#define		ERR_CFGBADDESCR		0x06
#define		ERR_CFGNOTEXIST		0x07
#define		ERR_SETUPTTY		0x08
#define		ERR_WRONGBOUDRATE	0x09
#define		ERR_READPORT		0x0A
#define		ERR_PORTBUFEMPTY	0x0B
#define		ERR_OUTBUFEMPTY		0x0C
#define		ERR_QMSGSEND		0x0D
#define		ERR_TOOSHORTPARAM	0x0F
#define		ERR_CHECKFILENOTEX	0x10
#define		ERR_CHECKFILEBADDEC	0x11
#define		ERR_SYSINITIALIZE	0x12
#define 	CFGFILE 			"/var/www/bin/printer/shtrihmfrk/settings.cfg"
#define		CHECKFILE			"/var/www/bin/printer/shtrihmfrk/check_text.txt"
#define 	SIG_ENQ 			0x05
#define 	SIG_STX 			0x02
#define 	SIG_ACK 			0x06
#define 	SIG_NAK 			0x15
#define 	MODE_START			0x01
#define 	MODE_TEST			0x02
#define		MODE_STOP			0x03
#define		MODE_SETCONFIG 		0x07
#define		MODE_GETCONFIG 		0x08
#define		MODE_PTEST	 		0x09
#define		MODE_READ			0x0A
#define		MODE_OPENSMENA		0x0B
#define		MODE_CLOSESMENA		0x0C
#define		MODE_NEWPAYMENT		0x0D
#define		MODE_OPENCHECK		0x0E
#define		MODE_CLOSECHECK		0x0F
#define		MODE_SALE			0x10
#define		MODE_SELFTEST		0x11
#define		MODE_GETMODE		0x12
#define		MODE_SETUPSPEED		0x13
#define		MODE_CHECK			0x14
#define		MODE_AUTOCONF		0x15
#define		MODE_INCASS			0x16
#define		MODE_REPEAT			0x17
#define		STRMODE_START 		"start"
#define		STRMODE_TEST	 	"test"
#define 	STRMODE_SETCONFIG	"setconfig"
#define 	STRMODE_GETCONFIG	"getconfig"
#define 	STRMODE_STOP		"stop"
#define		STRMODE_PTEST		"ptest"
#define		STRMODE_READ		"read"
#define		STRMODE_OPENSMENA	"opensmena"
#define		STRMODE_CLOSESMENA	"closesmena"
#define		STRMODE_NEWPAYMENT	"newpayment"
#define		STRMODE_SELFTEST	"selftest"
#define		STRMODE_GETMODE		"getmode"
#define		STRMODE_SETUPSPEED	"setupspeed"
#define		STRMODE_CHECK		"check"
#define		STRMODE_AUTOCONF	"autoconf"
#define		STRMODE_INCASS		"incass"
#define		STRMODE_REPEAT		"repeat"
#define		TRUE				1
#define		FALSE				0
#define		USLEEP_ADD			100
#define		QMSG_WORK_S			1
#define		QMSG_WORK_R			2
#define		QMSG_ABORT_S		3
#define		QMSG_ABORT_R		4

#define		CMD_BEEP				0x01
#define		CMD_TEST				0x02
#define		CMD_OPENSMENA			0x03
#define		CMD_CLOSESMENA			0x04
#define		CMD_OPENCHECK			0x05
#define		CMD_CLOSECHECK			0x06
#define		CMD_SALE				0x07
#define		CMD_CLICHE				0x08
#define		CMD_PRINTADVERT			0x09
#define		CMD_ENDDOC				0x0A
#define		CMD_PRINTSTR			0x0B
#define		CMD_PRINTTITLE			0x0C
#define		CMD_SELFTEST			0x0D
#define		CMD_GETMODE				0x0E
#define		CMD_INCASS				0x0F
#define		CMD_REPEAT				0x10

#define DUMP_REQUEST						0x01
#define GET_DATA							0x02
#define INTERRUPT_DATA_STREAM				0x03
#define GET_SHORT_ECR_STATUS				0x10
#define GET_ECR_STATUS						0x11
#define PRINT_WIDE_STRING					0x12
#define BEEP								0x13
#define SET_EXCHANGE_PARAM					0x14
#define GET_EXCHANGE_PARAM					0x15
#define RESET_SETTINGS						0x16
#define PRINT_STRING						0x17
#define PRINT_DOCUMENT_TITLE				0x18
#define TEST								0x19
#define GET_CASH_REG						0x1a
#define GET_OPERATION_REG					0x1b
#define WRITE_LICENSE						0x1c
#define READ_LICENSE						0x1d
#define WRITE_TABLE							0x1e
#define READ_TABLE							0x1f
#define SET_POINT_POSITION					0x20
#define SET_TIME							0x21
#define SET_DATE							0x22
#define CONFIRM_DATE						0x23
#define INIT_TABLE							0x24
#define CUT_CHECK							0x25
#define RESET_SUMMARY						0x27
#define OPEN_DRAWER							0x28
#define FEED_DOCUMENT						0x29
#define EJECT_SLIP_DOC						0x2a
#define INTERRUPT_TEST						0x2b
#define PRINT_OPERATION_REG					0x2c
#define GET_TABLE_STRUCT					0x2d
#define GET_FIELD_STRUCT					0x2e
#define PRINT_REPORT_WITHOUT_CLEANING		0x40
#define PRINT_REPORT_WITH_CLEANING			0x41
#define CASH_INCOME							0x50
#define CASH_OUTCOME						0x51
#define PRINT_CLICHE						0x52
#define END_DOC								0x53
#define PRINT_ADVERT						0x54
#define SET_SERIAL_NUMBER					0x60
#define INIT_FM								0x61
#define GET_FM_RECORDS_SUM					0x62
#define GET_LAST_FM_RECORD_DATE				0x63
#define GET_RANGE_DATES_AND_SESSIONS		0x64
#define FISCALIZATION						0x65
#define FISCAL_REPORT_FOR_DATES_RANGE		0x66
#define FISCAL_REPORT_FOR_SESSION_RANGE		0x67
#define INTERRUPT_FULL_REPORT				0x68
#define GET_FISCALIZATION_PARAMETERS		0x69
#define FISCAL_PRINT_SLIP_DOC	0x70
#define PRINT_SLIP_DOC			0x71
#define SALE					0x80
#define BUY						0x81
#define RETURN_SALE				0x82
#define RETURN_BUY				0x83
#define STORNO					0x84
#define CLOSE_CHECK				0x85
#define DISCOUNT				0x86
#define CHARGE					0x87
#define CANCEL_CHECK			0x88
#define CHECK_SUBTOTAL			0x89
#define STORNO_DISCOUNT			0x8a
#define STORNO_CHARGE			0x8b
#define REPEAT_DOCUMENT			0x8c
#define	OPEN_CHECK				0x8d
#define DOZE_OIL_CHECK			0x90
#define SUMM_OIL_CHECK			0x91
#define CORRECTION				0x92
#define SET_DOZE_IN_MILLILITERS	0x93
#define SET_DOZE_IN_MONEY		0x94
#define OIL_SALE				0x95
#define STOP_RK					0x96
#define LAUNCH_RK				0x97
#define RESET_RK				0x98
#define RESET_ALL_TRK			0x99
#define SET_RK_PARAMETERS		0x9a
#define GET_LITER_SUM_COUNTER	0x9b
#define GET_CURRENT_DOZE		0x9e
#define GET_RK_STATUS			0x9f
#define ECT_DEP_DATE_REP		0xa0
#define ECT_DEP_SHIFT_REP		0xa1
#define ECT_SHIFT_DATE_REP		0xa2
#define ECT_SHIFT_SHIFT_REP		0xa3
#define ECT_SHIFT_TOTAL			0xa4
#define ECT_PAY_DOC_BY_NUMBER	0xa5
#define ECT_BY_SHIFT_NUMBER		0xa6
#define ECT_REPORT_INTR			0xa7
#define CONTINUE_PRINTING		0xb0
#define LOAD_LINE_DATA			0xc0
#define DRAW					0xc1
#define PRINT_BARCODE			0xc2
#define BUF_REPORT_WITH_CLEANING	0xc6
#define PRINT_REPORTS_FROM_BUF		0xc7
#define	OPEN_SMENA				0xe0
#define GET_DEVICE_METRICS		0xfc
#define CTRL_ADD_DEVICE			0xfd
#define	PTESTPAGE				0x30

// ****** Structures ********
typedef struct {
	char	device[50];
	int		port_speed;
	int		port_timeout;
	int		err_timeout;
} cfgstruct;
typedef struct {
	int mode;
	int cfgload;
	int dev_descr;
	int dev_conn;
	int dev_init;
	int	error;
	key_t	qkey;
	int	qid;
} systemvars;
typedef struct {
	int		result;
	int		mode;
	int		error_code;
	int		responce_code;
	char	message[50];
} outstruct;
typedef struct {
	char	stx[1];
	int		len;
	char	cmd[1];
	char	param[260];
	int		paramlen;
	char	lrc[1];
	char	buff[260];
	int		buflen;
	char	rbuff[260];
	int		rbufflen;
} command;
typedef struct querymsgbuf {
	long	mtype;
	struct {
		pid_t	pid;
		int		len;
		char 	message[1024];
	} qbody;
} mess_t;
typedef struct {
	int 	OperNumber;
	int		FrMode;
	int		FrAdvMode;
	int		OperInCheck;
	int		Battery;
	int		Power;
	int		FPErr;
	int		EklzErr;
} fkstatestruct;


// ******* Variables *********
cfgstruct 	cfg;
systemvars 	sys;
outstruct	out;
command		cmd;
mess_t		query;
fkstatestruct fkstate;
static 		fd_set 	set;
const unsigned commlen[0x100] = {
   0,  6,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00 - 0x0f
   5,  5, 26,  5,  8,  6,  1, 46, 37,  6,  6,  6, 10,  5,255,  9, // 0x10 - 0x1f
   6,  8,  8,  8,  5,  6,  0,  5,  6,  7,  5,  5,  5,  6,  7,  0, // 0x20 - 0x2f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x30 - 0x3f
   5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x40 - 0x4f
  10, 10,  5,  6,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x50 - 0x5f
   9,  1,  6,  5,  5, 20, 12, 10,  5,  6,  0,  0,  0,  0,  0,  0, // 0x60 - 0x6f
 255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x70 - 0x7f
  60, 60, 60, 60, 60, 71, 54, 54,  5,  5, 54, 54,  5,  6,  0,  0, // 0x80 - 0x8f
  61, 57, 52, 11, 12, 52,  7,  7,  7,  5, 13,  7,  0,  0,  7,  7, // 0x90 - 0x9f
  13, 11, 12, 10, 10,  8,  7,  5,  0,  0,  0,  0,  0,  0,  0,  0, // 0xa0 - 0xaf
   5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xb0 - 0xbf
  46,  7, 10,  0,  0,  0,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0, // 0xc0 - 0xcf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xd0 - 0xdf
   5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xe0 - 0xef
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,255,  0,  0  // 0xf0 - 0xff
};
const speed_t LineSpeedVal[7] = {B2400, B4800, B9600, B19200, B38400, B57600, B115200};
int DEBUG_flag;

// ******* Modules **********
#include "cfg.c"
#include "sys.c"
#include "commands.c"
#include "controller.c"
