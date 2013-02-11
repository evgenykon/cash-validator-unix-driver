/* --------------------------------------
 * @title: FR controller/driver for UNIX
 * --------------------------------------
 * @parameters:
 * 		- start
 * 		- test
 * 		- setconfig
 * 		- getconfig
 * @output:
 * 		<result> <mode> <error> <responce> [<message>] -<text result>
 * -------------------------------------- */

#include 	"lib/defines.h"

int main(int argc, char** argv) {

	int tmpresult = 0;

	DEBUG_flag = 1;

	init_structures();
	tmpresult = init_query(TRUE);
	if (!tmpresult) {
		sys.error = ERR_SYSINITIALIZE;
		done(1);
	}
	// load configuration file
	tmpresult = load_config();
	if (!tmpresult) sys.error = ERR_LOADCONFFAIL;
	// detect application mode
	if (argc >= 2) {
		tmpresult = mode_detect(argv[1]);
		if (!tmpresult) {
			sys.error = ERR_UNKNOWNMODE;
			done(2);
		}
	} else {
		sys.error = ERR_UNKNOWNMODE;
		done(3);
	}
	// switch modes
	switch (sys.mode) {
	case MODE_START:
		open_device();
		if (sys.dev_init) {
			while (1) {
				if (recv_qmessage(QMSG_WORK_S)) {
					tmpresult = 0;
					if (query.qbody.len > 0) {
						tmpresult = process_message();
					}
					resp_message(QMSG_WORK_R, tmpresult);
					continue;
				}
				if (recv_qmessage(QMSG_ABORT_S)) {
					if (DEBUG_flag) printf("---- Stopping FR controller -----\n");
					resp_message(QMSG_ABORT_R, TRUE);
					break;
				}
			}
		}
		close_device();
		done(5+MODE_START);
		break;
	case MODE_SETCONFIG:
		printf("\t--------------- SET CONFIGURATION -----------------\n");
		printf("\tPlease, enter device address: ");
		scanf("%s",cfg.device);
		printf("\tPort speed: ");
		scanf("%d",&cfg.port_speed);
		printf("\tRead timeout: ");
		scanf("%d",&cfg.port_timeout);
		printf("\tResponce timeout: ");
		scanf("%d",&cfg.err_timeout);
		out.result = save_config();
		printf("\tThanks. Settings saved.\n");
		out.result = TRUE;
		done(5+MODE_SETCONFIG);
		break;
	case MODE_GETCONFIG:
		printf("\t-------- PRINT CONFIGURATION ----------\n");
		printf("\tDevice port: \t[%s]\n",cfg.device);
		printf("\tPort speed: \t[%d]\n",cfg.port_speed);
		printf("\tRead timeout: \t[%d]\n",cfg.port_timeout);
		printf("\tResp timeout: \t[%d]\n",cfg.err_timeout);
		printf("\t---------------------------------------\n");
		out.result = TRUE;
		done(5+MODE_GETCONFIG);
		break;
	case MODE_TEST:
		open_device();
		if (sys.dev_init) {
			cmd_Testprogon();
		}
		close_device();
		done(5+MODE_TEST);
		break;
	case MODE_READ:
		// mode for empty device buffer
		open_device();
		if (sys.dev_init) {
			printf("Read while empty...\n");
			while(1) {
				read_device(1000);
			}
		} else {
			out.result = FALSE;
			printf("Device not inited\n");
		}
		close_device();
		done(5+MODE_READ);
		break;
	case MODE_SETUPSPEED:
		if (argc >= 4) {
			cfg.port_speed = atoi(argv[2]);
			open_device();
			if (sys.dev_init) {
				tmpresult = atoi(argv[3]);
				cmd_Setdevicespeed(tmpresult);
			}
			close_device();
			cfg.port_speed = atoi(argv[3]);
			open_device();
			if (sys.dev_init) {
				cmd_Beep();
			}
			close_device();
		} else {
			sys.error = ERR_ILLEGALMODE;
			done(5+MODE_SETUPSPEED);
		}
		break;
	case MODE_CHECK:
		open_device();
		if (sys.dev_init) {
			cmd_Beep();
		}
		close_device();
		done(5+MODE_CHECK);
		break;
	case MODE_AUTOCONF:
		out.result = autoconf();
		done(5+MODE_AUTOCONF);
		break;
	default:
		out.error_code = sys.error;
		done(5);
	}
	return 0;
}
