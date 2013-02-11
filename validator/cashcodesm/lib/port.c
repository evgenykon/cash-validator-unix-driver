/* --------------------------------------------------- *
 * Includer with port-oriented functions
 * @version:	1.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 * --------------------------------------------------- */


int read_port(int fd, char buf[255]) {
	int res;

	memset(buf,0,255);
	fcntl(fd, F_SETFL, FNDELAY);
	res = read(fd,buf,255);
	return res;
}

void write_port(int portdesc, char * buf, int size) {
	int i;
	for (i=0; i<size; i++) {
		write(portdesc, &buf[i], 1);
		usleep(1000);
	}
}


int rs232init(char * device) {
	HRS232 hRS232 = NULL;
	RS232_PARAM param;
	char chrSend;

	hRS232 = RS232_Open  (device);

	if(hRS232==NULL) return 0;
	param.BaudRate = BR_9600;
	param.DataBits = DB_8;
	param.Parity = PARITY_NONE;
	param.StopBits = SB_1;
	param.xonxoff = 0;
	param.rtscts = 0;
	RS232_SetParam(hRS232, &param);
	RS232_Close (hRS232);
}

int rs232init4800(char * device) {
	HRS232 hRS232 = NULL;
	RS232_PARAM param;
	char chrSend;

	hRS232 = RS232_Open  (device);

	if(hRS232==NULL) return 0;
	param.BaudRate = BR_4800;
	param.DataBits = DB_8;
	param.Parity = PARITY_NONE;
	param.StopBits = SB_1;
	param.xonxoff = 0;
	param.rtscts = 0;
	RS232_SetParam(hRS232, &param);
	RS232_Close (hRS232);
}
