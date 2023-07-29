#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int main()
{
    regex_t state;
    char test_str[512];
    const char* pattern = "^(?=.*[A-Z]).+$";

    scanf("%s", &test_str);

    regcomp(&state, pattern, REG_EXTENDED);

    int status = regexec(&state, test_str, 0, NULL, 0);
    if (status == 0)
        printf("match : %s\n", test_str);
    else
        printf("unmatch : %s\n", test_str);

    return 0;
}
