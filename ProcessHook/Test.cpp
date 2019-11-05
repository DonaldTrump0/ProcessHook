#include <stdio.h>
#include <Windows.h>

int sum(int x, int y) {
	printf("%d + %d = %d\n", x, y, x + y);
	return x + y;
}

int main() {
	while (true) {
		int x, y, z;

		scanf_s("%d %d", &x, &y);
		z = sum(x, y);

		wchar_t buf[20];
		swprintf_s(buf, L"%d", z);

		MessageBox(0, buf, L"sum", 0);
	}
	return 0;
}