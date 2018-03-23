CC=mpicc

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@
hwk3make: main.c
	#module load gnu openmpi # module is a function type module
	#$(call module,load gnu openmpi)
	$(CC) -o hwk3 main.c
clean:
	rm hwk3


define module
    $(shell eval $(${LMOD_CMD} bash load gnu openmpi); [ $? = 0 ] && eval $(${LMOD_SETTARG_CMD:-:} -s sh))
endef 