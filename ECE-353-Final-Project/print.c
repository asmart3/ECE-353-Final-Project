#include "print.h"

void put_char(char c){
	fputc((int)c,stdout);
}

void put_string(char *data){
	
	while(*data != 0x00){
		put_char(*data);
		data += sizeof(char);
	}
}
