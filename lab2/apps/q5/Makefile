default:
	cd makeprocs; make
	cd inject1; make
	cd inject2; make
	cd react1; make
	cd react2; make 
	cd react3; make

clean:
	cd makeprocs; make clean
	cd inject1; make clean
	cd inject2; make clean
	cd react1; make clean
	cd react2; make clean
	cd react3; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1 1; ee469_fixterminal
