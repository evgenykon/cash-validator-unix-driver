/* --------------------------------------------------- *
 * Includer with file system operation functions
 * @version:	1.0
 * @created:	2010-05-05
 * @develop:	effus (effus@ifsps.ru)
 * --------------------------------------------------- */

int write_file(char * resfile, char * buf, size_t size) {
	FILE * pfile;

	pfile = fopen(resfile, "wb");
	if (!pfile) return 1;
	fwrite(buf, 1, size, pfile);
	fclose(pfile);
	return 0;
}

int open_file(char * fname) {
	int filedesc;
	static int fdflags;

	filedesc = open(fname, O_RDWR | O_NONBLOCK);
	if (filedesc == -1) {
		return FALSE;
	} else {
		fcntl(filedesc, F_GETFL);
		fcntl(filedesc, F_SETFL, fdflags & ~O_NONBLOCK);
	}
	return filedesc;
}

int file_exist(char * filename) {
	struct 	stat buf;
	int 	ret;

	if ((ret = stat(filename, &buf))==0) {return 0;} else {return 1;}
}

int run_bin(char cmdline[25]) {
	return system(cmdline);
}
