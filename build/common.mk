##################################################
# Copyright (C), 2010-2013, ATCOM Tech. Co., Ltd.
# Filename: common.mk
# Author: hyh <hyf@atcom.com.cn>
# Created: Wed Dec  4 09:03:11 CST 2013
##################################################

####
# Generic definitions

# Convenient variables
comma := ,
squote := '
empty :=
space := $(empty) $(empty)

###
# Name of target with a '.' as filename prefix. foo/bar.o => foo/.bar.o
dot-target = $(dir $@).$(notdir $@)

###
# The temporary file to save gcc -MD generated dependencies must not
# contain a comma
depfile = $(subst $(comma),_,$(dot-target).d)

###
# filename of target with directory and extension stripped
basetarget = $(basename $(notdir $@))

###
# Escape single quote for use in echo statements
escsq = $(subst $(squote),'\$(squote)',$1)

###
# Shorthand for $(Q)$(MAKE) -f build/build.mk obj=
# Usage:
# $(Q)$(MAKE) $(build)=dir
build := -f $(if $(BUILD_SRC),$(srctree)/)build/build.mk obj

# Prefix -I with $(srctree) if it is not an absolute path.
addtree = $(if $(filter-out -I/%,$(1)),$(patsubst -I%,-I$(srctree)/%,$(1))) $(1)

# Find all -I options and call addtree
flags = $(foreach o,$($(1)),$(if $(filter -I%,$(o)),$(call addtree,$(o)),$(o)))

# echo command.
# Short version is used, if $(quiet) equals `quiet_', otherwise full one.
echo-cmd = $(if $($(quiet)cmd_$(1)),\
    echo '  $(call escsq,$($(quiet)cmd_$(1)))$(echo-why)';)

# printing commands
cmd = @$(echo-cmd) $(cmd_$(1))

# Add $(obj)/ for paths that are not absolute
objectify = $(foreach o,$(1),$(if $(filter /%,$(o)),$(o),$(obj)/$(o)))

###
# if_changed      - execute command if any prerequisite is newer than
#                   target, or command line has changed
# if_changed_dep  - as if_changed, but uses fixdep to reveal dependencies
#                   including used config symbols
# if_changed_rule - as if_changed but execute rule instead


ifneq ($(BUILD_NOCMDDEP),1)
# Check if both arguments has same arguments. Result is empty string if equal.
# User may override this check using make KBUILD_NOCMDDEP=1
arg-check = $(strip $(filter-out $(cmd_$(1)), $(cmd_$@)) \
                    $(filter-out $(cmd_$@),   $(cmd_$(1))) )
endif

# >'< substitution is for echo to work,
# >$< substitution to preserve $ when reloading .cmd file
make-cmd = $(subst \#,\\\#,$(subst $$,$$$$,$(call escsq,$(cmd_$(1)))))

# Find any prerequisites that is newer than target or that does not exist.
# PHONY targets skipped in both cases.
any-prereq = $(filter-out $(PHONY),$?) $(filter-out $(PHONY) $(wildcard $^),$^)

# generate .cmd file
define fixdep
	echo 'cmd_$@ := $(call make-cmd,cc_o_c)' > $(dot-target).tmp; \
	echo '' >> $(dot-target).tmp; \
	sed 's,\($@\):,deps_\1 := ,' $(depfile) >> $(dot-target).tmp; \
	echo '' >> $(dot-target).tmp; \
	echo '$@: $$(deps_$@)' >> $(dot-target).tmp; \
	echo '' >> $(dot-target).tmp; \
	echo '$$(deps_$@):' >> $(dot-target).tmp; \
	rm -f $(depfile); \
	mv -f $(dot-target).tmp $(dot-target).cmd
endef

# Execute command if command has changed or prerequisite(s) are updated.
#
if_changed = $(if $(strip $(any-prereq) $(arg-check)),                       \
    @set -e;                                                             \
    $(echo-cmd) $(cmd_$(1));                                             \
    echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd)

# Execute the command and also postprocess generated .d dependencies file.
if_changed_dep = $(if $(strip $(any-prereq) $(arg-check) ),                  \
    @set -e;                                                             \
    $(echo-cmd) $(cmd_$(1));                                             \
    $(fixdep))

# Usage: $(call if_changed_rule,foo)
# Will check if $(cmd_foo) or any of the prerequisites changed,
# and if so will execute $(rule_foo).
if_changed_rule = $(if $(strip $(any-prereq) $(arg-check) ),                 \
    @set -e;                                                             \
    $(rule_$(1)))

###
# why - tell why a a target got build
#       enabled by make V=2
#       Output (listed in the order they are checked):
#          (1) - due to target is PHONY
#          (2) - due to target missing
#          (3) - due to: file1.h file2.h
#          (4) - due to command line change
#          (5) - due to missing .cmd file
#          (6) - due to target not in $(targets)
# (1) PHONY targets are always build
# (2) No target, so we better build it
# (3) Prerequisite is newer than target
# (4) The command line stored in the file named dir/.target.cmd
#     differed from actual command line. This happens when compiler
#     options changes
# (5) No dir/.target.cmd file (used to store command line)
# (6) No dir/.target.cmd file and target not listed in $(targets)
#     This is a good hint that there is a bug in the kbuild file
ifeq ($(BUILD_VERBOSE),2)
why =                                                                        \
    $(if $(filter $@, $(PHONY)),- due to target is PHONY,                    \
        $(if $(wildcard $@),                                                 \
            $(if $(strip $(any-prereq)),- due to: $(any-prereq),             \
                $(if $(arg-check),                                           \
                    $(if $(cmd_$@),- due to command line change,             \
                        $(if $(filter $@, $(targets)),                       \
                            - due to missing .cmd file,                      \
                            - due to $(notdir $@) not in $$(targets)         \
                         )                                                   \
                     )                                                       \
                 )                                                           \
             ),                                                              \
             - due to target missing                                         \
         )                                                                   \
     )

echo-why = $(call escsq, $(strip $(why)))
endif

BIN := $(srctree)/bin
LICENCE_CLIENT=$(srctree)/../third_party/licence_client
BIN_BK := $(srctree)/bin_bk
BUILD := $(srctree)/build
CONTRIB := $(srctree)/contrib
DOC := $(srctree)/doc/A11
DOCCOMMON := $(srctree)/doc/common
TEST := $(srctree)/test
UPGRADE_DIR := /usr/local/data/tmp/upgrade

ifeq ($(RELEASE_VERSION),YES)
RELEASE_DEFINE=
else
RELEASE_DEFINE=
endif


core-dirs := src 
wifi-dirs := src/wifi_cmd
mmi-dirs := src/mmi
cgi-dirs := src/web/cgi-bin
exp-dirs := src/mmi
check-dirs := test/mmi/yewu-moni
protolib-dirs := src/uart_protocol_re_ehs

NCLUDE_PATH:= -I$(srctree)/src/include


