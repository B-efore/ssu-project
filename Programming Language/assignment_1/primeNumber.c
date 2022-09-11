#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define N 30
#define M 100000
bool p[M+1];
int num[N];
int GCD(int, int);
void mergeSort(int, int);
void merge(int, int, int);
void primeNumber(int);
int prime(int, int);
int main(void)
{
	int i, n, tmp, gcd, max=0;
	clock_t start, end;

	while(1){
		printf(">> Input the number of numbers to process: ");
		scanf("%d", &n);

		if(2<=n && n<=30)
			break;
	}

	printf(">> Input the numbers to be processed:\n");
	for(i=0; i<n; i++){
		scanf("%d", &tmp);

		if(1<=tmp && tmp<=100000){
			num[i] = tmp;

			if(tmp>max)
				max = tmp;
		}
		else{
			i--;
			printf("Error: 1<=N<=100,000\n");
			continue;
		}
	}

	start = clock();

	gcd = GCD(num[0], num[1]);
	for(i=2; i<n; i++)
		gcd = GCD(gcd, num[i]);

	printf("GCD of input numbers is %d\n", gcd);
	
	mergeSort(0, n-1);
	memset(p, true, sizeof(p));
	primeNumber(max);

	for(i=0; i+1<n; i++){
		if(num[i]!=num[i+1])
	   		printf("Number of prime numbers between %d, %d: %d\n", num[i], num[i+1], prime(num[i], num[i+1]));
	}

	end = clock();

	printf("Total execution time using C is %lf seconds!\n", (double)(end-start)/CLOCKS_PER_SEC);

	return 0;
}
int GCD(int a, int b)
{
	int i;

	if(a>b) i=b;
	else i=a;

	while(i>0)
	{
		if((a%i==0)&&(b%i==0)) break;
		i--;
	}

	return i;
}
void merge(int start, int mid, int end)
{
	int tmp[N];
	int i, j, k;

	i=start; j=mid+1; k=0;

	while(i<=mid && j<=end){
		if(num[i]<=num[j])
			tmp[k++] = num[i++];
		else
			tmp[k++] = num[j++];
	}

	while(i<=mid)
		tmp[k++] = num[i++];

	while(j<=end)
		tmp[k++] = num[j++];

	k--;

	while(k>=0)
	{
		num[start+k] = tmp[k];
		k--;
	}

}
void mergeSort(int start, int end)
{
	int mid;

	if(start<end){
		mid = (start + end)/2;
		mergeSort(start, mid);
		mergeSort(mid+1, end);
		merge(start, mid, end);
	}
}
void primeNumber(int m)
{
	p[0] = false;
	p[1] = false;

	for(int i=2; i<=sqrt(m); i++){
		if(p[i]){
			for(int j=i*2; j<=m; j+=i)
				p[j] = false;
		}
	}
}
int prime(int a, int b)
{
	int num=0;

	while(a<=b){
		if(p[a])
			num++;

		a++;
	}

	return num;
}
