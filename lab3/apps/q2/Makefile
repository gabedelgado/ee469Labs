default:
	cd makeprocs; make
	cd genCO; make
	cd genS2; make
	cd react4CO; make
	cd reactS2; make
	cd reactS2O2; make

clean:
	cd makeprocs; make clean
	cd makeprocs; make clean
	cd genCO; make clean
	cd genS2; make clean
	cd react4CO; make clean
	cd reactS2; make clean
	cd reactS2O2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 2 2; ee469_fixterminal
