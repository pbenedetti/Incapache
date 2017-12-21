TGZ = incapache1.0.tgz
SOURCES =  incApache.h incApache_aux.c incApache_http.c incApache_main.c \
           incApache_threads.c incApache_mime.c Makefile COPYING README
HTML_404 =  404_Not_Found.html
HTML_501 =  501_Method_Not_Implemented.html
IMAGE = uncadunca.jpg
STILE = 000_stile.css
EXECUT1 = incapache
EXECUT2 = incApache_chrooted
EXECUTABLES = $(EXECUT1) $(EXECUT2)
GCEXEC1 = bin/incagc
GCEXEC2 = bin/incAgc_chrooted
GCEXECUTABS = $(GCEXEC1) $(GCEXEC2)

all : $(EXECUTABLES)

$(EXECUT1) : incApache_mime.o incApache_aux.o
	gcc -o $(EXECUT1) incApache_aux.o incApache_mime.o

$(EXECUT2) : incApache_main.o incApache_aux.o incApache_http.o incApache_threads.o
	gcc -o $(EXECUT2) -pthread incApache_aux.o incApache_main.o incApache_http.o incApache_threads.o
	chmod g+w $(EXECUT2)

incApache_http.o : incApache_http.c incApache.h
	gcc -c incApache_http.c

incApache_main.o : incApache_main.c incApache.h
	gcc -DEXECUT2=\"$(EXECUT2)\" -c incApache_main.c

incApache_aux.o : incApache_aux.c incApache.h
	gcc -c incApache_aux.c

incApache_threads.o : incApache_threads.c incApache.h
	gcc -c incApache_threads.c

incApache_mime.o : incApache_mime.c incApache.h
	gcc -DEXECUT1=\"$(EXECUT1)\" -DEXECUT2=\"$(EXECUT2)\" -c incApache_mime.c

clean :
	rm -f a.out *.o $(EXECUT1) $(EXECUT2) ../www/$(HTML_404) ../www/$(HTML_501) \
		../www/$(IMAGE) ../www/$(STILE)

install : ../bin/$(EXECUT1) ../bin/$(EXECUT2) ../www/$(HTML_404) ../www/$(HTML_501) \
		../www/$(IMAGE) ../www/$(STILE)


../bin/$(EXECUT1) : ../bin $(EXECUT1)
	mv $(EXECUT1) ../bin

../bin/$(EXECUT2) : ../bin $(EXECUT2)
	sudo mv $(EXECUT2) ../bin
	cd ../bin; sudo chown root $(EXECUT2); sudo chmod u+s $(EXECUT2)

../bin :
	mkdir ../bin

../www/$(HTML_404) : ../www $(HTML_404)
	cp $(HTML_404) ../www

../www/$(HTML_501) : ../www $(HTML_501)
	cp $(HTML_501) ../www

../www/$(IMAGE) : ../www $(IMAGE)
	cp $(IMAGE) ../www/

../www/$(STILE) : ../www $(STILE)
	cp $(STILE) ../www/

../www :
	mkdir ../www

tgz : ../$(TGZ)

../$(TGZ) : $(SOURCES) clean
	cd .. ; tar cvzf $(TGZ) src www $(GCEXEC1) $(GCEXEC2)

