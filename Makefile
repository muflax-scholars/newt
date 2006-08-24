GAME = newt

help:
	echo "Help!"

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
	make -C nethack clean
	make -C slashem clean

spotless:
	make -C nethack spotless
	make -C slashem spotless
