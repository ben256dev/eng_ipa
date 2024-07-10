printf 'building... '
gcc eng_ipa.c -lncursesw -ldl -lpthread -lm -o eng_ipa.exe
./eng_ipa.exe
rm *.exe
if [ $? -eq 0 ]; then
	printf 'succeeded!\n'
fi
