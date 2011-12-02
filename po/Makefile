PKGNAME=eject
PREFIX=$(DESTDIR)/usr
INSTALL = install
MODIR = $(PREFIX)/share/locale
POTFILES = ../*.c ../*.h
POFILES= cs.po de.po es.po fr.po it.po ja.po ru.po sl.po sv.po tr.po zh_TW.po pt.po pt_BR.po

all:

mo:
	files='$(POFILES)'; \
	for file in $$files;do \
	  echo compiling $$LOCALENAME ; \
	  msgfmt $$file -o `echo $$file |sed 's/\.po$$/.mo/g'`; \
	done

install:
	$(MAKE) mo
	files='$(POFILES)'; \
	for file in $$files;do \
	  LOCALENAME=`echo $$file |sed 's/\.po$$//g'`; \
	  echo installing $$LOCALENAME ; \
	  MOPATH=$(MODIR)/$$LOCALENAME/LC_MESSAGES; \
	  if [ ! -d $$MOPATH ] ;then  \
	     mkdir -p $$MOPATH; \
	  fi; \
	  cp $$LOCALENAME.mo $$MOPATH/$(PKGNAME).mo; \
	done

pot:
	xgettext --keyword=_ --keyword=N_ $(POTFILES) -o $(PKGNAME).pot

merge:
	for i in *.po;do \
	   NN=`echo $$i |sed "s/\.po$$//g"` ;\
	   echo $$i $$NN ;\
	   msgmerge $$i $(PKGNAME).pot -o $$i.tmp.$$ && mv -vf $$i.tmp.$$ $$i;\
	done

clean:
	rm -fv *.mo

distclean: clean

distdir:
