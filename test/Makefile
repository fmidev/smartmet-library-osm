PROG = $(patsubst %.cpp,%,$(wildcard *.cpp))

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

MAINFLAGS = -Wall -W -Wno-unused-parameter $(FLAGS)

CFLAGS = -DUNIX -D_REENTRANT -O0 -g $(MAINFLAGS)

INCLUDES += -I../osm

LIBS += \
	../libsmartmet-osm.so \
	$(PREFIX_LDFLAGS) \
	-lsmartmet-macgyver

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@err=; \
	for prog in $(PROG); do \
		( ./$$prog || err="$$err $$prog" ) ; \
	done; \
	test -z "$$err"

$(PROG) : % : %.cpp ../libsmartmet-osm.so
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)
