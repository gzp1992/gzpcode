OBJS=main.o ftp.o t_func.o connect_url.o login_pwd.o t_cmd.o resume.o
t_ftp:$(OBJS)
	gcc $(OBJS) -o t_ftp
%.o:%.c
	gcc -c $< -o $@
clean:
	rm *.o

