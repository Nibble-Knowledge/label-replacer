SRC=lr.c
EXE=lr4
EXTRACFLAGS=
EXTRALDFLAGS=
export SRC
export EXE
export EXTRACFLAGS
export EXTRALDFLAGS

all: fast


fast: phony
	$(MAKE) -C src fast
	cp src/lr4 .

debug: phony
	$(MAKE) -C src
	cp src/lr4 .

clean:
	$(MAKE) -C src clean
	rm -rf lr4 gmon.out

phony: 
	true
