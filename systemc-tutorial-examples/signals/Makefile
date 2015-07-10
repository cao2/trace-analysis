
.PHONY: clean all trivial prodcons

all: trivial prodcons
	$(MAKE) -C prodcons

clean:
	$(MAKE) -C trivial clean
	$(MAKE) -C prodcons clean

trivial:
	$(MAKE) -C $@

prodcons:
	$(MAKE) -C $@
