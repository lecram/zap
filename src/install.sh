#! /bin/bash

BINDIR=$1
INCLUDEDIR=$2
LIBDIR=$3

sudo mkdir -p $BINDIR
sudo cp zap $BINDIR/zap
sudo mkdir -p $INCLUDEDIR
sudo mkdir -p $INCLUDEDIR/zap
sudo cp include/*.h $INCLUDEDIR/zap/
sudo mkdir -p $LIBDIR
sudo mkdir -p $LIBDIR/zap
sudo cp libzap.a $LIBDIR/zap/libzap.a
