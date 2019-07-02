void print_f(char *str)
{
  	int i;
	for (i = 0; i<strlen(str); i++)
	{
		push_ch((char) str[i]);
	}

}
void print_ln(char *str)
{
    char *s="\r\n";
    print_f(str);
    print_f(s);
}

void DEBUG_DATA()