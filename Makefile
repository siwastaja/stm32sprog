stm32sprog: stm32sprog.c
	gcc -std=gnu99 -Wall -Wextra -pedantic -g -o stm32sprog stm32sprog.c

clean:
	-rm -f stm32sprog

.PHONY: clean

