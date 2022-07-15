dist:
	$(MAKE) -C src/main clean
	tar -zcf "$(CURDIR).tar.gz" src/* biojokev_rapport.pdf makefile
