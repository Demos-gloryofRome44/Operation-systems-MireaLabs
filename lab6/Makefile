all: clean check_pipe check_fifo

check_pipe: pipe_program.c 
	gcc -o check_pipe pipe_program.c

producer: producer.c 
	gcc -o producer producer.c

consumer: consumer.c
	gcc -o consumer consumer.c

check_fifo: producer consumer

clean:
	rm -f check_pipe producer consumer

.PHONY: all check_pipe check_fifo producer consumer clean