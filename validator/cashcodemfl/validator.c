/* -------------------------------------------------
 * Unix port-level controller for Cashcode Validator
 * using ID003(JCM) protocol
 *
 * @version:	2.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 *
 * OUTPUT FORMAT:
 * <RESULT CODE>|<MESSAGE CODE>|<DATA>
 *
 * RESULT CODES:
 * 80 - error (unknown mode)
 * 81 - error (wrong parameter count)
 * 82 - error (cannot open device)
 * 83 - error (no params)
 * 84 - error (cant reading config)
 * 11 - ok with responce
 * 12 - ok empty
 * 21 - write reset ok
 * 22 - write ack ok
 * 23 - write poll ok
 * 29 - read port empty
 * 2a - read port error
 * 2b - read port success
 * ------------------------------------------------- */
#include "lib/defines.c"


int output (char * code, char * mess, char * field) {
	printf("%s|%s|%s\n",code,mess,field);
}

int print_Answer() {
	int i=0;
	char data2[510];
	memset(data2,0,510);
	for (i=0;i<answer.datasize;i++) {
		strncat(data2,hextable[answer.data[i]],2);
	}
	output(OUT_OKR, hextable[answer.cmd[0]], data2);
}

int main(int argc, char** argv) {
	/* Parameters:
	 * 	1 reset
	 * 	2 ack
	 * 	3 poll
	 */
	int res = 0;

	if (read_config()<0) {
		output(OUT_ERR_CONFIGERROR,OUT_NULLS,OUT_NULLS);
		return -1;
	}

	if (argc == 2) {
		mode_detect(argv[1]);
		switch (CFG.mode) {
		case MODE_RESET:
			cmd_Reset();
			break;
		case MODE_STATE:
			if (cmd_State()) {
				print_Answer();
			} else output(OUT_FAIL_EXCHANGE,OUT_NULLS,OUT_NULLS);

			break;
		case MODE_SCENDIS:
			cmd_Setcmd_enabledisable();
			break;
		default:
			output(OUT_ERR_UNKMODE,OUT_NULLS,OUT_NULLS);
			break;
		}
	} else {
		output(OUT_ERR_CONFIGERROR,OUT_NULLS,OUT_NULLS);
	}
	close(CFG.fd);
	return 0;
}
