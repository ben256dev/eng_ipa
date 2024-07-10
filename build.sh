printf 'building... '
gcc hangul.c -lncursesw -ldl -lpthread -lm -o hangul.exe

if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
