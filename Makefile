SUBDIRS = src
BINDIR = $(CURDIR)/bin
PHONY:= subdirs $(SUBDIRS)

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

build-dir=$(CURDIR)/src
BUILD_OUTPUT=$(CURDIR)/build/output

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

.PHONY:clean

clean:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir $@ ;\
	done
	$(RM) $(BINDIR)/*
