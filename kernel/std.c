int ConstStrLen(const char * str)
{
    char c = str[0];
    int i;
    for (i = 1; c != '\0'; i++)
    {
        c = str[i];
    }
    return i - 1;
}
