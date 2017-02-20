.PHONY: test
test:
	@make --no-print-directory -C lib lib
	@cp lib/target/libmmu.so exe/lib/libmmu.so
	@make --no-print-directory -C exe test
