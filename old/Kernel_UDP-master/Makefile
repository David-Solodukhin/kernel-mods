UNAME := $(shell uname)
#https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt
ifeq ($(UNAME), Linux) #if system is linux kernel, do the following by indent. 1 tab
	obj-m += udp_client.o #defines the files to be built as modules
	obj-m += udp_server.o

	K_DIR:= kern_udp
	#this is an actual directory
	#backslashes are just so u can newline escape
	#common files is a list of common object files
	COMMON_FILES:= $(K_DIR)/kernel_udp.o $(K_DIR)/k_file.o \
								$(K_DIR)/kernel_service.o $(K_DIR)/kernel_message.o
	#since the modules are built from many files, we must define a list of obj files
	#in the following way: module_name-y 
	udp_server-y:= \
		$(COMMON_FILES) \
		$(K_DIR)/udp_server.o \
		$(K_DIR)/kserver_operations.o

	udp_client-y:= \
		$(COMMON_FILES) \
		$(K_DIR)/udp_client.o \
		$(K_DIR)/kclient_operations.o

	# ADDITIONAL_FLAG:= #add here if you want to give flags to modules
	EXTRA_CFLAGS:=  -I$(PWD)/$(K_DIR)/include -O2 # $(ADDITIONAL_FLAG)
endif

USER_FLAGS:= -I$(PWD)/user_udp/include -O2 #compilation flags for user programs(-I is for cc to
#know what header files to include and what optimization level to compile with

ccflags-y:= -std=gnu99 -Wno-declaration-after-statement

COMMON_OBJ:=user_udp.o user_message.o
COMMON_HDR:=user_udp.h user_message.h

I_DIR:= user_udp
USER_CL_OBJ:=user_client.o uclient_operations.o $(COMMON_OBJ)
USER_SERV_OBJ:=user_server.o userver_operations.o $(COMMON_OBJ)

_USER_CL_HEAD:= user_udp.h uclient_operations.h $(COMMON_HDR)
USER_CL_HEAD:= $(patsubst %,$(I_DIR)/include/%,$(_USER_CL_HEAD))

_USER_SERV_HEAD:= user_udp.h userver_operations.h $(COMMON_HDR)
USER_SERV_HEAD:= $(patsubst %,$(I_DIR)/include/%,$(_USER_CL_HEAD))

ifeq ($(UNAME), Linux)
all: #user_client user_server #i don't need the user versions of the server/client
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
else
all: user_client user_server
endif

%.o: $(I_DIR)/%.c $(USER_CL_HEAD) $(USER_SERV_HEAD)
	$(CC) $(USER_FLAGS) $(ccflags-y) -c $< -o $@

user_client: $(USER_CL_OBJ)
	$(CC) $(USER_FLAGS) -o $@ $^ #$@ is name of generated file, $^ is name of all prereqs?

user_server: $(USER_SERV_OBJ)
	$(CC) $(USER_FLAGS) -o $@ $^

ifeq ($(UNAME), Linux)
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm user_client user_server 
else
clean:
	rm user_client user_server *.o 
endif
