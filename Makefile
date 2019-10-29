BUILDDIR = build
TARGETDIR = bin

TF_C: main.c
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)
	gcc -c -g $^ -o $(BUILDDIR)/$@.o
	gcc -W -lm $(BUILDDIR)/$@.o -o $(TARGETDIR)/$@

clean:
	rm -rf $(BUILDDIR) $(TARGETDIR)


