BINPATH   := bin
BUILDPATH := build
DOCPATH   := doc

all: doc

clean:
	rm -rf $(BINPATH) $(BUILDPATH) $(DOCPATH)

doc:
	mkdir -p $(DOCPATH)
	doxygen Doxyfile
	$(MAKE) -C $(DOCPATH)/latex

.PHONY: all clean doc
