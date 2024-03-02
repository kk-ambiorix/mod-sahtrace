include makefile.inc

NOW = $(shell date +"%Y-%m-%d(%H:%M:%S %z)")

# Extra destination directories
PKGDIR = ./output/$(MACHINE)/pkg/

define create_changelog
	@$(ECHO) "Update changelog"
	mv CHANGELOG.md CHANGELOG.md.bak
	head -n 9 CHANGELOG.md.bak > CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	$(ECHO) "## Release $(VERSION) - $(NOW)" >> CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	$(GIT) log --pretty=format:"- %s" $$($(GIT) describe --tags | grep -v "merge" | cut -d'-' -f1)..HEAD  >> CHANGELOG.md
	$(ECHO) "" >> CHANGELOG.md
	tail -n +10 CHANGELOG.md.bak >> CHANGELOG.md
	rm CHANGELOG.md.bak
endef

# targets
all:
	$(MAKE) -C src all
	$(MAKE) -C odl all

clean:
	$(MAKE) -C src clean
	$(MAKE) -C odl clean
	$(MAKE) -C test clean

install: all
	$(INSTALL) -D -p -m 0644 output/$(MACHINE)/$(COMPONENT).so $(DEST)/usr/lib/amx/modules/$(COMPONENT).so
	$(INSTALL) -D -p -m 0644 odl/mod_sahtrace.odl $(DEST)/etc/amx/modules/mod_sahtrace.odl
	$(INSTALL) -D -p -m 0644 odl/global_trace.odl $(DEST)/etc/amx/modules/global_trace.odl

package: all
	$(INSTALL) -D -p -m 0644 output/$(MACHINE)/$(COMPONENT).so $(PKGDIR)/usr/lib/amx/modules/$(COMPONENT).so
	$(INSTALL) -D -p -m 0644 odl/mod_sahtrace.odl $(PKGDIR)/etc/amx/modules/mod_sahtrace.odl
	$(INSTALL) -D -p -m 0644 odl/global_trace.odl $(PKGDIR)/etc/amx/modules/global_trace.odl
	cd $(PKGDIR) && $(TAR) -czvf ../$(COMPONENT)-$(VERSION).tar.gz .
	cp $(PKGDIR)../$(COMPONENT)-$(VERSION).tar.gz .
	make -C packages

changelog:
	$(call create_changelog)

test:
	$(MAKE) -C test run
	$(MAKE) -C test coverage

.PHONY: all clean changelog install package test