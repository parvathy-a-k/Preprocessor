exe:main.o comment_removal.o file_inclusion.o macro_subst.o
	cc main.o comment_removal.o file_inclusion.o macro_subst.o -o mypreprocessor
main.o: main.c header.h
	cc -c main.c
comment_removal.o :comment_removal.c header.h
	cc -c comment_removal.c
file_inclusion.o :file_inclusion.c header.h
	cc -c file_inclusion.c
macro_subst.o :macro_subst.c header.h
	cc -c macro_subst.c 

clear:
	@echo "cleaning up........"
	@rm -r *.o mypreprocessor
