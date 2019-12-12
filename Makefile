SUBDIRS = $(CURDIR)/src
BINDIR = $(CURDIR)/bin
PHONY:= subdirs $(SUBDIRS)

build-dir=$(CURDIR)/src
BUILD_OUTPUT=$(CURDIR)/build/output


.PHONY: subdirs $(SUBDIRS) clean install
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $(BUILD_OUTPUT) -f $@/Makefile

CC=g++
CFLAGS:=-g -I $(build-dir)/include
CPPFLAGS:=$(CFLAGS)
LIBS:=-lpthread

export CPPFLAGS
export build-dir
export BUILD_OUTPUT
export LIBS
export CC
export BINDIR

install clean snake tetris:
	@$(RM) $(BINDIR)/*
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $(BUILD_OUTPUT) -f $$dir/Makefile $@ ;\
	done

