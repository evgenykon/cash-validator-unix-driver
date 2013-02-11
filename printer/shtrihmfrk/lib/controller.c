/* ----------------------------------
 * CONTROL DEVICE
 * ------------------------------- */



int set_tty() {
	int speed;
	struct termios tios;

	if (tcgetattr(sys.dev_descr, &tios) < 0) {
		sys.error = ERR_SETUPTTY;
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

	speed = LineSpeedVal[get_index_baudrate()];
	if (speed) {
		cfsetospeed (&tios, speed);
		cfsetispeed (&tios, speed);
	}
	if (tcsetattr(sys.dev_descr, TCSAFLUSH, &tios) < 0) {
		sys.error = ERR_SETUPTTY;
		return FALSE;
	}
	return TRUE;
}



int open_device() {
	int buf[1] = {SIG_ENQ};
	unsigned char readbuff[2] = "";
	int readed = 0;
	static int fdflags;

	if (DEBUG_flag) printf("open_device: [%s]\n",cfg.device);
	sys.dev_init = FALSE;
	sys.dev_descr = open(cfg.device, O_NONBLOCK | O_RDWR, 0);
	if (!sys.dev_descr) {
		sys.error = ERR_OPENDEVICEFAIL;
		if (DEBUG_flag) perror("open_device: err 1: ");
		return FALSE;
	}
	fcntl(sys.dev_descr, F_GETFL);
	fcntl(sys.dev_descr, F_SETFL, fdflags & ~O_NONBLOCK);
	if (DEBUG_flag) printf("open_device: setup tty\n");
	if (!set_tty()) return FALSE;
	write(sys.dev_descr,buf,1);
	if (DEBUG_flag) printf("open_device: write ENQ\n");
	usleep(1000);
	readed = read(sys.dev_descr, readbuff, 1);
	if (readed < 0) {
		if (DEBUG_flag) printf("open_device: error\n");
		sys.error = ERR_OPENDEVICEFAIL;
		return FALSE;
	} else {
		if (readed > 0) {
			out.responce_code = readbuff[0];
			if (DEBUG_flag) printf("open_device: responce ENQ => [%x], readed %d\n",out.responce_code,readed);
			sys.dev_init = TRUE;
		} else {
			sys.dev_init = FALSE;
			if (DEBUG_flag) printf("open_device: device not response\n");
		}
		return sys.dev_init;
	}
}



int close_device(void) {
	return close(sys.dev_descr);
}



int compose_command(int comm, int pass) {
	int len;

	len = commlen[comm];
	if(len >= 5 && cmd.paramlen > (len - 5)) cmd.paramlen = len - 5;
	cmd.buff[0] = SIG_STX;
	cmd.buff[1] = len;
	cmd.buff[2] = comm;
	if(len >= 5) {
		memcpy(cmd.buff + 3, &pass, sizeof(int));
		if(cmd.paramlen > 0) memcpy(cmd.buff + 7, cmd.param, cmd.paramlen);
	}
	cmd.buff[len + 2] = LRC(cmd.buff, len + 1, 1);
	cmd.len = len + 3;
	cmd.buflen = cmd.len;
	return 1;
}



int read_device(int timeout) {
	int readed = 0;
	int tries = 20;
	int bufcnt = 0;
	int i=0,len=0;
	char tmpread[100];
	if (DEBUG_flag) printf("read_device: control bytes ... \n");
	cmd.rbufflen = 0;
	memset(tmpread,0,100);
	tries=20;
	printf("read [");
	while(1) {
		readed = read(sys.dev_descr, tmpread, 1);
		if (readed > 0) {
			if (tmpread[0]==SIG_STX) {
				printf("] - done\n");
				break;
			} else {
				if (DEBUG_flag) printf("read unknown byte [%xh]\n",cmd.rbuff[0]);
			}
		}
		tries--;
		printf(".");
		if (tries<=0) {
			printf("] - timeout\n");
			if (DEBUG_flag) printf("read_device: timeout\n");
			return FALSE;
		}
		usleep(timeout * USLEEP_ADD);
		//fflush(stdout);
		//sleep(1);
	}
	memset(tmpread,0,100);
	readed = read(sys.dev_descr, tmpread, 1);
	if (readed > 0) {
		len = tmpread[0];
	} else return FALSE;
	usleep(cfg.port_timeout * USLEEP_ADD);
	memset(tmpread,0,100);
	cmd.rbufflen = 2;
	if (len>0) {
		if (DEBUG_flag) printf("read_device: [ %xh %xh | ",SIG_STX,len);
		for (i=0;i<len+1;i++) {
			readed = read(sys.dev_descr, tmpread, 1);
			if (readed > 0) {
				cmd.rbuff[i+2] = tmpread[0];
				cmd.rbufflen++;
				//if (DEBUG_flag) printf("%xh ",tmpread[0]);
			} else {
				sys.error = ERR_READPORT;
				return FALSE;
			}
			usleep(timeout * USLEEP_ADD);
		}
		if (DEBUG_flag) printf("]\n");
	} else {
		if (DEBUG_flag) printf("ERR: length is null\n");
		return FALSE;
	}
	return TRUE;
}



int send_command() {
	int i=0;
	char buftmp[1] = {0};
	if (sys.dev_init) {
		if (DEBUG_flag) printf("send_command: [");
		if (cmd.buflen > 0) {
			for (i=0;i<cmd.buflen;i++) {
				buftmp[0] = cmd.buff[i];
				write(sys.dev_descr,buftmp,1);
				//if (DEBUG_flag) printf("%xh ",buftmp[0]);
				//fflush(stdout);
				if (cfg.port_timeout > 10) usleep(cfg.port_timeout * USLEEP_ADD);
			}
			if (DEBUG_flag) printf("]\n");
			usleep(cfg.port_timeout * USLEEP_ADD);
			if (read_device(cfg.port_timeout)) {
				sendACK();
				return TRUE;
			} else sendNAK();
		} else {
			if (DEBUG_flag) printf("send_command: output buffer empty\n");
			sys.error = ERR_OUTBUFEMPTY;
			return FALSE;
		}
	} return FALSE;
}



int sendNAK(void) {
	char buff[2];
	buff[0] = SIG_NAK;
	if (DEBUG_flag) printf("send NAK\n");
	return write(sys.dev_descr,buff,1);
}



int sendACK(void) {
	char buff[2];
	buff[0] = SIG_ACK;
	if (DEBUG_flag) printf("send ACK\n");
	return write(sys.dev_descr,buff,1);
}

int autoconf() {
	int i=0,j=0;
	char device[11];
	int speeds[7] = {2400,4800,9600,19200,38400,57600,115200};
	int timeouts[10] = {0, 10, 50, 100, 200, 250, 300, 350, 400, 500};
	//DEBUG_flag = 0;
	for (j=0;j<7;j++) {
		memset(device,0,11);
		strncat(device,"/dev/ttyS2",strlen("/dev/ttyS2"));
		for (i=0;i<10;i++) {
			init_config();
			strncat(cfg.device,device,strlen(device));
			cfg.port_speed = speeds[j];
			cfg.port_timeout = timeouts[i];
			cfg.err_timeout = 10000;
			sys.cfgload = TRUE;
			printf ("Autoconfig: checking... %s, speed %d, timeout %d\n",cfg.device,cfg.port_speed,cfg.port_timeout);
			if (open_device()==TRUE) {
				if (cmd_Beep()) {
					printf("Autoconfig: done!\n");
					save_config();
					return TRUE;
				}
			}
			close_device();
			usleep(1000 * USLEEP_ADD);
		}
	}
}
