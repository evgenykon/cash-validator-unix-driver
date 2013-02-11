void init_structures() {
	command.sync[0] = 0;
	command.length = 0;
	command.cmd[0] = 0;
	memset(command.data,0,256);
	command.datalen = 0;
	command.crc[0] = 0;
	command.crc[1] = 0;
	memset(command.output,0,261);

	answer.sync[0] = 0;
	answer.length = 0;
	answer.cmd[0] = 0;
	memset(answer.inbuf,0,261);
	answer.bufsize = 0;
	memset(answer.data,0,255);
	answer.datasize = 0;
}

void init_nominals() {
	nominals.n1 = 0;
	nominals.n2 = 0;
	nominals.n3 = 0;
	nominals.n4 = 0;
	nominals.n5 = 0;
	nominals.n6 = 0;
	nominals.n7 = 0;
	nominals.n8 = 0;
	memset(nominals.buf,0,2);
}

int compose_nominals() {
	int tmp;
	nominals.buf[0] = 0xff;
	nominals.buf[1] = 0x00;
	nominals.buf[1] = nominals.n1 * 0x80 | nominals.n2 * 0x40 | nominals.n3 * 0x20 |
			nominals.n4 * 0x10 | nominals.n5 * 0x8 | nominals.n6 * 0x4 |
			nominals.n7 * 0x2 | nominals.n8;
	printf("compose_nominals: data [%02X]\n", nominals.buf[1]);
	return TRUE;
}

int compose_command(int cmdbyte) {
	unsigned short crc_kermit, low_byte, high_byte, high_byte1;
	char buffer[260];
	char *ptr,prev_byte;
	int i,l;

	command.sync[0] = SIG_SYNC;
	if (cmdbyte < 0xff) {
		command.length = commlen[cmdbyte];
	}
	command.cmd[0] = cmdbyte;
	memset(buffer,0,260);
	l=3;
	command.output[0] = command.sync[0];
	command.output[1] = command.length;
	command.output[2] = command.cmd[0];
	for (i=0;i<command.datalen;i++) {
		command.output[i+3] = command.data[i];
		l++;
	}
	//crc
	crc_kermit = 0;
	prev_byte = 0;
	ptr = command.output;
	while ( *ptr ) {
		crc_kermit = update_crc_kermit(crc_kermit,*ptr);
		prev_byte = *ptr;
		ptr++;
	}
	low_byte   = (crc_kermit & 0xff00) >> 8;
	high_byte1 = (crc_kermit & 0x00ff);
	high_byte  = (crc_kermit & 0x00ff) << 8;
	crc_kermit = low_byte | high_byte;
	command.crc[0] = high_byte1;
	command.crc[1] = low_byte;
	command.output[command.length-2] = command.crc[0];
	command.output[command.length-1] = command.crc[1];
	printf("Compose command: [");
	for (i=0;i<command.length;i++) {
		printf("%02X ",command.output[i]);
	}
	printf("]\n");
	return TRUE;
}


int send_comand() {
	printf("send_comand... starting\n");
	fflush(stdout);
	CFG.fd = open_file(CFG.device_addr);
	rs232init(CFG.device_addr);
	if (CFG.fd) {
		if (write_port(CFG.fd, command.output, command.length) < 0)	return FALSE;
	} else return FALSE;
	printf("send_comand... done\n");
	fflush(stdout);
	return TRUE;
}

int write_port(int portdesc, char * buf, int size) {
	int i;
	printf("Write port: [ ");
	for (i=0; i<size; i++) {
		printf("%02X ",buf[i]);
		fflush(stdout);
		if (write(portdesc, &buf[i], 1) < 0) {
			printf("] .. FALSE\n");
			return FALSE;
		}
		usleep(1000);
	}
	printf("]\n");
	return TRUE;
}

int read_device() {
	int total_readed = 0, triescnt = READ_TRIES, i=0, tmpreaded=0;
	char	readbuf[1] = {0};
	printf("read_device... starting\n");
	fflush(stdout);
	// lookig for SYNC byte
	while(1) {
		tmpreaded = 0;
		readbuf[0] = 0;
		printf("read_device tries [%d]\n",triescnt);
		fflush(stdout);
		tmpreaded = read(CFG.fd, readbuf, 1);
		if (tmpreaded >= 0) {
			if (tmpreaded > 0) {
				if (readbuf[0] != SIG_SYNC) {
					printf("read_device: get wrong byte [%02x]\n",readbuf[0]);
				} else break; // found SYNC byte
			} else {
				printf("read_device: empty\n");
				triescnt--;
			}
		} else {
			printf("read_device:\n");
			perror("\t!!! ERROR");
			return FALSE;
		}
		if (triescnt<=0) {
			printf("read_device: timeout\n");
			return FALSE;
			break;
		}
		usleep(WAITTIMEOUT);
	}
	// read responce from next byte
	total_readed = 1;
	answer.inbuf[0] = SIG_SYNC;
	while(1) {
		tmpreaded = 0;
		readbuf[0] = 0;
		tmpreaded = read(CFG.fd, readbuf, 1);
		if (tmpreaded == 1) {
			answer.inbuf[total_readed] = readbuf[0];
			total_readed++;
		} else break;
		usleep(WAITTIMEOUT);
	}
	// format responce message
	if (total_readed >= 5 && total_readed < 256) {
		answer.sync[0] = answer.inbuf[0];
		answer.length = answer.inbuf[1];
		if (answer.length != total_readed) printf("read_device: message length not equal [%d must / %d get]\n",answer.length,total_readed);
		answer.cmd[0] = answer.inbuf[2];
		for (i=0;i<total_readed-3;i++) {
			answer.data[i] = answer.inbuf[i+3];
			answer.datasize++;
		}
		if (answer.length > total_readed) {
			printf("read_device: format error (compare length)\n");
			return FALSE;
		}
		answer.bufsize = total_readed;
		return TRUE;
	} else {
		printf("read_device: format error (message to short)\n");
		return FALSE;
	}
}


int cmd_Reset() {
	init_structures();
	compose_command(CMD_RESET);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Ack() {
	init_structures();
	compose_command(CMD_ACK);
	return send_comand();
}

int cmd_Stack1() {
	init_structures();
	compose_command(CMD_STACK1);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Stack2() {
	init_structures();
	compose_command(CMD_STACK2);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Return() {
	init_structures();
	compose_command(CMD_RETURN);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Hold() {
	init_structures();
	compose_command(CMD_HOLD);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Wait() {
	init_structures();
	compose_command(CMD_WAIT);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_Setcmd_enabledisable() {
	init_structures();
	init_nominals();
	nominals.n1 = 0;
	nominals.n2 = 0;
	nominals.n3 = 1;
	nominals.n4 = 1;
	nominals.n5 = 1;
	nominals.n6 = 1;
	nominals.n7 = 0;
	nominals.n8 = 0;
	compose_nominals();
	command.datalen = 2;
	command.data[0] = nominals.buf[0];
	command.data[1] = nominals.buf[1];
	compose_command(CMD_SETENDIS);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}

int cmd_State() {
	init_structures();
	compose_command(CMD_STATE);
	if (send_comand()) {
		usleep(WAITTIMEOUT);
		return read_device();
	} else return FALSE;
}
