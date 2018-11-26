make distclean
find . -name \*.gcno -o -name \*.gcda | xargs rm -f
find . -name \*.lo -o -name \*.o | xargs rm -f
find . -name \*.la -o -name \*.a | xargs rm -f 
find . -name \*.so | xargs rm -f
find . -name .libs -a -type d|xargs rm -rf
rm -f libphp.la modules/* libs/*
rm -R modules/ build/ autom4te.cache/
rm -f run-tests.php mkinstalldirs missing Makefile.global ltmain.sh install-sh configure.in configure config.sub config.nice config.h.in~ config.h.in config.guess
rm -f aclocal.m4 acinclude.m4