#include<stdio.h>  
#include<string.h>

int main() {
	FILE* file;
	if (fopen_s(&file, "C:/Users/Алексей/Desktop/test_file.alexeytxt", "r+")) {
		printf_s("Error on open file!\n");
		getchar();
		return 0;
	}

	char buffer[1024] = { 0 };
	fread(buffer, sizeof(char), 1024, file);
	printf_s("% s", buffer);
	printf_s("\n");



	memset(buffer, '\0', 1024);
	strcat_s(buffer, "Samsonov_Aleks");

	fseek(file, 0, 0);
	fwrite(buffer, sizeof(char), 1024, file);

	fclose(file);

	getchar();

	return 0;


}
