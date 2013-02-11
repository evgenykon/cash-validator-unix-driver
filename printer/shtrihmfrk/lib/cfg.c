/* ----------------------------------
 * CONFIGURATION FUCNTIONS
 * ------------------------------- */
void init_config() {
	memset(cfg.device,0,50);
	cfg.port_speed = 0;
	cfg.port_timeout = 0;
	cfg.err_timeout = 0;
	sys.cfgload = 0;
	return;
}



int load_config() {
	FILE *cfgf;
	struct 	stat buf;
	int 	ret;

	init_config();
	if ((ret = stat(CFGFILE, &buf))!=0) {
		sys.error = ERR_CFGNOTEXIST;
		return FALSE;
	}
	cfgf=fopen(CFGFILE, "rb");
	if (!cfgf) {
		sys.error = ERR_CFGBADDESCR;
		return FALSE;
	}
	fread(&cfg, sizeof (cfgstruct), 1, cfgf);
	fclose(cfgf);
	sys.cfgload = TRUE;
	return TRUE;
}



int save_config() {
	FILE *cfgf;
	cfgf = fopen(CFGFILE, "wb");
	if (cfgf) {
		fwrite(&cfg, sizeof(cfgstruct), 1, cfgf);
		fclose(cfgf);
		return TRUE;
	} else {
		sys.error = ERR_CFGBADDESCR;
		return FALSE;
	}
}



//{B2400, B4800, B9600, B19200, B38400, B57600, B115200};
int get_index_baudrate() {
	if (sys.cfgload) {
		switch (cfg.port_speed) {
		case 2400: return 0;
		case 4800: return 1;
		case 9600: return 2;
		case 19200: return 3;
		case 38400: return 4;
		case 57600: return 5;
		case 115200: return 6;
		default:
			sys.error = ERR_WRONGBOUDRATE;
			return 0;
		}
	}
	return 0;
}
