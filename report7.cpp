// ���� 7. ��ø ����ü�� �̿��Ͽ� �α��� ���α׷��� �ۼ��Ͻÿ�. (Page 231)
#include <stdio.h>
#include <string.h>
#include <regex.h>

// ����ü 1 > ID/PW ����
// ����ü 2 > �ش� ID/PW ȣ��
struct account_struct {
	char id[20] = "";
	char pw[20] = "";
};

struct Member {
	int num = 0;
	account_struct account;
};

Member member[512];

int check_pw(char* input_pw) {
	regex_t state;
	const char* pattern = "^(?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[$@$!%*#?&])[A-Za-z\d$@$!%*#?&]+";
	regcomp(&state, pattern, REG_EXTENDED);
	int status = regexec(&state, input_pw, 0, NULL, 0);

	return status;
}

void create_account(char* input_id, char* input_pw) {
	int i;
	for (i = 0; i < 512; i++) {
		if (strlen(member[i].account.id) == 0 && strlen(member[i].account.pw)==0) {
			strcpy(member[i].account.id, input_id);
			strcpy(member[i].account.pw, input_pw);
			printf("Create account success !\n");
			break;
		}
	}
}

void login_account(char* input_id, char* input_pw) {
	int i;
	printf("%s %s\n", input_id, input_pw);
	for (i = 0; i < 512; i++) {
		if (strcmp(member[i].account.id, input_id) == 0 && strcmp(member[i].account.pw, input_pw) == 0) {
			printf("Login Success !");
			break;
		}
	}
	printf("Login Failed !");
}

int main() {
	int input_num = 0;
	char input_id[20];
	char input_pw[20];

	while (true) {
		printf("1. Create an account 2. Sign in  3. Exit > ");
		scanf("%d", &input_num);
		switch (input_num) {
			case 1:
				printf("Input ID > ");
				scanf("%s", input_id);
				printf("Input PW > ");
				scanf("%s", input_pw);
				if (check_pw(input_pw) == 0) {
					printf("match");
				};
				if (strlen(input_id) > 20 || strlen(input_pw) > 20) {
					printf("ID&PW can`t over than 20 char");
					break;
				}
				if (input_id != NULL && input_pw != NULL) {
					create_account(input_id, input_pw);
				}
				break;
			case 2:
				printf("Input ID > ");
				scanf("%s", input_id);
				printf("Input PW > ");
				scanf("%s", input_pw);
				if (input_id != NULL && input_pw != NULL) {
					login_account(input_id, input_pw);
				}
				break;
			case 3:
				printf("Exit. . .");
				return 0;
			default:
				printf("Enter 1, 2, 3\n");
				break;
		}
	}
}