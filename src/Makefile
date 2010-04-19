# Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
#
# This file is part of zap.
#
# zap is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# zap is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with zap.  If not, see <http://www.gnu.org/licenses/>.

-include env

CFLAGS = -Wall -Werror -O2 -I include
I = include/

objects = zap.o zerr.o ztypes.o zobject.o znone.o zbool.o zbyte.o \
          zword.o zbitarray.o zbytearray.o zwordarray.o \
		  zbignum.o zlist.o zdict.o zfunc.o ztruth.o zeval.o zgc.o zbuiltin.o

dist : $(objects)
	$(CC) -o zap $(objects)

zap.o : zap.c $(I)zeval.h $(I)zbuiltin.h $(I)ztruth.h
	$(CC) -c $(CFLAGS) zap.c

zerr.o : zerr.c $(I)zerr.h
	$(CC) -c $(CFLAGS) zerr.c

ztypes.o : ztypes.c $(I)ztypes.h
	$(CC) -c $(CFLAGS) ztypes.c

zobject.o : zobject.c $(I)zerr.h $(I)ztypes.h $(I)zobject.h $(I)znone.h \
            $(I)zbool.h $(I)zbyte.h $(I)zword.h $(I)zbitarray.h \
	        $(I)zbytearray.h $(I)zwordarray.h $(I)zbignum.h \
	        $(I)zlist.h $(I)zdict.h $(I)zfunc.h
	$(CC) -c $(CFLAGS) zobject.c

znone.o : znone.c $(I)zerr.h $(I)ztypes.h $(I)znone.h
	$(CC) -c $(CFLAGS) znone.c

zbool.o : zbool.c $(I)zerr.h $(I)ztypes.h $(I)zbool.h
	$(CC) -c $(CFLAGS) zbool.c

zbyte.o : zbyte.c $(I)zerr.h $(I)ztypes.h $(I)zbyte.h
	$(CC) -c $(CFLAGS) zbyte.c

zword.o : zword.c $(I)zerr.h $(I)ztypes.h $(I)zword.h
	$(CC) -c $(CFLAGS) zword.c

zbitarray.o : zbitarray.c $(I)zerr.h $(I)ztypes.h $(I)zbyte.h $(I)zbitarray.h
	$(CC) -c $(CFLAGS) zbitarray.c

zbytearray.o : zbytearray.c $(I)zerr.h $(I)ztypes.h $(I)zbyte.h $(I)zbytearray.h
	$(CC) -c $(CFLAGS) zbytearray.c

zwordarray.o : zwordarray.c $(I)zerr.h $(I)ztypes.h $(I)zword.h $(I)zwordarray.h
	$(CC) -c $(CFLAGS) zwordarray.c

zbignum.o : zbignum.c $(I)zerr.h $(I)ztypes.h $(I)zbyte.h $(I)zbignum.h
	$(CC) -c $(CFLAGS) zbignum.c

zlist.o : zlist.c $(I)zap.h
	$(CC) -c $(CFLAGS) zlist.c

zdict.o : zdict.c $(I)zap.h
	$(CC) -c $(CFLAGS) zdict.c

zfunc.o : zfunc.c $(I)zap.h
	$(CC) -c $(CFLAGS) zfunc.c

ztruth.o : ztruth.c $(I)zerr.h $(I)ztypes.h $(I)zobject.h $(I)znone.h \
           $(I)zbool.h $(I)zbyte.h $(I)zword.h $(I)zbitarray.h $(I)zbytearray.h \
		   $(I)zwordarray.h $(I)zbignum.h $(I)zlist.h $(I)zdict.h $(I)zfunc.h
	$(CC) -c $(CFLAGS) ztruth.c

zeval.o : zeval.c $(I)zap.h $(I)zeval.h
	$(CC) -c $(CFLAGS) zeval.c

zgc.o : zgc.c $(I)zap.h
	$(CC) -c $(CFLAGS) zgc.c

zbuiltin.o : zbuiltin.c $(I)zap.h
	$(CC) -c $(CFLAGS) zbuiltin.c


.PHONY : clean
clean :
	$(RM) $(objects)
