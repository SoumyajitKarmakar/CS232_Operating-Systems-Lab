# include<stdio.h>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		return 0;
	}
	
	int i, f = 0;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			printf("%s ", argv[i]);
			f =  1;
		}
	}
	if (f == 1) {
		printf("%c", '\n');
	}

	return 0;
}