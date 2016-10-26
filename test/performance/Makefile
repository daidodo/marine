TEST_DIRS := $(wildcard *_test/)

all:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) all -C $${dir} ; then exit 1 ; fi ; done

debug: all

release: all

clean:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) clean -C $${dir} ; then exit 1 ; fi ; done

cleanall:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) cleanall -C $${dir} ; then exit 1 ; fi ; done
	@$(RM) tags

.PHONY: all clean cleanall
