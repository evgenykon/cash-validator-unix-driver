/* -------------------------------------------------
 * Unix port-level controller for Cashcode Validator
 * using CC-Net protocol
 *
 * @version:	2.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 *
 * OUTPUT FORMAT:
 * <RESULT CODE>|<MESSAGE CODE>|<FULL CODE>
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
 * 24 - write bill table ok
 * 25 - write disbtbls ok
 * 26 - write enbiltbls ok
 * 27 - write identif ok
 * 28 - write set secr ok
 * 29 - read port empty
 * 2a - read port error
 * 2b - read port success
 * ------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "lib/librs232.c"
#include "lib/port.c"
#include "lib/fs.c"
#include "lib/cfg.c"

int output (char code[3], char mess[3], char field[3]) {
	printf("%s|%s|%s\n",code,mess,field);
}


int read_proc(int with_out) {
	char devicebuf[255], devicecode[255], singlechar[10], message[3];
	int read_size,i,n,errs=5,empt=100;

	memset(devicebuf,0,255);
	memset(devicecode,0,255);
	memset(message,0,3);
	while (1) {
		read_size = read_port(CFG.fd,&*devicebuf);
		if (read_size>0) {
			if (with_out) output("2b","00","00");
			break;
		}
		if (read_size < 0) {
			errs--;
			if (errs < 0) {
				if (with_out) output("2a","00","00");
				CFG.state = 0x00;
				return -1;
				break;
			}
		} else {
			empt--;
		}
		if (empt < 0) {
			if (with_out) output("29","00","00");
			CFG.state = 0x00;
			return -1;
			break;
		}
		usleep(500);
	}

	for (i=0;i<read_size;i++) {
		memset(singlechar,0,10);
		if (devicebuf[i] < 17 && devicebuf[i] >= 0) {
			sprintf(singlechar, "0%X", devicebuf[i]);
		} else {
			if (devicebuf[i] > 0 && devicebuf[i] < 0x80) {
				sprintf(singlechar, "%X", devicebuf[i]);
			} else {
				sprintf(singlechar, "%X", devicebuf[i]);
				singlechar[0] = singlechar[6];
				singlechar[1] = singlechar[7];
				singlechar[2] = 0;
			}
		}
		strncat(devicecode,singlechar,strlen(singlechar));

		if (i==3) {
			strncat(message,singlechar,strlen(singlechar));
			CFG.state = devicebuf[i];
		}
	}
	if (with_out) output("11",message,devicecode);
	return 0;
}


int m_reset() {
	char	escape[6]={0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x06;
	escape[3] = 0x30;
	escape[4] = 0x41;
	escape[5] = 0xB3;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 6);
		output("21","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int m_ack() {
	char	escape[6]={0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x06;
	escape[3] = 0x00;
	escape[4] = 0xC2;
	escape[5] = 0x82;

	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 6);
		//output("22","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int m_poll() {
	char	escape[6]={0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x06;
	escape[3] = 0x33;
	escape[4] = 0xDA;
	escape[5] = 0x81;

	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 6);
		//output("23","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}


int m_billtable() {
	char	escape[6]={0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x06;
	escape[3] = 0x41;
	escape[4] = 0x4F;
	escape[5] = 0xD1;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 6);
		//output("24","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int disbtypes() {
	char	escape[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x0C;
	escape[3] = 0x34;
	escape[4] = 0x00;
	escape[5] = 0x00;
	escape[6] = 0x00;
	escape[7] = 0x00;
	escape[8] = 0x00;
	escape[9] = 0x00;
	escape[10] = 0x17;
	escape[11] = 0x0C;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 12);
		//output("25","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int enbtypes() {
	char	escape[12]={0,0,0,0,0,0,0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x0C;
	escape[3] = 0x34;
	escape[4] = 0xFF;
	escape[5] = 0xFF;
	escape[6] = 0xFF;
	escape[7] = 0x00;
	escape[8] = 0x00;
	escape[9] = 0x00;
	escape[10] = 0xB5;
	escape[11] = 0xC1;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 12);
		//output("26","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int identif() {
	char	escape[6]={0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x06;
	escape[3] = 0x37;
	escape[4] = 0xFE;
	escape[5] = 0xC7;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 6);
		//output("27","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}

int setsecr() {
	char	escape[9]={0,0,0,0,0,0,0,0,0};
	int		fd;

	escape[0] = 0x02;
	escape[1] = 0x03;
	escape[2] = 0x09;
	escape[3] = 0x32;
	escape[4] = 0x00;
	escape[5] = 0x00;
	escape[6] = 0x00;
	escape[7] = 0x26;
	escape[8] = 0x1F;
	CFG.fd = open_file(CFG.device_addr);
	if (CFG.fd) {
		write_port(CFG.fd, escape, 9);
		//output("28","00","00");
	} else {
		output("82","00","00");
		return 1;
	}
	return 0;
}


int main(int argc, char** argv) {
	/* Parameters:
	 * 	1 reset
	 * 	2 ack
	 * 	3 poll
	 * 	4 btbl
	 * 	5 disbtps
	 *  6 enbtbs
	 * 	7 identif
	 * 	8 stsecr
	 * Parameters for scripts:
	 *  9 s_init
	 *  10 s_poll
	 */
	int res = 0;

	if (read_config()<0) {
		output("84","00","00");
		return -1;
	}

	if (argc == 2) {
		mode_detect(argv[1]);
		switch (CFG.mode) {
		case 1:
			// Reset [reset]
			rs232init(CFG.device_addr);
			m_reset();
			break;
		case 2:
			// Ack [ack]
			m_ack();
			break;
		case 3:
			// Poll [poll]
			m_poll();
			break;
		case 4:
			// Bill table [btbl]
			m_billtable();
			break;
		case 5:
			// Disable bill types [disbt]
			disbtypes();
			break;
		case 6:
			// Enable bill types [enbt]
			enbtypes();
			break;
		case 7:
			// Identification [id]
			res = identif();
			break;
		case 8:
			// Set security [sec]
			res = setsecr();
			break;
		case 9:
			// SCRIPT: INITIALIZE [s_init]
			rs232init(CFG.device_addr);
			m_reset();
			m_ack();
			while(1) {
				res = m_poll();
				if (!res) read_proc(0);
				m_ack();
				if (CFG.state == 0x19) {
					output("11","19","00");
					return 0;
					break;
				}
				sleep(1);
			}
			output("12","00","00");
			break;
		case 10:
			// SCRIPT: POLL [s_poll]
			res = m_poll();
			if (!res) read_proc(1);
			m_ack();
			break;
		case 11:
			// SCRIPT: OPEN [s_open]
			res = m_poll();
			if (!res) read_proc(0);
			m_ack();
			if (CFG.state = 0x19) {
				enbtypes();
				res = m_poll();
				if (!res) read_proc(1);
				m_ack();
			}
			break;
		case 12:
			// SCRIPT: CLOSE [s_close]
			disbtypes();
			res = m_poll();
			if (!res) read_proc(1);
			m_ack();
			break;
		case 13:
			// SCRIPT: simulator [s_sim]
			printf("SIMULATION:START\n");
			res = m_poll();
			if (!res) read_proc(0);
			m_ack();
			if (CFG.state != 0x19) {
				rs232init(CFG.device_addr);
				m_reset();
				m_ack();
				while(1) {
					res = m_poll();
					if (!res) read_proc(0);
					m_ack();
					if (CFG.state == 0x19) {
						break;
					}
					sleep(1);
				}
			}
			enbtypes();
			read_proc(0);
			m_ack();
			printf("SIMULATION:validator open\n");
			while (1) {
				res = m_poll();
				if (!res) {
					read_proc(1);
					printf("SIMULATION:state=%d\n",CFG.state);
					if (CFG.state == -127) {
						printf("SIMULATION:----------------------- GET MONEY!-----------------\n");
						m_ack();
						sleep(1);
					}
				} else {
					disbtypes();
					m_ack();
					printf("SIMULATION:validator closed\n");
					break;
				}
				m_ack();
				sleep(1);
			}
			printf("SIMULATION:END\n");
			break;
		default:
			output("80","00","00");
			break;
		}
	} else {
		output("83","00","00");
	}
	close(CFG.fd);
	return 0;
}
