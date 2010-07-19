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

ifeq ($(CC), cc)
CFLAGS = -Wall -Werror -O2 -I include
else ifeq ($(CC), gcc)
CFLAGS = -Wall -Werror -O2 -I include
else ifeq ($(CC), tcc)
CFLAGS = -Wall -Werror -Wunsupported -Iinclude
endif

I = include/

objects = ztypes.o zerr.o zgc.o znone.o zbool.o zbyte.o \
          zword.o zbytearray.o zbignum.o zlist.o zdict.o \
          zfunc.o zobject.o zruntime.o zbuiltin.o zcpl_expr.o \
		  zcpl_mod.o zap.o

base = $(I)ztypes.h $(I)zerr.h $(I)zgc.h

types = $(I)znone.h $(I)zbool.h $(I)zbyte.h $(I)zword.h $(I)zbytearray.h \
        $(I)zbignum.h $(I)zlist.h $(I)zdict.h $(I)zfunc.h


dist : $(objects)
	$(CC) -o zap$(BINEXT) $(objects)


# Base.

ztypes.o : ztypes.c $(I)ztypes.h
	$(CC) -c $(CFLAGS) ztypes.c

zerr.o : zerr.c $(I)zerr.h
	$(CC) -c $(CFLAGS) zerr.c

zgc.o : zgc.c $(I)ztypes.h $(I)zerr.h $(I)zobject.h $(I)zgc.h
	$(CC) -c $(CFLAGS) zgc.c

# Types.

znone.o : znone.c $(I)ztypes.h $(I)zerr.h $(I)znone.h
	$(CC) -c $(CFLAGS) znone.c

zbool.o : zbool.c $(I)ztypes.h $(I)zerr.h $(I)zbool.h
	$(CC) -c $(CFLAGS) zbool.c

zbyte.o : zbyte.c $(I)ztypes.h $(I)zerr.h $(I)zbyte.h
	$(CC) -c $(CFLAGS) zbyte.c

zword.o : zword.c $(I)ztypes.h $(I)zerr.h $(I)zword.h
	$(CC) -c $(CFLAGS) zword.c

zbytearray.o : zbytearray.c $(I)ztypes.h $(I)zerr.h \
               $(I)zbyte.h $(I)zbytearray.h
	$(CC) -c $(CFLAGS) zbytearray.c

zbignum.o : zbignum.c $(I)ztypes.h $(I)zerr.h $(I)zbyte.h $(I)zbignum.h
	$(CC) -c $(CFLAGS) zbignum.c

zlist.o : zlist.c $(base) $(I)zlist.h $(I)zobject.h
	$(CC) -c $(CFLAGS) zlist.c

zdict.o : zdict.c $(base) $(I)zlist.h $(I)zdict.h $(I)zobject.h
	$(CC) -c $(CFLAGS) zdict.c

zfunc.o : zfunc.c $(I)ztypes.h $(I)zerr.h $(I)zlist.h $(I)zfunc.h
	$(CC) -c $(CFLAGS) zfunc.c

# High level.

zobject.o : zobject.c $(I)ztypes.h $(I)zerr.h $(types) $(I)zobject.h
	$(CC) -c $(CFLAGS) zobject.c

zruntime.o : zruntime.c $(base) $(types) $(I)zobject.h $(I)zruntime.h
	$(CC) -c $(CFLAGS) zruntime.c

zbuiltin.o : zbuiltin.c $(base) $(types) $(I)zobject.h $(I)zbuiltin.h
	$(CC) -c $(CFLAGS) zbuiltin.c

zcpl_expr.o : zcpl_expr.c $(I)ztypes.h $(I)zlist.h \
              $(I)zdict.h $(I)zruntime.h $(I)zcpl_expr.h
	$(CC) -c $(CFLAGS) zcpl_expr.c

zcpl_mod.o : zcpl_mod.c $(I)zerr.h $(I)zcpl_expr.h $(I)zcpl_mod.h
	$(CC) -c $(CFLAGS) zcpl_mod.c

# Main.

zap.o : zap.c $(I)ztypes.h $(I)zerr.h $(I)zlist.h \
	    $(I)zdict.h $(I)zobject.h $(I)zruntime.h \
		$(I)zbuiltin.h $(I)zcpl_expr.h $(I)zcpl_mod.h $(I)zap.h
	$(CC) -c $(CFLAGS) zap.c


.PHONY : clean
clean :
	$(RM) $(objects)
