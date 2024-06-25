SRCDIR = ../../../../../../AMiRo-Apps/messagetypes/

OPT = --no-embed-preamble 

SOURCES = $(wildcard $(SRCDIR)/*.h)
HEADERS = $(patsubst $(SRCDIR)/%.h,%.py,$(SOURCES))

.PHONY: all clean

all: $(HEADERS)

%.py: $(SRCDIR)/%.h
	@ctypesgen $< $(OPT) -o $@ 2>&1 | awk '/^ERROR/ && !/cdefs.h/{print; error=1} {print} END{exit (error ? -1 : 0)}'



#clean:
#	rm -f $(PROGRAMS)
