
dest_dir = ram:AmigaInputAnywhere
prog_name = AmigaInputAnywhere

COPT=-D__USE_INLINE__

g=g++

objects_dir = objs/

objects_main = writeconfig.c \
		readconfig.c \
		cfg.c \
		prefs.c \
		joy_input.c \
		init_amigainput.c \
		startup.c

objects_in_dir = ${objects_main:%.c=${objects_dir}%.o}

all:		$(objects_in_dir)
		$(g) $(prog_name).c $(objects_in_dir) -o $(prog_name)  -wall -D__USE_INLINE__

$(objects_dir)%.o:	%.c
	$(g) $(warnings) -c -O2  $(COPT) $(inc) $(@:${objects_dir}%.o=%.c) -o $@

lha:
		makedir $(dest_dir)
		makedir $(dest_dir)/config
		copy config $(dest_dir)/config all
		copy $(prog_name) $(dest_dir)
		copy $(prog_name).info $(dest_dir)
		copy $(prog_name).txt $(dest_dir)
		copy #?.info $(dest_dir)
		lha -er a ram:$(prog_name).lha $(dest_dir) 
		delete $(dest_dir) all

clean:
		delete $(objects_dir)#?.o