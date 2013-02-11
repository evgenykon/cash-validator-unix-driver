/* --------------------------------------
 * @title: Client module for FK controller
 * --------------------------------------
 * @parameters:

 * @output:
 * 		<result>
 * -------------------------------------- */
#include 	"lib/defines.h"

int mode2_detect(char * smode) {
	if (strncmp(smode, STRMODE_TEST, strlen(STRMODE_TEST))==0) {
		sys.mode = MODE_TEST;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_STOP, strlen(STRMODE_STOP))==0) {
		sys.mode = MODE_STOP;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_PTEST, strlen(STRMODE_PTEST))==0) {
		sys.mode = MODE_PTEST;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_OPENSMENA, strlen(STRMODE_OPENSMENA))==0) {
		sys.mode = MODE_OPENSMENA;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_CLOSESMENA, strlen(STRMODE_CLOSESMENA))==0) {
		sys.mode = MODE_CLOSESMENA;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_NEWPAYMENT, strlen(STRMODE_NEWPAYMENT))==0) {
		sys.mode = MODE_NEWPAYMENT;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_SELFTEST, strlen(STRMODE_SELFTEST))==0) {
		sys.mode = MODE_SELFTEST;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_GETMODE, strlen(STRMODE_GETMODE))==0) {
		sys.mode = MODE_GETMODE;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_INCASS, strlen(STRMODE_INCASS))==0) {
		sys.mode = MODE_INCASS;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_REPEAT, strlen(STRMODE_REPEAT))==0) {
		sys.mode = MODE_REPEAT;
		return TRUE;
	}
	return FALSE;
}

int process_qmes(char * tmpbuf, int buflen) {
	int tmpresult = 0,i=0;
	send_qmessage(QMSG_WORK_S, getpid(), buflen, tmpbuf);
	for (i=0;i<10;i++) {
		tmpresult = recv_qmessage(QMSG_WORK_R);
		if (tmpresult) {
			if (query.qbody.message[0] == TRUE) {
				return TRUE;
			} else return FALSE;
		}
		usleep(500000);
	}
	return FALSE;
}

int printfile() {
	FILE *checkf;
	struct 	stat buf;
	char line[100], *result, nr[2] = "\n", tmpbuf[100];
	int i,txtlen,tmpresult = FALSE,ret;

	if ((ret = stat(CHECKFILE, &buf))!=0) {
		sys.error = ERR_CHECKFILENOTEX;
		return FALSE;
	}
	checkf=fopen(CHECKFILE, "rb");
	if (!checkf) {
		sys.error = ERR_CHECKFILEBADDEC;
		return FALSE;
	}
	while (1) {
		memset(line,0,100);
		memset(tmpbuf,0,100);
		result = fgets(line,100,checkf);
		if (!result) break;
		for (i=0;i<100;i++) {
			if (line[i] == nr[0]) line[i] = 0;
		}
		printf("send PRINTSTR [%s] request\n",line);
		tmpbuf[0] = CMD_PRINTSTR;
		txtlen = strlen(line);
		tmpbuf[1] = txtlen;
		for (i=0;i<txtlen;i++) {
			tmpbuf[2+i] = line[i];
		}
		tmpresult = process_qmes(tmpbuf,2+txtlen);
		if (!tmpresult) break;
	}
	return tmpresult;
}


int main(int argc, char** argv) {
	int tmpresult = 0,i=0,prlen=0,txtlen=0;
	char tmpbuf[50],tmpbuf2[50];

	DEBUG_flag = 1;
	init_structures();
	tmpresult = init_query(TRUE);
	if (!tmpresult) {
		sys.error = ERR_SYSINITIALIZE;
		printf("init_query fail\n");
		printf("FAIL\n");
	}
	memset(tmpbuf,0,50);
	memset(tmpbuf2,0,50);
	if (argc >= 2) {
		tmpresult = mode2_detect(argv[1]);
		if (!tmpresult) {
			sys.error = ERR_UNKNOWNMODE;
			printf("FAIL\n");
		}
	} else {
		sys.error = ERR_UNKNOWNMODE;
		printf("FAIL\n");
	}
	switch (sys.mode) {
		case MODE_TEST:
			printf("send BEEP request\n");
			tmpbuf[0] = CMD_BEEP;
			process_qmes(tmpbuf,1);
			break;
		case MODE_STOP:
			printf("send QMSG_ABORT_S request\n");
			send_qmessage(QMSG_ABORT_S, getpid(), 1, query.qbody.message);
			for (i=0;i<100;i++) {
				if (recv_qmessage(QMSG_ABORT_R)) break;
				usleep(1000);
			}
			break;
		case MODE_PTEST:
			printf("send PTESTPAGE request\n");
			tmpbuf[0] = CMD_TEST;
			process_qmes(tmpbuf,1);
			break;
		case MODE_OPENSMENA:
			printf("send OPEN_SMENA request\n");
			tmpbuf[0] = CMD_OPENSMENA;
			process_qmes(tmpbuf,1);
			break;
		case MODE_CLOSESMENA:
			printf("send CLOSE_SMENA request\n");
			tmpbuf[0] = CMD_CLOSESMENA;
			process_qmes(tmpbuf,1);
			break;
		case MODE_NEWPAYMENT:
			if (argc >= 4) {
				/*printf("send PRINTTITLE request\n");
				tmpbuf[0] = CMD_PRINTTITLE;
				tmpresult = process_qmes(tmpbuf,1);
				if (!tmpresult) break;*/

				printf("send OPEN_CHECK request\n");
				tmpbuf[0] = CMD_OPENCHECK;
				tmpresult = process_qmes(tmpbuf,1);
				if (!tmpresult) break;

				tmpresult = printfile();
				if (!tmpresult) break;

				printf("send SALE request\n");
				// FORMAT:
				// Price:50, text:MTS
				// <SALE>	<Price length>	<Price chars>		<Text length>	<Text chars>
				// 0		1				2...2+price length	price length+3	price length+4...price length+4+text length
				// [0]=SALE;[1]=2;[2]=5;[3]=0;[4]=3;[5]=M;[6]=T;[7]=S;
				tmpbuf[0] = CMD_SALE;
				prlen = strlen(argv[2]);
				tmpbuf[1] = prlen; //price length
				for (i=0;i<prlen;i++) {
					tmpbuf[2+i] = argv[2][i]; //price chars
				}
				txtlen = strlen(argv[3]);
				printf("text length: %d\n",txtlen);
				tmpbuf[2+prlen] = txtlen; // text message len
				for (i=0;i<txtlen;i++) {
					tmpbuf[3+prlen+i] = argv[3][i]; // text mesage
				}
				tmpresult = process_qmes(tmpbuf,3+prlen+txtlen);
				if (!tmpresult) break;
				printf("send CLOSE_CHECK request\n");
				memset(tmpbuf,0,50);
				tmpbuf[0] = CMD_CLOSECHECK;
				tmpbuf[1] = prlen;
				for (i=0;i<prlen;i++) {
					tmpbuf[2+i] = argv[2][i]; //price chars
				}
				txtlen = strlen(argv[3]);
				printf("text length: %d\n",txtlen);
				tmpbuf[2+prlen] = txtlen; // text message len
				for (i=0;i<txtlen;i++) {
					tmpbuf[3+prlen+i] = argv[3][i]; // text mesage
				}
				tmpresult = process_qmes(tmpbuf,3+prlen+txtlen);
			} else {
				sys.error = ERR_TOOSHORTPARAM;
				printf("unknown mode\n");
			}
			break;
		case MODE_SELFTEST:
			printf("send SELFTEST request\n");
			tmpbuf[0] = CMD_SELFTEST;
			process_qmes(tmpbuf,1);
			break;
		case MODE_GETMODE:
			printf("send GETMODE request\n");
			tmpbuf[0] = CMD_GETMODE;
			process_qmes(tmpbuf,1);
			if (query.qbody.message[0] != FALSE) {
				printf("MODE:%d;\n", query.qbody.message[0]);
				return 1;
			}
		case MODE_INCASS:
			printf("send INCASS request\n");
			tmpbuf[0] = CMD_INCASS;
			process_qmes(tmpbuf,1);
			break;
		case MODE_REPEAT:
			printf("send REPEAT request\n");
			tmpbuf[0] = CMD_REPEAT;
			process_qmes(tmpbuf,1);
			break;
		default:
			printf("unknown mode\n");
	}
	if (query.qbody.message[0] == TRUE) {
		printf("DONE\n");
	} else {
		printf("FAIL\n");
	}
	return 1;
}
