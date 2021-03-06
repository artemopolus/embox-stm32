
PKG_NAME := pjproject
PKG_VER  := 2.2.1

PKG_SOURCES := http://www.pjsip.org/release/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := 6ed4bb7750c827dc1d881e209a3b62db

include $(EXTBLD_LIB)

PKG_PATCHES := pjproject.patch \
	simpleua_default_loglevel.patch \
	mutex_loglevel_increase.patch \
	kmalloc.patch \
	srtp.patch \
	sha256_error_fix.patch

ifeq ($(PJSIP_ENABLE_CXX),false)
PKG_PATCHES	+= pjsua2_disable.patch
endif

DISABLE_FEATURES := \
	l16-codec   \
	ilbc-codec  \
	speex-codec \
	speex-aec   \
	gsm-codec   \
	g722-codec  \
	g7221-codec \
	#g711-codec

# Build fix for GCC9 because of old STLport
PJSIP_BUILD_FIXUP_CFLAGS := -Wno-narrowing

PJPROJECT_ROOT := $(ROOT_DIR)/third-party/pjproject
PJSIP_CPPFLAGS := -I$(PJPROJECT_ROOT)/include $(EMBOX_DEPS_CPPFLAGS) -I$(SRC_DIR)/compat/cxx/include 
PJSIP_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include -I$(SRC_DIR)/compat/posix/include
BUILD_ROOT := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_ROOT) && ( \
		./aconfigure \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			CPPFLAGS="$(PJSIP_CPPFLAGS)" \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=/ \
			$(DISABLE_FEATURES:%=--disable-%) \
			--with-external-pa; \
		echo export CFLAGS+="$(PJSIP_CPPFLAGS) $(PJSIP_BUILD_FIXUP_CFLAGS)" > user.mak; \
		echo export CXXFLAGS+="$(EMBOX_CXXFLAGS) $(PJSIP_CPPFLAGS)" >> user.mak; \
	)
	cp ./config_site.h $(BUILD_ROOT)/pjlib/include/pj/config_site.h
	touch $@

$(BUILD) :
	cd $(BUILD_ROOT) && ( \
		$(MAKE) -j1 dep; \
		$(MAKE) -j1 MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

# XXX Since in both core_c and core_cxx cases we always have core/ folder
# just copy all includes here
# PKG_INSTALL_DIR is build/extbld/pjproject/core_c/install or
# build/extbld/pjproject/core_cxx/install
PJSIP_INCLUDE_DIR := $(PKG_INSTALL_DIR)/../../core/include
PJSIP_INSTALL_DIR := $(PKG_INSTALL_DIR)/../../core/install

$(INSTALL) :
	for f in $(BUILD_ROOT)/pjsip-apps/bin/samples/$(AUTOCONF_TARGET_TRIPLET)/*; do \
		cp $$f $(PJSIP_INSTALL_DIR)/$$(basename $$f).o; \
	done
	for f in $(BUILD_ROOT)/pjsip-apps/bin/*-$(AUTOCONF_TARGET_TRIPLET); do \
		fn=$$(basename $$f); \
		cp $$f $(PJSIP_INSTALL_DIR)/$${fn%-$(AUTOCONF_TARGET_TRIPLET)}.o; \
	done
	for f in $(BUILD_ROOT)/pjsip/lib/*-$(AUTOCONF_TARGET_TRIPLET).a \
			$(BUILD_ROOT)/pjmedia/lib/*-$(AUTOCONF_TARGET_TRIPLET).a \
			$(BUILD_ROOT)/pjlib/lib/*-$(AUTOCONF_TARGET_TRIPLET).a \
			$(BUILD_ROOT)/pjlib-util/lib/*-$(AUTOCONF_TARGET_TRIPLET).a \
			$(BUILD_ROOT)/pjnath/lib/*-$(AUTOCONF_TARGET_TRIPLET).a \
			$(BUILD_ROOT)/third_party/lib/*-$(AUTOCONF_TARGET_TRIPLET).a; do \
		fn=$$(basename $$f); \
		cp $$f $(PJSIP_INSTALL_DIR)/$${fn%-$(AUTOCONF_TARGET_TRIPLET).a}.a; \
	done
	# Copy includes
	mkdir $(PJSIP_INCLUDE_DIR)
	mkdir $(PJSIP_INCLUDE_DIR)/pjsip && cp -r $(BUILD_ROOT)/pjsip/include/* $(PJSIP_INCLUDE_DIR)/pjsip
	mkdir $(PJSIP_INCLUDE_DIR)/pjlib-util && cp -r $(BUILD_ROOT)/pjlib-util/include/* $(PJSIP_INCLUDE_DIR)/pjlib-util
	mkdir $(PJSIP_INCLUDE_DIR)/pjlib && cp -r $(BUILD_ROOT)/pjlib/include/* $(PJSIP_INCLUDE_DIR)/pjlib
	mkdir $(PJSIP_INCLUDE_DIR)/pjmedia && cp -r $(BUILD_ROOT)/pjmedia/include/* $(PJSIP_INCLUDE_DIR)/pjmedia
	mkdir $(PJSIP_INCLUDE_DIR)/pjnath && cp -r $(BUILD_ROOT)/pjnath/include/* $(PJSIP_INCLUDE_DIR)/pjnath
	touch $@
