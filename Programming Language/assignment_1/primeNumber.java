import java.util.Scanner;
import java.util.Arrays;

public class primeNumber {

    static int number[] = new int[30];
    static boolean p[] = new boolean[100001];

    public static int GCD(int a, int b)
    {
        int i;

        if(a > b) i = b;
        else i = a;

        while(i>0){
            if((a%i==0)&&(b%i==0)) break;
            i--;
        }

        return i;
    }

    private static void merge(int start, int mid, int end)
    {
        int tmp[] = new int[30];
        int i, j, k;

        i=start; j=mid+1; k=0;

        while(i<=mid && j<=end){
            if(number[i]<=number[j])
                tmp[k++] = number[i++];
            else
                tmp[k++] = number[j++];
        }

        while(i<=mid)
            tmp[k++] = number[i++];

        while(j<=mid)
            tmp[k++] = number[j++];

        k--;

        while(k>=0){
            number[start+k] = tmp[k];
            k--;
        }
    }

    private static void mergeSort(int start, int end)
    {
        int mid;

        if(start<end){
            mid = (start + end)/2;
            mergeSort(start, mid);
            mergeSort(mid+1, end);
            merge(start, mid, end);
        }
    }

    private static void primeNumber(int m)
    {
        p[0] = false;
        p[1] = false;

        for (int i = 2; i <= Math.sqrt(m); i++) {
            if (p[i]) {
                for (int j = i * 2; j <= m; j += i)
                    p[j] = false;
            }
        }
    }

    private static int prime(int a, int b)
    {
        int num = 0;

        while(a<=b){
            if(p[a])
                num++;

            a++;
        }

        return num;
    }

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        int gcd, tmp, max=0, i, n;

        while(true) {
            System.out.print(">> Input the number of numbers to process: ");
            n = scanner.nextInt();

            if(2<=n && n<=30)
                break;
        }

        System.out.println(">> Input the numbers to be processed:");
        for(i=0; i<n; i++) {
            tmp = scanner.nextInt();

            if(1<=tmp && tmp<=100000) {
                number[i] = tmp;

                if (tmp > max)
                    max = tmp;
            }
            else{
                i--;
                System.out.print("Error: 1<=N<=100,000\n");
                continue;
            }
        }

        long start = System.nanoTime();

        gcd = GCD(number[0], number[1]);
        for(i=2; i<n; i++)
            gcd = GCD(gcd, number[i]);

        System.out.println("GCD of input numbers is " + gcd);

        mergeSort(0, n-1);
        Arrays.fill(p, true);
        primeNumber(max);

        for(i=0; i+1<n; i++){
            if(number[i]!=number[i+1])
                System.out.println("Number of prime numbers between "
                        + number[i] + ", " + number[i+1] + ": " + prime(number[i], number[i+1]));
        }

        long end = System.nanoTime();

        System.out.println("Total execution time using JAVA is " + (double)(end-start)/1000000000 + " seconds!");
    }
}