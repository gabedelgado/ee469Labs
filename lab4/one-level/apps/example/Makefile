default:
	cd makeprocs; make
	cd hello_world; make
	cd maxvaddress; make
	cd outsidepages; make
	cd growcallstack; make
	cd bigcount; make

clean:
	cd makeprocs; make clean
	cd hello_world; make clean
	cd maxvaddress; make clean
	cd outsidepages; make clean
	cd growcallstack; make clean
	cd bigcount; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal
