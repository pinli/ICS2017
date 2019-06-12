#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,TK_PLUS,TK_MINUS,TK_MULTIPLY,TK_DIVIDE,TK_L_BRACKET,TK_R_BRACKET,TK_NUM,TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_NOT,TK_HEX,TK_REG,TK_OBJECT,TK_DEREF

  /* TODO: Add more token types */

};

//  TK_NOTYPE = 256, TK_EQ
static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
//  {"\\+", '+'},         // plus

  {"\\+", TK_PLUS},                                  // plus
  {"\\-", TK_MINUS},                                 // minus
  {"\\*", TK_MULTIPLY},                                      // multiply
  {"\\/", TK_DIVIDE},                                        // divide
  {"\\(", TK_L_BRACKET},                                     // left_bracket 
  {"\\)", TK_R_BRACKET},                                     // right_bracket

  {"\\&{2}",TK_AND},                                         //and
  {"\\|{2}",TK_OR},                                          //or
  {"\\!\\=",TK_NEQ},                                         //neq
  {"0[xX][0-9a-fA-F]+",TK_HEX},                              //hex
  {"\\$[a-z]{3}",TK_REG},                                    //reg
  {"\\!",TK_NOT},                                            //not

  {"-?[0-9]+", TK_NUM},                                      // number
  {"[a-zA-Z_$][a-zA-Z_$0-9]*",TK_OBJECT},                   // object

  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  char regrec[32];
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

//        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {


			case TK_PLUS:
				tokens[nr_token].type=TK_PLUS;
				break;
			case TK_MINUS:
				tokens[nr_token].type=TK_MINUS;
				break;
			case TK_MULTIPLY:
				tokens[nr_token].type=TK_MULTIPLY;
				break;
			case TK_DIVIDE:
				tokens[nr_token].type=TK_DIVIDE;
				break;
			case TK_L_BRACKET:
				tokens[nr_token].type=TK_L_BRACKET;
				break;
			case TK_R_BRACKET:
				tokens[nr_token].type=TK_R_BRACKET;
				break;
			case TK_AND:
				tokens[nr_token].type=TK_AND;
				break;
			case TK_OR:
				tokens[nr_token].type=TK_OR;
				break;
			case TK_NOT:
				tokens[nr_token].type=TK_NOT;
				break;
			case TK_NEQ:
				tokens[nr_token].type=TK_NEQ;
				break;
			case TK_EQ:
				tokens[nr_token].type=TK_EQ;
				break;
			case TK_OBJECT:
//				tokens[nr_token].type=OBJECT;
//				strncpy(tokens[nr_token].str,substr_start,substr_len);
//				sprintf(tokens[nr_token].str,"%#x",object_addr(tokens[nr_token].str));
				break;
			case TK_NUM:
				tokens[nr_token].type=TK_NUM;
				strncpy(tokens[nr_token].str,substr_start,substr_len);
				break;
			case TK_HEX:
				tokens[nr_token].type=TK_HEX;
				strncpy(tokens[nr_token].str,substr_start,substr_len);
				break;
		   	case TK_REG:
				tokens[nr_token].type=TK_REG;
				strncpy(regrec,substr_start+1,substr_len-1);
			   	if(strcmp(regrec,"eax")==0)
				{
					sprintf(tokens[nr_token].str,"%#x",cpu.gpr[0]._32);
				}
				else if(strcmp(regrec,"ecx")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[1]._32);
			   	}
				else if(strcmp(regrec,"edx")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[2]._32);
			  	}
				else if(strcmp(regrec,"ebx")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[3]._32);
				}
				else if(strcmp(regrec,"esp")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[4]._32);
				}
				else if(strcmp(regrec,"ebp")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[5]._32);
				}
				else if(strcmp(regrec,"esi")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[6]._32);
			   	}
				else if(strcmp(regrec,"edi")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.gpr[7]._32);
				}
				else if(strcmp(regrec,"eip")==0)
				{
					sprintf(tokens[nr_token].str,"%x",cpu.eip);
				}
				break;
			case TK_NOTYPE:
				printf("This is a space.\n");
				break;
			default: panic("please implement me");
		};
		if(rules[i].token_type!=TK_NOTYPE)
			nr_token++;
		break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int q)
{       
	struct {
		char op[32];
		int top;
	}st;    
	st.top=-1;
	if(tokens[p].type==TK_L_BRACKET&&tokens[q].type==TK_R_BRACKET){
		int index=p;
		while(index!=q+1){
			switch(tokens[index].type)
			{     
				case TK_L_BRACKET:
					st.top++;
					st.op[st.top]='(';
					break;
				case TK_R_BRACKET:
					if(st.top>=0&&st.op[st.top]=='('){
						st.top--;
						if(st.top==-1&&index==q){
							return true;
						}
						else if(st.top==-1&&index!=q){
							return false;
						}
					} 
					else{   
						return false;
					}
					break;
				default:
					;
			};
			index++;
		}
		if(st.top==-1) 
			return true;
		else    
			return false;
	}
	else
		return false;
}


int eval(int p,int q) {
	if(p > q)
	{
		assert(0);
	}
	else if(p == q)
	{
		if(tokens[p].type==TK_HEX||tokens[p].type==TK_REG||tokens[p].type==TK_OBJECT)
		{
			int num;
			sscanf(tokens[p].str,"%x",&num);
			return num;
		}
		else
		{
			int num;
			sscanf(tokens[p].str,"%d",&num);
		   	return num;
  		}
	}
	else if(check_parentheses(p, q) == true)
	{
		struct{
			int op[32];
			int top;
		}st;
		st.top=-1;
		int j;
		for(j=q;j>=p;j--)
		{
			if(tokens[j].type==TK_R_BRACKET)
			{
				st.top++;
				st.op[st.top]=TK_R_BRACKET;
			}
			else if(tokens[j].type==TK_L_BRACKET)
			{
				st.top--;
			}
			if(st.top==-1&&j!=p) 
			{
				int v1=eval(p,j-1);
				int v2=eval(j+1,q);
				switch(tokens[j].type)
				{
					case TK_PLUS: return v1 + v2;
					case TK_MINUS: return v1 - v2;
					case TK_MULTIPLY: return v1 * v2;
					case TK_DIVIDE: return v1 / v2;
					default:assert(0);
				};
			}
			else if(st.top==-1&&j==p)
				return eval(p + 1, q - 1);
		}
	}
	else {
		int op=-1;
		int op_type=-1;
		int j;
		bool flag=false;
		bool MDflag=false;
		int val1;
		int val2;
		if(p+1==q&&tokens[p].type==TK_MINUS&&(tokens[q].type==TK_NUM||tokens[q].type==TK_HEX))
		{
			int k=0;
			while(tokens[q].str[k]!='\0')
			{
				k++;
			}
			for(;k>=0;k--)
			{
				tokens[q].str[k+1]=tokens[q].str[k];
			}
			tokens[q].str[0]='-';
			return eval(q,q);
		}

	   	if(p+1==q&&tokens[p].type==TK_DEREF)
		{
			if(tokens[q].type==TK_NUM||tokens[q].type==TK_HEX||tokens[q].type==TK_REG||tokens[q].type==TK_OBJECT)
			{
				int number=eval(q,q);
				return vaddr_read(number,4);
			}
		}
		if(tokens[p].type==TK_DEREF)
		{
			if(check_parentheses(p+1, q) == true)
			{
				int number=eval(p+1,q);
				return vaddr_read(number,4);
			}
		}
		for(j=q;j>=p;j--)
		{
			if(flag==true)
			{
				if(tokens[j].type==TK_L_BRACKET)
				{
					flag=false;
				}
				continue;
			}
			if(tokens[j].type==TK_R_BRACKET)
			{
				flag=true;
			}
			if(tokens[j].type==TK_PLUS||tokens[j].type==TK_MINUS)
			{
				if(tokens[j].type==TK_MINUS&&j==p)
				{
					break;
				}
				else if(tokens[j].type==TK_MINUS&&(tokens[j-1].type==TK_PLUS||tokens[j-1].type==TK_MINUS))
				{
					op=j-1;
					op_type=tokens[j-1].type;
					break;
				}
				else if(tokens[j].type==TK_MINUS&&(tokens[j-1].type==TK_MULTIPLY||tokens[j-1].type==TK_DIVIDE))
				{
					if(MDflag==true)
						continue;
					op=j-1;
					op_type=tokens[j-1].type;
					MDflag=true;
				}
				else
				{
					op=j;
					op_type=tokens[j].type;
					break;
				}
			}

		  	if(tokens[j].type==TK_MULTIPLY||tokens[j].type==TK_DIVIDE)	    
			{
				if(MDflag==true)
					continue;
				op=j;
				op_type=tokens[j].type;
				MDflag=true;
			}
		}

		if(tokens[p].type==TK_DEREF)
		{
			int num1=eval(p+1,op-1);
			val1=vaddr_read(num1,4);
		}
		else
		{
			val1=eval(p,op-1);
		}
   	   	if(tokens[op+1].type==TK_DEREF)
		{
			int num2=eval(op+2,q);
			val2=vaddr_read(num2,4);
		}
		else
		{
		   	val2=eval(op+1,q);
		}
		switch(op_type) {
			case TK_PLUS: return val1 + val2;
			case TK_MINUS: return val1 - val2;
			case TK_MULTIPLY: return val1 * val2;
			case TK_DIVIDE: return val1 / val2;
			default:assert(0);
		}
   	}
	return 0;
}






uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  if(tokens[1].type==TK_EQ)
  {
	  uint32_t fir=eval(0,0);
	  uint32_t sec=eval(2,2);
	  if(fir==sec)
		  return 1;
	  else
		  return 0;
  }
  else
  {
	  int i;
	  for(i = 0; i < nr_token; i ++)
	  {
		  if(tokens[i].type==TK_MULTIPLY&&(i== 0||(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=TK_HEX&&tokens[i-1].type!=TK_R_BRACKET)))
		  {
			  tokens[i].type = TK_DEREF;
		  }
	  }
	  return eval(0,nr_token-1);

  }

}
