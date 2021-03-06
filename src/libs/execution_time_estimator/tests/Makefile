#*****************************************************************************
#                      Makefile Build System for Fawkes
#                            -------------------
#   Created on Sun 22 Dec 2019 19:38:06 CET
#   Copyright (C) 2019 by Till Hofmann <hofmann@kbsg.rwth-aachen.de>
#
#*****************************************************************************
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#*****************************************************************************

BASEDIR = ../../../..
include $(BASEDIR)/etc/buildsys/config.mk
include $(BASEDIR)/etc/buildsys/gtest.mk

LIBS_test_execution_time_estimator_skill_parser = m fawkes_execution_time_estimator
OBJS_test_execution_time_estimator_skill_parser = test_skill_parser.o
OBJS_all = $(OBJS_gtest_execution_time_estimator_skill_parser)

ifeq ($(HAVE_CPP17),1)
  CFLAGS += $(CFLAGS_CPP17)
  ifeq ($(HAVE_GTEST),1)
    CFLAGS += $(CFLAGS_GTEST)
    LDFLAGS += $(LDFLAGS_GTEST)
    BINS_gtest = $(BINDIR)/test_execution_time_estimator_skill_parser
  else
    WARN_TARGETS += warning_gtest
  endif
else
  WARN_TARGETS += warning_cpp17
endif

ifeq ($(OBJSSUBMAKE),1)
test: $(WARN_TARGETS)
.PHONY: $(WARN_TARGETS)
warning_cpp17:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting execution time estimator tests$(TNORMAL) (C++17 not supported)"
warning_gtest:
	$(SILENT)echo -e "$(INDENT_PRINT)--> $(TRED)Omitting execution time estimator tests$(TNORMAL) (gtest not found)"
endif

include $(BUILDSYSDIR)/base.mk
