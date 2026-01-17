make-all:
    make clean && make
    make program-dfu

make-flash:
    make program-dfu

make-clean:
    make clean && make

daisy:
    cd lib/DaisySP && make
    cd lib/libDaisy && make