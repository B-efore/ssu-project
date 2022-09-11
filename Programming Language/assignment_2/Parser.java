import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Parser {

    final int ERR = -987654321;
    static List<Character> form;

    private double number(){
        double value = ERR;

        while(form.size()!=0 && ('0' <= form.get(0) && form.get(0) <= '9')){

            if(value==ERR)
                value = form.get(0) - '0';
            else
            {
                value *= 10;
                value += (form.get(0) - '0');
            }

            form.remove(0);
        }

        return value;
    }

    private double factor(){
        double value = ERR;
        int minusCheck = 1;

        if(form.size()!=0 && form.get(0)=='-'){
            form.remove(0);
            minusCheck = -1;
        }

        if(form.size()!=0 && ('0' <= form.get(0) && form.get(0) <= '9')){
            value = number();

            if(value==ERR)
                return ERR;
        }
        else if(form.size()!=0 && form.get(0)=='('){
            form.remove(0);
            value = expr();

            if(value==ERR)
                return ERR;

            if(form.size()!=0 && form.get(0)==')')
                form.remove(0);
            else
                return ERR;
        }
        else
            return ERR;

        return (value * minusCheck);
    }

    private double term(){
        double value = factor();
        double tmp;

        if(value==ERR)
            return ERR;

        while(form.size()!=0)
        {
            if(form.get(0)!='*' && form.get(0)!='/')
                break;

            if(form.get(0)=='*'){
                form.remove(0);

                if((tmp=factor())==ERR)
                    return ERR;

                value *= tmp;
            }
            else if(form.get(0)=='/'){
                form.remove(0);

                if((tmp=factor())==ERR)
                    return ERR;

                value /= tmp;
            }
        }

        return value;
    }

    private double expr(){
        double value = term();
        double tmp;

        if(value==ERR)
            return ERR;

        while(form.size()!=0){
            if(form.get(0)!='+' && form.get(0)!='-')
                break;

            if(form.get(0)=='+'){
                form.remove(0);

                if((tmp=term())==ERR)
                    return ERR;

                value += tmp;
            }
            else if(form.get(0)=='-'){
                form.remove(0);

                if((tmp=term())==ERR)
                    return ERR;

                value -= tmp;
            }
        }

        return value;
    }

    public static void main(String[] args){

        Parser parser = new Parser();
        Scanner scanner = new Scanner(System.in);

        while(true){

            System.out.print(">> ");
            String input = scanner.nextLine();
            String newInput = input.replaceAll(" ", "");

            form = new ArrayList<>();
            for(char ch : newInput.toCharArray())
                form.add(ch);

            double solve = parser.expr();

            if(solve== parser.ERR || form.size()!=0) {
                System.out.println("syntax error!!");
                break;
            }

            if(solve%1==0)
                System.out.println((int)solve);
            else
                System.out.println(solve);
        }
    }
}