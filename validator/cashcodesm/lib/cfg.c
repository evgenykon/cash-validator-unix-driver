/* --------------------------------------------------- *
 * Includer with config reader function
 * @version:	1.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 * --------------------------------------------------- */

#define CFG_FILE "/var/www/bin/validator/cashcodesm/settings.cfg"

struct config {
	char	device_addr[50];
	int 	mode;
	int		fd;
	int		state;
};

struct config CFG;

int read_config() {
	FILE 	*fd;
	char 	readed_char[2] = {0,0};

	memset(CFG.device_addr,0,50);
	fd = fopen (CFG_FILE, "r");
	if (fd) {
		while ((fread (&readed_char, 1, 1, fd)) > 0) {
			if (strncmp(readed_char,"\n", strlen("\n"))) {
				strncat(CFG.device_addr, readed_char, 1);
			}
		}
		CFG.mode = 0;
		CFG.fd = -1;
		CFG.state = 0;
		return 0;
	} else return -1;
}

int mode_detect(char * mode) {
	if (strncmp(mode, "reset", strlen("reset"))==0) {
		CFG.mode = 1;
		return 0;
	}
	if (strncmp(mode, "ack", strlen("ack"))==0) {
		CFG.mode = 2;
		return 0;
	}
	if (strncmp(mode, "poll", strlen("poll"))==0) {
		CFG.mode = 3;
		return 0;
	}
	if (strncmp(mode, "btbl", strlen("btbl"))==0) {
		CFG.mode = 4;
		return 0;
	}
	if (strncmp(mode, "disbt", strlen("disbt"))==0) {
		CFG.mode = 5;
		return 0;
	}
	if (strncmp(mode, "enbt", strlen("enbt"))==0) {
		CFG.mode = 6;
		return 0;
	}
	if (strncmp(mode, "id", strlen("id"))==0) {
		CFG.mode = 7;
		return 0;
	}
	if (strncmp(mode, "sec", strlen("sec"))==0) {
		CFG.mode = 8;
		return 0;
	}
	if (strncmp(mode, "s_init", strlen("s_init"))==0) {
		CFG.mode = 9;
		return 0;
	}
	if (strncmp(mode, "s_poll", strlen("s_poll"))==0) {
		CFG.mode = 10;
		return 0;
	}
	if (strncmp(mode, "s_open", strlen("s_open"))==0) {
		CFG.mode = 11;
		return 0;
	}
	if (strncmp(mode, "s_close", strlen("s_close"))==0) {
		CFG.mode = 12;
		return 0;
	}
	if (strncmp(mode, "s_sim", strlen("s_sim"))==0) {
		CFG.mode = 13;
		return 0;
	}
	return -1;
}

/* reverse:  переворачиваем строку s на месте */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* записываем знак */
        n = -n;          /* делаем n положительным числом */
    i = 0;
    do {       /* генерируем цифры в обратном порядке */
        s[i++] = n % 10 + '0';   /* берем следующую цифру */
    } while ((n /= 10) > 0);     /* удаляем */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}
