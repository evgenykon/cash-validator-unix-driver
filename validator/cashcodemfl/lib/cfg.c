/* --------------------------------------------------- *
 * Includer with config reader function
 * @version:	1.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 * --------------------------------------------------- */


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
		printf("Config file loaded\n");
		return 0;
	} else {
		printf("Error open config file: [%s]\n",CFG_FILE);
		return -1;
	}
}

int mode_detect(char * mode) {
	if (strncmp(mode, PARAM_RESET, strlen(PARAM_RESET))==0) {
		CFG.mode = MODE_RESET;
		return 0;
	}
	if (strncmp(mode, PARAM_STATE, strlen(PARAM_STATE))==0) {
		CFG.mode = MODE_STATE;
		return 0;
	}
	if (strncmp(mode, PARAM_SCENDIS, strlen(PARAM_SCENDIS))==0) {
		CFG.mode = MODE_SCENDIS;
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

void int2hex(int code, char * hex) {
	hex = hextable[code];
	return;
}
