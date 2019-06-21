#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

struct termios original;

void disableRaw() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) {
		xit("tcsetattr");
	}
}

void enableRaw() {
	if (tcgetattr(STDIN_FILENO, &original) == -1) { //read current attributes into termios struct, pass to tcsetattr()
		xit("tcsetattr");
	}
	atexit(disableRaw);
	struct termios raw = original;
	raw.c_cflag |= (CS8); //bit mask
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); //input flags
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); //local flags
	raw.c_oflag &= ~(OPOST); //output flags
	raw.c_cc[VMIN] = 0; //min # of input bytes needed before read() can return
	raw.c_cc[VTIME] = 1; //max amt of time to wait before read() returns
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { //apply new attributes to terminal
		xit("tcsetattr");
	}
}

void xit(const char* a) {
	perror(a); //looks at global errno var and prints an error message for it
	exit(1);
}

int main() {
	enableRaw();
	while (1) {
		char input = '\0';
		if (read(STDIN_FILENO, &input, 1) == -1 && errno != EAGAIN) xit("read");
		if (iscntrl(input)) {
			printf("%d\r\n", input);
		}
		else {
			printf("%d('%iinput')\r\n", input, input);
		}
		if (input == 'q') break;
	}
	return 0;
}
