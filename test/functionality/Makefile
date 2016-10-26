TEST_DIRS := $(wildcard *_test/)
TEST_SRC := $(foreach d,$(TEST_DIRS),$(wildcard $(d)*.cc) $(wildcard $(d)*.h))

all: debug

debug:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) debug -C $${dir} ; then exit 1 ; fi ; done

release:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) release -C $${dir} ; then exit 1 ; fi ; done

clean:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) clean -C $${dir} ; then exit 1 ; fi ; done

cleanall:
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) cleanall -C $${dir} ; then exit 1 ; fi ; done
	$(RM) tags

line:
	@wc -l $(TEST_SRC)

run: all
	@for dir in $(TEST_DIRS) ; do if ! $(MAKE) run -C $${dir} ; then exit 1 ; fi ; done

.PHONY: all debug release clean cleanall line run
