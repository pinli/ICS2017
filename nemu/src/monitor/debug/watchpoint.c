#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{       
	WP *p=head;
	WP *ret;
	if(p==NULL)
	{       
		head=free_;
		free_=free_->next;
		head->next=NULL;
		ret=head;
	}
	else
	{       
		while(p->next!=NULL)
		{       
			p=p->next;
		}
		ret=free_;
		p->next=free_;
		free_=free_->next;
		p->next->next=NULL;
	}
	return ret;
}

void free_wp(int NO)
{       
	if(head!=NULL&&head->NO==NO)
	{       
		WP *p=head->next;
		strcpy(head->watch_str,"");
		head->next=free_->next;
		free_->next=head;
		head=p;
	}
	else
	{
		WP *p=head;
		while(p->next->NO!=NO)
		{
			p=p->next;
		}
		WP *p_tmp=p->next;
		strcpy(p_tmp->watch_str,"");
		p->next=p_tmp->next;
		p_tmp->next=free_->next;
		free_->next=p_tmp;
	}
}


bool check_watchpoint()
{
	bool watch_flag=false;
	if(head!=NULL)
	{
		WP *p=head;
		while(p!=NULL)
		{
			bool *success=false;
			char *strsec="$eip==";
			char *strres=strstr(p->watch_str,strsec);
			int expr_res=expr(p->watch_str,success);
			if(strres==NULL)
			{
				if(expr_res!=p->oldvalue)
				{
					watch_flag=true;
					printf("%s:the old value is:%d\n%s:the new value is:%d\n",p->watch_str,p->oldvalue,p->watch_str,expr_res);
					printf("%s:the old value is:%x\n%s:the new value is:%x\n",p->watch_str,p->oldvalue,p->watch_str,expr_res);
					p->oldvalue=expr_res;
				}
			}
			else
			{
				if(expr_res==1)
					return true;
			}
			p=p->next;
		}
	}
	return watch_flag;
}

void all_watchpoint()
{
	WP *p=head;
	while(p!=NULL)
	{
		printf("the NO is %d,the expr is:%s\n",p->NO,p->watch_str);
		p=p->next;
	}
}

