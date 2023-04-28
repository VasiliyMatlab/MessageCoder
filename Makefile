BUILDPATH := build
DOCPATH   := doc
LIBPATH   := lib

all: doc

clean:
	rm -rf $(BUILDPATH) $(LIBPATH) $(DOCPATH)

doc:
	mkdir -p $(DOCPATH)
	doxygen Doxyfile
	$(MAKE) -C $(DOCPATH)/latex

.PHONY: all clean doc
