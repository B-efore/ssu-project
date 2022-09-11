#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1024
#define ERR -987654321

char input[N+1];
char *form;

char* DeleteSpace(char input[]);
double expr();
double term();
double factor();
double number();
double cur;
int main(void)
{
	double value;

	while(1)
	{
		printf(">> ");

		fgets(input, N, stdin);
		form = DeleteSpace(input);

		cur = 0;
		value = expr();

		if(value==ERR || *form!='\0'){
			printf("syntax error!!\n");
			break;
		}

		if(value - (int)value == 0)
			printf("%d\n", (int)value);
		else
			printf("%.2f\n", value);
	}

	return 0;
}
char* DeleteSpace(char input[])
{
	char *tmp = (char *)malloc(sizeof(char)*(N+1));
	int len = strlen(input);
	int i, k = 0;

	for(i=0; i<len; i++)
		if(input[i] != ' ')
			tmp[k++] = input[i];

	tmp[k-1] = '\0';
	return tmp;
}
double expr()
{
	double value = term();
	double tmp;

	if(value == ERR)
		return ERR;

	while(1)
	{
		if(*form!='+' && *form!='-')
			break;

		if(*form=='+'){
			form++;

			if((tmp=term())==ERR)
				return ERR;

			value += tmp;
		}
		else if(*form=='-'){
			form++;

			if((tmp=term())==ERR)
				return ERR;

			value -= tmp;
		}
	}

	return value;
}
double term()
{
	double value = factor();
	double tmp;

	if(value == ERR)
		return ERR;

	while(1)
	{
		if(*form!='*' && *form!='/')
			break;

		if(*form =='*'){
			form++;
			
			if((tmp=factor())==ERR)
				return ERR;
			
			value *= tmp;
		}
		else if(*form =='/'){
			form++;

			if((tmp=factor())==ERR)
				return ERR;

			value /= tmp;
		}
	}

	return value;
}
double factor()
{
	double value = ERR;
	int minusCheck = 1;

	if(*form=='-'){
		form++;
		minusCheck = -1;
	}

	if('0' <= *form && *form <= '9'){
		value = number();
	}
	else if(*form=='(')
	{
		form++;
		value = expr();

		if(*form==')')
			form++;
		else
			return ERR;
	}
	else
		return ERR;

	return (value * minusCheck);
}
double number()
{
	double value = ERR;

	while('0' <= *form && *form <= '9'){
		
		if(value == ERR)
			value = *form - '0';
		else
		{
			value *= 10;
			value += (*form - '0');
		}

		form++;
	}

	return value;
}
