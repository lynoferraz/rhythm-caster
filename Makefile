
jit:
	./rivemu -print-outcard  -workspace -exec riv-jit-c rhythm_caster.c ${ARGS}

exec: rhythm_caster
	./rivemu -print-outcard  -workspace -exec ./rhythm_caster ${ARGS}

run: rhythm_caster.sqfs
	./rivemu -print-outcard rhythm_caster.sqfs ${ARGS}

build: build-executable build-sqfs

rhythm_caster: build-executable
build-executable: lint
	./rivemu -quiet -no-window -sdk -workspace -exec 'gcc rhythm_caster.c -o rhythm_caster $$(riv-opt-flags -Osize)'
	./rivemu -quiet -no-window -sdk -workspace -exec riv-strip rhythm_caster

rhythm_caster.sqfs: build-sqfs
build-sqfs:
	./rivemu -quiet -no-window -sdk -workspace -exec riv-mksqfs rhythm_caster info.json \
	 images \
	 incards \
	 rhythm_caster.sqfs -keep-as-directory -noappend

lint:
	gcc -Wall -Wextra -fanalyzer -I. -c *.c
	clang-tidy *.c -- -I.
