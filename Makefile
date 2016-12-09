all:
	cl /MD /I. *.lib ppmrw.c ezview.c /link /out:ezview.exe

