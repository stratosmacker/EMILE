#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PROGRAMS = emile-set-cmdline emile-first-info emile-first-tune emile-install

CFLAGS	= -Wall

all: $(PROGRAMS)

emile-set-cmdline.o: emile-set-cmdline.c emile-first.h emile-second.h emile.h

emile-first-info.o: emile-first-info.c emile-first.h emile.h

emile-first-tune.o: emile-first-tune.c emile-first.h emile.h

emile-install.o: emile-install.c emile-first.h emile-second.h emile.h

clean:
	rm -f *.o $(PROGRAMS)