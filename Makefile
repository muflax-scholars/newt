GAME = newt
DATE != date +%Y%m%d%H%M%S
VERSION = snapshot-$(DATE)
FULLNAME = $(GAME)-$(VERSION)
DISTDIR = dist/$(FULLNAME)

help:
	@echo "to build NetHack in your home directory: make nethack-home"
	@echo "to build Slashem in your home directory: make slashem-home"
	@echo "to build * Both  in your home directory: make home"

home: nethack-home slashem-home

nethack-home: nethack/Makefile nethack/win/newt
	export PREFIX=$$HOME/$(GAME)/nethack/ && export CHOWN=true && export CHGRP=true && mkdir -p $$PREFIX && make -C nethack -e install

slashem-home: slashem/Makefile slashem/win/newt
	export PREFIX=$$HOME/$(GAME)/slashem/ && export CHOWN=true && export CHGRP=true && mkdir -p $$PREFIX && make -C slashem -e install

nethack/Makefile:
	cd nethack && sh sys/unix/setup.sh -

nethack/win/newt:
	cd nethack/win && ln -s ../../newt/win/newt

slashem/Makefile:
	cd slashem && sh sys/unix/setup.sh -

slashem/win/newt:
	cd slashem/win && ln -s ../../newt/win/newt

clean:
	-make -C nethack clean
	-make -C slashem clean

spotless:
	-make -C nethack spotless
	-make -C slashem spotless
	-rm -rf dist

distfiles:	\
	$(DISTDIR)/$(FULLNAME)-full.tar.gz		\
	$(DISTDIR)/$(FULLNAME)-nethack.tar.gz	\
	$(DISTDIR)/$(FULLNAME)-slashem.tar.gz	\
	$(DISTDIR)/$(FULLNAME)-full.tar.bz2		\
	$(DISTDIR)/$(FULLNAME)-nethack.tar.bz2	\
	$(DISTDIR)/$(FULLNAME)-slashem.tar.bz2

$(DISTDIR)/$(FULLNAME)-full.tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvf $(FULLNAME)-full.tar.gz $(FULLNAME)
	cd $(DISTDIR); md5 $(FULLNAME)-full.tar.gz > $(FULLNAME)-full.tar.gz.md5

$(DISTDIR)/$(FULLNAME)-nethack.tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvfL $(FULLNAME)-nethack.tar.gz $(FULLNAME)/nethack
	cd $(DISTDIR); md5 $(FULLNAME)-nethack.tar.gz > $(FULLNAME)-nethack.tar.gz.md5

$(DISTDIR)/$(FULLNAME)-slashem.tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvfL $(FULLNAME)-slashem.tar.gz $(FULLNAME)/slashem
	cd $(DISTDIR); md5 $(FULLNAME)-slashem.tar.gz > $(FULLNAME)-slashem.tar.gz.md5

$(DISTDIR)/$(FULLNAME)-full.tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvf $(FULLNAME)-full.tar.bz2 $(FULLNAME)
	cd $(DISTDIR); md5 $(FULLNAME)-full.tar.bz2 > $(FULLNAME)-full.tar.bz2.md5

$(DISTDIR)/$(FULLNAME)-nethack.tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvfL $(FULLNAME)-nethack.tar.bz2 $(FULLNAME)/nethack
	cd $(DISTDIR); md5 $(FULLNAME)-nethack.tar.bz2 > $(FULLNAME)-nethack.tar.bz2.md5

$(DISTDIR)/$(FULLNAME)-slashem.tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvfL $(FULLNAME)-slashem.tar.bz2 $(FULLNAME)/slashem
	cd $(DISTDIR); md5 $(FULLNAME)-slashem.tar.bz2 > $(FULLNAME)-slashem.tar.bz2.md5

$(DISTDIR)/$(FULLNAME): $(DISTDIR)
	cp -r _darcs/current $(.TARGET)
	ln -s ../../newt/win/newt $(DISTDIR)/$(FULLNAME)/nethack/win/newt
	ln -s ../../newt/win/newt $(DISTDIR)/$(FULLNAME)/slashem/win/newt

$(DISTDIR):
	mkdir -p $(DISTDIR)

