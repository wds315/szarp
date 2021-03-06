# Makefile.am for SZARP

# Pawe� Pa�ucha <pawel@praterm.com.pl>
#
# $Id: Makefile.am 6725 2009-05-18 09:16:48Z reksio $

SUBDIRS = documentation

MAINTAINERCLEANFILES = Makefile.in 
CLEANFILES = szarp.cfg szarp_in.cfg parstart parstart.cfg 

helpcommand = @HELPCOMMAND@

RESOURCES = \
       @srcdir@/cor.vim \
       @srcdir@/definable.vim \
       @srcdir@/documentation \
       @srcdir@/dtd \
       @srcdir@/elog.vim \
       @srcdir@/libpar.vim \
       @srcdir@/meteo_template.xml \
       @srcdir@/paramd_access.conf \
       @srcdir@/parcook.vim \
       @srcdir@/sounds \
       @srcdir@/szablon.vim \
       @srcdir@/szarp.vim \
       @srcdir@/wx \
       @srcdir@/qt4 \
       @srcdir@/xsltd \
       @srcdir@/szarp_in.cfg \
       @srcdir@/parstart_TEMPLATE \
       @srcdir@/parstart_cfg_TEMPLATE \
       @srcdir@/SZARP_CFG_TEMPLATE \
       @srcdir@/ssc.desktop \
       @srcdir@/scc.desktop \
       @srcdir@/filler2.desktop \
       @srcdir@/koper.desktop

EXTRA_DIST = \
      $(RESOURCES) \
      @srcdir@/SZARP_IN_CFG_TEMPLATE \
      @srcdir@/cor.vim \
      @srcdir@/definable.vim \
      @srcdir@/elog.vim \
      @srcdir@/libpar.vim \
      @srcdir@/parcook.vim \
      @srcdir@/szablon.vim \
      @srcdir@/szarp.vim \
      @srcdir@/paramd_access.conf 

INIT_D="/etc/init.d"

@srcdir@/szarp_in.cfg: @srcdir@/SZARP_IN_CFG_TEMPLATE
	@cat @srcdir@/SZARP_IN_CFG_TEMPLATE | \
	sed s/__INSTALL_DIR__/`echo @prefix@ | sed s/'\/'/'\\\\\/'/g`/g | \
	sed s+__HELPCOMMAND__+$(helpcommand)+g \
	> @srcdir@/szarp_in.cfg
	

szarp_cfg: DATABASE_PREFIX
	@cat SZARP_CFG_TEMPLATE | \
         sed s/__INSTALL_DIR__/`echo @prefix@ | sed s/'\/'/'\\\\\/'/g`/g | \
	 sed s/__PREFIX__/`cat DATABASE_PREFIX`/g | \
	 sed s+__HELPCOMMAND__+$(helpcommand)+g \
         > szarp.cfg
	$(INSTALL) -m 644 szarp.cfg @prefix@/`cat @srcdir@/DATABASE_PREFIX`/szarp.cfg
	mkdir -p /etc/szarp/
	ln -sf @prefix@/`cat DATABASE_PREFIX`/szarp.cfg $(DESTDIR)/etc/szarp/szarp.cfg

parstart_:
	@cat @srcdir@/parstart_TEMPLATE | \
         sed s+__INSTALL_DIR__+`echo @prefix@`+g | \
	 sed s+__LOCK_DIR__+@lockdir@+g \
         > parstart
	@chmod +x parstart
	
	@( if [ ! -d $(DESTDIR)/etc/szarp ] ; then $(INSTALL) -d $(DESTDIR)/etc/szarp ; fi ) 
	@( if [ ! -f $(DESTDIR)$(INIT_D)/parstart ] ; then \
		if [ ! -e $(DESTDIR)$(INIT_D) ] ; then \
			$(INSTALL) -d $(DESTDIR)$(INIT_D); \
		fi; \
		$(INSTALL) -m 755 parstart $(DESTDIR)$(INIT_D) ; \
	elif [ -f $(DESTDIR)/etc/szarp/parstart.cfg ] ; then \
		$(INSTALL) -m 755 parstart $(DESTDIR)$(INIT_D) ; \
	fi )

parstart_cfg: parstart_ DATABASE_PREFIX
	cat parstart_cfg_TEMPLATE | \
         sed s+__INSTALL_DIR__+`@prefix@+ | \
	 sed s+__PREFIX__+`cat DATABASE_PREFIX`+g \
         > parstart.cfg
	[ ! -f $(DESTDIR)/etc/szarp/parstart.cfg ] && ( \
		$(INSTALL) -m 644 parstart.cfg $(DESTDIR)@prefix@/`cat DATABASE_PREFIX` ; \
		ln -sf @prefix@/`cat DATABASE_PREFIX`/parstart.cfg \
                             $(DESTDIR)/etc/szarp/parstart.cfg \
	)
	
if !DEB_BUILD
vim:
	pwd
	@srcdir@/../script/install_vim

else DEB_BUILD
vim: 
	cp -f @srcdir@/../script/install_vim $(DESTDIR)@prefix@/resources

endif
	
paramd_access:
	$(INSTALL) -m 644 @srcdir@/paramd_access.conf \
		$(DESTDIR)@prefix@/resources/paramd_access.conf

copyfiles: $(RESOURCES)
	@echo "Copying resources"
	@$(INSTALL) -d $(DESTDIR)@prefix@/resources
	@cp -ad $(RESOURCES) $(DESTDIR)@prefix@/resources
	ls @abs_builddir@
	@cp @srcdir@/szarp_in.cfg $(DESTDIR)@prefix@/resources

regulators_defs:
	$(INSTALL) -d $(DESTDIR)@prefix@/resources/regulators
	$(INSTALL) regulators/*xml $(DESTDIR)@prefix@/resources/regulators

dictionary:
	$(INSTALL) -d $(DESTDIR)@prefix@/resources
	$(INSTALL) dictionary.xml $(DESTDIR)@prefix@/resources

rsyslog: rsyslog.conf
	$(INSTALL) -D rsyslog.conf $(DESTDIR)/etc/rsyslog.d/szarp.conf


if !DEB_BUILD

apache2:
	mkdir -p $(DESTDIR)/etc/apache2/sites-available && \
		$(LN_S) -f @prefix@/resources/xsltd/apache2-szarp \
		$(DESTDIR)/etc/apache2/sites-available/szarp || true
	[ -x "`which a2ensite`" ] && a2ensite szarp || true;
	[ -x "`which a2enmod`" ] && { \
		a2enmod modxslt || true; \
		a2enmod expires || true; \
		a2enmod rewrite || true; \
	} || true

uninstall-apache2:
	[ -x "`which a2dissite`" ] && a2dissite szarp || true;
	[ -d $(DESTDIR)/etc/apache2/sites-available ] && \
		rm -f $(DESTDIR)/etc/apache2/sites-available/szarp || true

else DEB_BUILD

apache2:

uninstall-apache2:

endif

links:
	install -d $(DESTDIR)/usr/share/autostart;
	install -d $(DESTDIR)/etc/xdg/autostart;
	install -d $(DESTDIR)/usr/share/applications;
	ln -sf @prefix@/resources/koper.desktop $(DESTDIR)/usr/share/autostart/koper.desktop 
	ln -sf @prefix@/resources/koper.desktop $(DESTDIR)/etc/xdg/autostart/koper.desktop 
	ln -sf @prefix@/resources/scc.desktop $(DESTDIR)/usr/share/autostart/scc.desktop 
	ln -sf @prefix@/resources/scc.desktop $(DESTDIR)/etc/xdg/autostart/scc.desktop 
	ln -sf @prefix@/resources/scc.desktop $(DESTDIR)/usr/share/applications/scc.desktop
	ln -sf @prefix@/resources/filler2.desktop $(DESTDIR)/usr/share/applications/filler2.desktop


if !DEB_BUILD

install-data-local: copyfiles

else DEB_BUILD

install-data-local: copyfiles links

endif

install-data-hook: vim paramd_access parstart_ apache2 regulators_defs dictionary rsyslog

uninstall-local: uninstall-apache2
	rm -rf $(DESTDIR)@prefix@/resources/* $(DESTDIR)/etc/apache2/sites-available/szarp
        
