#! gmake
include manifest.mn

#######################################################################
# (2) Include "global" configuration information. (OPTIONAL)          #
#######################################################################

include $(CORE_DEPTH)/coreconf/config.mk

#######################################################################
# (3) Include "component" configuration information. (OPTIONAL)       #
#######################################################################



#######################################################################
# (4) Include "local" platform-dependent assignments (OPTIONAL).      #
#######################################################################

include config.mk

ifeq (,$(filter-out WIN%,$(OS_TARGET)))
CSRCS	+= win32err.c
DEFINES += -DIN_LIBSSL
else
ifeq ($(OS_TARGET),OS2)
CSRCS	+= os2_err.c
else
CSRCS	+= unix_err.c
endif
endif

ifdef USE_SYSTEM_ZLIB
DEFINES += -DNSS_ENABLE_ZLIB
EXTRA_LIBS += $(ZLIB_LIBS)
endif

#######################################################################
# (5) Execute "global" rules. (OPTIONAL)                              #
#######################################################################

include $(CORE_DEPTH)/coreconf/rules.mk

#######################################################################
# (6) Execute "component" rules. (OPTIONAL)                           #
#######################################################################



#######################################################################
# (7) Execute "local" rules. (OPTIONAL).                              #
#######################################################################

export:: private_export

# indicates dependency on freebl static lib
$(SHARED_LIBRARY): $(CRYPTOLIB)
