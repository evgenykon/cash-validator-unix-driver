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

int set_tty() {
	int speed;
	struct termios tios;

	if (tcgetattr(CFG.fd, &tios) < 0) {
		return FALSE;
	}

	tios.c_cflag     &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
	tios.c_cflag     |= CS8 | CREAD | HUPCL;

	tios.c_iflag      = IGNBRK | IGNPAR;
	tios.c_oflag      = 0;
	tios.c_lflag      = 0;
	tios.c_cc[VMIN]   = 1;
	tios.c_cc[VTIME]  = 0;

	tios.c_cflag &= ~CRTSCTS;

	speed = B9600;
	if (speed) {
		cfsetospeed (&tios, speed);
		cfsetispeed (&tios, speed);
	}
	if (tcsetattr(CFG.fd, TCSAFLUSH, &tios) < 0) {
		return FALSE;
	}
	return TRUE;
}

