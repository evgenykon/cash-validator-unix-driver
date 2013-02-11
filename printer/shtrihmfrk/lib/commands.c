int cmd_Beep() {
	int tmp = FALSE;
	init_cmd();
	compose_command(BEEP,30);
	tmp = send_command();
	return tmp;
}

int get_doc_number() {
	//TODO: запрос в девайс значения счетчика документов, инкрементация на 1
	return 11;
}

int cmd_Testprogon() {
	int tmp = FALSE;
	printf("cmd_Testprogon\n");
	init_cmd();
	cmd.param[0] = 1;
	cmd.paramlen = 1;
	compose_command(TEST,30);
	tmp = send_command();
	usleep(50000 * USLEEP_ADD);
	init_cmd();
	compose_command(INTERRUPT_TEST,30);
	tmp |= send_command();
	return tmp;
}

int cmd_Feeddocument(int lines) {
	int tmp = FALSE;
	printf("cmd_Feeddocument\n");
	init_cmd();
	cmd.param[0] = 2;
	cmd.param[1] = lines;
	cmd.paramlen = 2;
	compose_command(FEED_DOCUMENT,30);
	tmp = send_command();
	return tmp;
}

int cmd_Printtitle(char *title) {
	int tmp,docnum = FALSE;
	char tmpchar[200];
	memset(tmpchar,0,200);
	printf("cmd_Printtitle\n");
	init_cmd();
	utf2win1251(title, tmpchar);
	strncpy(cmd.param, tmpchar, 30);
	docnum = get_doc_number();
	memcpy(cmd.param+30, &docnum, 2);
	cmd.paramlen = 32;
	compose_command(PRINT_DOCUMENT_TITLE,30);
	tmp = send_command();
	return tmp;
}

int cmd_Cutcheck() {
	int tmp = FALSE;
	printf("cmd_Cutcheck\n");
	init_cmd();
	cmd.param[0] = 0;
	cmd.paramlen = 1;
	compose_command(CUT_CHECK,30);
	tmp = send_command();
	return tmp;
}

int cmd_Printstring(char *str) {
	int tmp = FALSE;
	char tmpchar[200];
	memset(tmpchar,0,200);
	init_cmd();
	printf("cmd_Printstring\n");
	cmd.param[0] = 2;
	utf2win1251(str, tmpchar);
	strncpy(cmd.param+1, tmpchar, 40);
	cmd.paramlen = 41;
	compose_command(PRINT_STRING,30);
	tmp = send_command();
	return tmp;
}

int cmd_Getfrstate() {
	int tmp = FALSE;
	init_cmd();
	printf("cmd_Getfrstate\n");
	compose_command(GET_SHORT_ECR_STATUS,30);
	tmp = send_command();
	fkstate.OperNumber = cmd.rbuff[2];
	fkstate.FrMode = cmd.rbuff[7];
	fkstate.FrAdvMode = cmd.rbuff[8];
	fkstate.OperInCheck = cmd.rbuff[9];
	fkstate.Battery = cmd.rbuff[10];
	fkstate.Power = cmd.rbuff[11];
	fkstate.FPErr = cmd.rbuff[12];
	fkstate.EklzErr = cmd.rbuff[13];
	usleep(1000 * USLEEP_ADD);
	return tmp;
}

int cmd_Opensmena() {
	int tmp = FALSE;
	printf("cmd_Opensmena\n");
	init_cmd();
	compose_command(OPEN_SMENA,30);
	tmp = send_command();
	return tmp;
}

int cmd_Closesmena() {
	int tmp = FALSE;
	printf("cmd_Closesmena\n");
	init_cmd();
	compose_command(PRINT_REPORT_WITH_CLEANING,30);
	tmp = send_command();
	return tmp;
}

int cmd_Opencheck() {
	int tmp = FALSE;
	printf("cmd_Opencheck\n");
	init_cmd();
	cmd.param[0] = 0;
	cmd.paramlen = 1;
	compose_command(OPEN_CHECK,30);
	tmp = send_command();
	return tmp;
}

int cmd_Closecheck(int amount, char *str) {
	int tmp = FALSE;
	int64_t	sum;
	char tmpchar[200];
	printf("cmd_Closecheck (amount:%d, string:%s)\n",amount,str);
	init_cmd();
	cmd.paramlen   = 67;
	sum = llround(amount * 100);
	memcpy(cmd.param,    &sum, 5);			// 0-4
	sum = llround(0);
	memcpy(cmd.param+5, &sum, 5);			// 5-9
	sum = llround(0);
	memcpy(cmd.param+10, &sum, 5);			//10-14
	sum = llround(0);
	memcpy(cmd.param+15, &sum, 5);			//15-19
	sum = llround(0);
	memcpy(cmd.param+20, &sum, 3);			//20-22
	cmd.param[23] = 0;				//23
	cmd.param[24] = 0;				//24
	cmd.param[25] = 0;				//25
	cmd.param[26] = 0;				//26

	utf2win1251(str, tmpchar);
	strncpy(cmd.param+27, tmpchar, 40);
	compose_command(CLOSE_CHECK,30);
	tmp = send_command();
	return tmp;
}

int cmd_Sale(int price, char *name) {
	int tmp = FALSE,i=0;
	char tmpchar[200];
	printf("cmd_Sale (price:%d,name:%s)\n",price,name);
	int quant = 1000;
	long cur_price = price * 100;
	init_cmd();
	memset(tmpchar,0,200);
	memcpy(cmd.param,    &quant, sizeof(int));
	memcpy(cmd.param+5,  &cur_price, sizeof(long));
	cmd.param[10] = 1;
	cmd.param[11] = 0;
	cmd.param[12] = 0;
	cmd.param[13] = 0;
	cmd.param[14] = 0;
	utf2win1251(name, tmpchar);
	strncpy(cmd.param+15, tmpchar, 40);
	cmd.paramlen   = 55;
	compose_command(SALE,30);
	tmp = send_command();
	return tmp;
}

int cmd_Printcliche() {
	int tmp = FALSE;
	printf("cmd_Printcliche\n");
	init_cmd();
	compose_command(PRINT_CLICHE,30);
	tmp = send_command();
	return tmp;
}

/* not supported */
int cmd_Printadvert() {
	int tmp = FALSE;
	printf("cmd_Printadvert\n");
	init_cmd();
	compose_command(PRINT_ADVERT,30);
	tmp = send_command();
	return tmp;
}

/* not supported */
int cmd_Enddoc() {
	int tmp = FALSE;
	printf("cmd_Enddoc\n");
	init_cmd();
	cmd.param[0] = 1;
	cmd.paramlen = 1;
	compose_command(END_DOC,30);
	tmp = send_command();
	return tmp;
}

int cmd_Reporttobuffer() {
	int tmp = FALSE;
	printf("cmd_Reporttobuffer\n");
	init_cmd();
	compose_command(BUF_REPORT_WITH_CLEANING,30);
	tmp = send_command();
	return tmp;
}

int cmd_Printreportsfrombuffer() {
	int tmp = FALSE;
	printf("cmd_Printreportsfrombuffer\n");
	init_cmd();
	compose_command(PRINT_REPORTS_FROM_BUF,30);
	tmp = send_command();
	return tmp;
}

int cmd_Setdevicespeed(int setspeed) {
	//02 08 14 1E 00 00 00 00 01 97 94	// 4800
	//02 08 14 1E 00 00 00 00 02 97 97	// 9600
	int tmp = FALSE, speed=0;
	printf("cmd_Setdevicespeed\n");
	init_cmd();
	cmd.param[0] = 0;
	cfg.port_speed = setspeed;
	speed = LineSpeedVal[get_index_baudrate()];
	cmd.param[1] = speed;
	cmd.param[2] = 0x97;
	cmd.paramlen = 3;
	compose_command(SET_EXCHANGE_PARAM,30);
	tmp = send_command();
	return tmp;
}

int cmd_Repeatdocument() {
	int tmp = FALSE;
	printf("cmd_Repeatdocument\n");
	init_cmd();
	compose_command(REPEAT_DOCUMENT,30);
	tmp = send_command();
	return tmp;
}
