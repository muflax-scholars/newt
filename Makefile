GAME = newt

help:
	echo "Help!"

home: nethack-home slashem-home

nethack-home: nethack/Makefile
	export PREFIX=$$HOME/$(GAME)/nethack/ && export CHOWN=true && export CHGRP=true && mkdir -p $$PREFIX && make -C nethack -e install

slashem-home: slashem/Makefile
	export PREFIX=$$HOME/$(GAME)/slashem/ && export CHOWN=true && export CHGRP=true && mkdir -p $$PREFIX && make -C slashem -e install

clean:
	make -C nethack clean
	make -C slashem clean

spotless:
	make -C nethack spotless
	make -C slashem spotless
