TEST_DIRS := $(wildcard *_test/)

all: release

debug:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) debug -C $${dir} ; then exit 1 ; fi ; done

release:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) release -C $${dir} ; then exit 1 ; fi ; done

clean:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) clean -C $${dir} ; then exit 1 ; fi ; done

cleanall:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) cleanall -C $${dir} ; then exit 1 ; fi ; done
	$(RM) tags

.PHONY: all debug release clean cleanall
