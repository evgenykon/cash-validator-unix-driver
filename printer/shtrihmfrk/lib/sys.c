/* ----------------------------------
 * SYSTEM FUCNTIONS
 * ------------------------------- */
void init_sys() {
	sys.mode = 0;
	sys.cfgload = 0;
	sys.dev_descr = 0;
	sys.dev_conn = 0;
	sys.dev_init = 0;
	sys.error = 0;
	sys.qkey = 0;
	sys.qid = 0;
	return;
}



void init_out() {
	out.result = 0;
	out.mode = 0;
	out.error_code = 0;
	out.responce_code = 0;
	memset(out.message,0,50);
	return;
}



void init_cmd() {
	cmd.stx[0] = SIG_STX;
	cmd.len = 0;
	cmd.cmd[0] = 0;
	memset(cmd.param,0,50);
	cmd.paramlen = 0;
	cmd.lrc[0] = 0;
	memset(cmd.buff,0,260);
	cmd.buflen = 0;
	memset(cmd.param,0,260);
	cmd.paramlen = 0;
	memset(cmd.rbuff,0,260);
	cmd.rbufflen = 0;
	return;
}

void init_fkstate() {
	fkstate.OperNumber = 0;
	fkstate.FrMode = 0;
	fkstate.FrAdvMode = 0;
	fkstate.OperInCheck = 0;
	fkstate.Battery = 0;
	fkstate.Power = 0;
	fkstate.FPErr = 0;
	fkstate.EklzErr = 0;
}

int remove_queue(int qid) {
	if(msgctl(qid, IPC_RMID, 0) == -1) {
		if (DEBUG_flag) printf("remove_queue: fail\n");
		return FALSE;
	}
	if (DEBUG_flag) printf("remove_queue: ok\n");
	return TRUE;
}

int init_query(int rm) {
	query.mtype = 0;
	query.qbody.pid = 0;
	query.qbody.len = 0;
	memset(query.qbody.message,0,1024);
	sys.qkey = ftok(CFGFILE, 0);
	printf("init_query >> started [qkey=%d]\n",sys.qkey);
	sys.qid = msgget(sys.qkey, (IPC_CREAT | IPC_EXCL | 0777));
	if (sys.qid < 0) {
		sys.qid = msgget(sys.qkey, (IPC_CREAT | 0777));
		if (sys.qid < 0) {
			printf("init_query >> msgget error\n");
			return FALSE;
		} else {
			printf("init_query >> create query, new qid [%d]\n",sys.qid);
		}
	} else {
		if (rm) {
			if (!remove_queue(sys.qid)) {
				printf("init_query >> remove_queue [%d] fail\n",sys.qid);
				return FALSE;
			} else {
				sys.qid = msgget(sys.qkey, (IPC_CREAT | 0777));
				if (sys.qid < 0) {
					perror("init_query >> msgget error:");
					return FALSE;
				} else {
					printf("init_query >> create query, new qid [%d]\n",sys.qid);
				}
			}
		} else {
			printf("init_query >> qid [%d]\n",sys.qid);
		}
	}
	return TRUE;
}



int mode_detect(char * smode) {
	if (strncmp(smode, STRMODE_TEST, strlen(STRMODE_TEST))==0) {
		sys.mode = MODE_TEST;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_START, strlen(STRMODE_START))==0) {
		sys.mode = MODE_START;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_SETCONFIG, strlen(STRMODE_SETCONFIG))==0) {
		sys.mode = MODE_SETCONFIG;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_GETCONFIG, strlen(STRMODE_GETCONFIG))==0) {
		sys.mode = MODE_GETCONFIG;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_READ, strlen(STRMODE_READ))==0) {
		sys.mode = MODE_READ;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_SETUPSPEED, strlen(STRMODE_SETUPSPEED))==0) {
		sys.mode = MODE_SETUPSPEED;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_CHECK, strlen(STRMODE_CHECK))==0) {
		sys.mode = MODE_CHECK;
		return TRUE;
	}
	if (strncmp(smode, STRMODE_AUTOCONF, strlen(STRMODE_AUTOCONF))==0) {
		sys.mode = MODE_AUTOCONF;
		return TRUE;
	}
	sys.error = ERR_UNKNOWNMODE;
	return FALSE;
}



void output() {
	printf("%d %d %xh %xh [%s] ",out.result,out.mode,out.error_code,out.responce_code,out.message);
	if (sys.error) {printf("-ERROR\n");} else printf("-DONE\n");
	return;
}



void done(int result) {
	out.mode = sys.mode;
	out.error_code = sys.error;
	output(out);
	exit(result);
	return;
}



void init_structures() {
	init_sys();
	init_config();
	init_out();
	init_cmd();
	return;
}



int LRC(char *str, int len, int offset) {
	int i;
	char *ptr;
	char ch = 0;

	ptr = str + offset;
	for(i=0; i<len; i++)ch ^= ptr[i];
	return ch;
}



void clear_query_buffer() {
	query.qbody.pid = 0;
	query.qbody.len = 0;
	query.mtype = 0;
	memset(query.qbody.message,0,1024);
}



int get_qmsgsize() {
	return sizeof(mess_t);
}



int send_qmessage(long mtype, pid_t pid, int msglen, char *message) {
	int length,i;
	length = sizeof(query.qbody);
	clear_query_buffer();
	query.qbody.pid = pid;
	query.mtype = mtype;
	query.qbody.len = msglen;
	for (i=0;i<msglen;i++) {
		query.qbody.message[i] = message[i];
	}
	if (msgsnd(sys.qid, &query, length, 0) != 0) {
		sys.error = ERR_QMSGSEND;
		if (DEBUG_flag) printf("send_qmessage: send error\n");
		return FALSE;
	} else {
		if (DEBUG_flag) printf("send_qmessage: sended [%x]\n",message[0]);
		return TRUE;
	}
}



int recv_qmessage(long mtype) {
	ssize_t qsize = 0;
	qsize = msgrcv(sys.qid, &query, get_qmsgsize(), mtype, MSG_NOERROR | IPC_NOWAIT);
	if (qsize > 0) {
		if (DEBUG_flag) printf("recv_qmessage: get message [%x]\n",query.qbody.message[0]);
		return TRUE;
	} else return FALSE;
}



int process_message() {
	int tmp=0;
	char tmpchar[100];
	int prlen=0,txtlen=0,i=0,price=0,tmplen=100;
	int msgtype = 0;
	if (sys.dev_init) {
		msgtype = query.qbody.message[0];
		switch(msgtype) {
		case CMD_BEEP:
			if (DEBUG_flag) printf("process_message: command BEEP\n");
			tmp = cmd_Beep();
			return tmp;
		case CMD_TEST:
			if (DEBUG_flag) printf("process_message: command PTESTPAGE\n");
			tmp = cmd_Testprogon();
			return tmp;
		case CMD_OPENSMENA:
			if (DEBUG_flag) printf("process_message: command OPENSMENA\n");
			tmp = cmd_Opensmena();
			return tmp;
		case CMD_CLOSESMENA:
			if (DEBUG_flag) printf("process_message: command CLOSESMENA\n");
			tmp = cmd_Reporttobuffer();
			return tmp;
		case CMD_OPENCHECK:
			if (DEBUG_flag) printf("process_message: command OPENCHECK\n");
			tmp =  cmd_Opencheck();
			return tmp;
		case CMD_CLOSECHECK:
			if (DEBUG_flag) printf("process_message: command CLOSECHECK\n");
			prlen = query.qbody.message[1];
			memset(tmpchar,0,tmplen);
			for (i=0;i<prlen;i++) {
				tmpchar[i] = query.qbody.message[2+i];
			}
			price = atoi(tmpchar);
			memset(tmpchar,0,tmplen);
			txtlen = query.qbody.message[2+prlen];
			for (i=0;i<txtlen;i++) {
				tmpchar[i] = query.qbody.message[3+prlen+i];
			}
			tmp =  cmd_Closecheck(price, tmpchar);
			return tmp;
		case CMD_SALE:
			if (DEBUG_flag) printf("process_message: command SALE\n");
			prlen = query.qbody.message[1];
			memset(tmpchar,0,tmplen);
			for (i=0;i<prlen;i++) {
				tmpchar[i] = query.qbody.message[2+i];
			}
			price = atoi(tmpchar);
			memset(tmpchar,0,tmplen);
			txtlen = query.qbody.message[2+prlen];
			for (i=0;i<txtlen;i++) {
				tmpchar[i] = query.qbody.message[3+prlen+i];
			}
			tmp =  cmd_Sale(price,tmpchar);
			return tmp;
		case CMD_CLICHE:
			if (DEBUG_flag) printf("process_message: command CLICHE\n");
			tmp = cmd_Printcliche();
			return tmp;
		case CMD_ENDDOC:
			if (DEBUG_flag) printf("process_message: command ENDDOC\n");
			tmp = cmd_Enddoc();
			return tmp;
		case CMD_PRINTSTR:
			if (DEBUG_flag) printf("process_message: command PRINTSTR\n");
			txtlen = query.qbody.message[1];
			memset(tmpchar,0,tmplen);
			for (i=0;i<txtlen;i++) {
				tmpchar[i] = query.qbody.message[2+i];
			}
			tmp = cmd_Printstring(tmpchar);
			return tmp;
		case CMD_PRINTTITLE:
			if (DEBUG_flag) printf("process_message: command PRINTTITLE\n");
			tmp = cmd_Printtitle("Платежный документ");
			return tmp;
		case CMD_SELFTEST:
			tmp = cmd_Getfrstate();
			printf("------- SELFTEST -------\n");
			printf("OperNumber:\t%d\n",fkstate.OperNumber);
			printf("FrMode:\t\t%d\n",fkstate.FrMode);
			printf("FrAdvMode:\t%d\n",fkstate.FrAdvMode);
			printf("OperInCheck:\t%d\n",fkstate.OperInCheck);
			printf("Battery:\t%x\n",fkstate.Battery);
			printf("Power:\t\t%x\n",fkstate.Power);
			printf("FPErr:\t\t%d\n",fkstate.FPErr);
			printf("EklzErr:\t%d\n",fkstate.EklzErr);
			printf("------------------------\n");
			if (fkstate.FrMode == 1 && fkstate.FrAdvMode == 0) {
				cmd_Beep();
				return TRUE;
			} else return FALSE;
		case CMD_INCASS:
			if (DEBUG_flag) printf("process_message: command INCASS\n");
			tmp = cmd_Printreportsfrombuffer();
			return tmp;
		case CMD_GETMODE:
			if (DEBUG_flag) printf("process_message: command GETMODE\n");
			tmp = cmd_Getfrstate();
			if (tmp == TRUE) {
				tmp = fkstate.FrMode;
			} else tmp = FALSE;
			return tmp;
		case CMD_REPEAT:
			if (DEBUG_flag) printf("process_message: command REPEAT\n");
			tmp = cmd_Repeatdocument();
			return tmp;
		default:
			if (DEBUG_flag) printf("process_message: unknown command [%x]\n",query.qbody.message[0]);
			return FALSE;
		}
	} else return FALSE;
}


int resp_message(long TYPE, int result) {
	char buf[0];
	buf[0] = result;
	send_qmessage(TYPE, getpid(), 1, buf);
	return TRUE;
}

int evalint(unsigned char *str, int len) {
	int result = 0;
	while(len--) {
		result <<= 8;
		result += str[len];
	};
	return result;
}

int64_t evalint64(unsigned char *str, int len) {
	int64_t result = 0;
	while(len--) {
		result <<= 8;
		result += str[len];
	};
	return result;
}

void evaldate(unsigned char *str, struct tm *date) {
	date->tm_mday = evalint(str    , 1);
	date->tm_mon  = evalint(str + 1, 1) - 1;
	date->tm_year = evalint(str + 2, 1) + 100;
	mktime(date);
}
void evaltime(unsigned char *str, struct tm *time) {
	time->tm_hour = evalint(str    , 1);
	time->tm_min  = evalint(str + 1, 1);
	time->tm_sec  = evalint(str + 2, 1);
	mktime(time);
}

int utf2win1251(char * in, char * out) {
	iconv_t cd;
	size_t k,f,t;
	char *pin = in, *pout = out;
	memset(out,0,200);
	cd = iconv_open("cp1251", "utf-8");
	if(cd == (iconv_t)(-1)) {
		strncpy(out,in,40);
		return FALSE;
	}
	f = strlen(in);
	t = 200;
	k = iconv(cd, &pin, &f, &pout, &t);
	iconv_close(cd);
	return TRUE;
}
