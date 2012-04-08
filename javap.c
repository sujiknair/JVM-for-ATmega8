#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "opcode.c"
FILE *fp;
/******************************************************************************/
/* structure declarations */
/*****************************************************************************/

typedef struct table
{
	int dat;
	struct table *next;
}table;
typedef struct line_info
{
	int start_pc;
	int line_number;
}line_info;
typedef struct local_info
{
	int start_pc;
	int length;
	int name_index;
	int descriptor_index;
	int index;
}local_info;
typedef struct exception_table_info
{
	int start_pc;
	int end_pc;
	int handler_pc;
	int catch_type;
}exception_table_info;
typedef struct sourcefile_info
{
	int sourcefile_index;
}sourcefile_info;
typedef struct constantvalue_info
{
	int constantvalue_index;
}constantvalue_info;
typedef struct exception_info
{
	int number_of_exception;
	int exception_index_table[5];
}exception_info;
typedef struct linenumbertable_info
{
	int linenumber_length;
	line_info line[10];
}linenumbertable_info;

typedef struct localvariable_info
{
	int localvariable_length;
	local_info local[10];
}localvariable_info;	
typedef struct code_info
{
	int max_stack;
	int max_locals;
	int code_length;
	int code[100];
	int exceptiontable_length;
	struct exception_table_info exception[5];
	int attribute_count;
	int attribute_name_index;
	int attribute_length;
	struct linenumbertable_info line;
	
}code_info;
typedef struct attr_info
{
	struct sourcefile_info source;
	struct constantvalue_info constantvalue;
	struct exception_info exception;
	struct linenumbertable_info linenumber;
	struct localvariable_info localvariable;
	struct code_info code;
}attr_info;

typedef struct attribute_info
{
	int attribute_name_index;
	int attribute_length;
	struct attr_info attr;
}attribute_info;
typedef struct method_info
{
	int access_flags;
	int name_index;
	int descriptor_index;
	int attribute_count;
	struct attribute_info attributes[10];
}method_info;
typedef struct field_info
{
	int access_flags;
	int name_index;
	int descriptor_index;
	int attribute_count;
	struct attribute_info attributes[10];
}field_info;
typedef struct classfile
{
	int magic;
	int minor;
	int major;
	int con_pool_count;
	struct table* cons_pool[25];
	int access_flag;
	int this_class;
	int super_class;
	int interface_count;
	int interfaces[10];
	int field_count;
	struct field_info fields[5];
	int method_count;
	struct method_info methods[5];
	int attribute_count;
	struct attribute_info attributes[3];
}classfile;

/******************************************************************************/
/*  functions */
/******************************************************************************/
int compute(int a)
{
	int val=0,i;
	for(i=0;i<a;i++)
		val+=getc(fp)*pow(16,a-i-1);
	return val;
}	

/*****************************************************************************/
table *insert(int dat,table *cons_pool)
{
	table *temp;
	if(cons_pool==NULL)
	{
	temp=malloc(sizeof(table));
	temp->dat=dat;
	temp->next=NULL;
	cons_pool=temp;
	}
	else
	cons_pool->next=insert(dat,cons_pool->next);
	return cons_pool;
}
/*****************************************************************************/

void display(table *cons_pool)
{
	table *temp;
	int tag;
	temp=cons_pool;
	printf("%X\t",tag=temp->dat);
	temp=temp->next;
	if(tag==1)
	while(temp!=NULL)
	{
		printf("%c",temp->dat);
		temp=temp->next;
	}
	else
	while(temp!=NULL)
	{
	printf("%X\t",temp->dat);
	temp=temp->next;
	}
	
	printf("\n");
}
/******************************************************************************/

table *constant_utf8(table *cons_pool)
{
	int class_nm_len=0,c;
	int X,Y,Z,j,val;
	class_nm_len=compute(2);
	
	for(j=0;j<class_nm_len;j++)
	{
		X=getc(fp);
		if(X>=0x0001&&X<=0x007f)
		val=X;
		else
		{
			Y=getc(fp);
			if(X>=0x00c0&&X<=0x00df)
			val=((X&0x1f)<<6)+(Y&0x3f);
			else
			{
			Z=getc(fp);
			val=((X&0xf)<<12)+((Y&0x3f)<<6)+(Z&0x3f);
			}
		}
		cons_pool=insert(val,cons_pool);
	}
	return cons_pool;
}
table *constant_class(table *cons_pool)
{
	cons_pool=insert(compute(2),cons_pool);
	return cons_pool;
}
table *constant_name_type(table *cons_pool)
{
	cons_pool=insert(compute(2),cons_pool);
	cons_pool=insert(compute(2),cons_pool);
	return cons_pool;
}
table *constant_f_m_im_ref(table *cons_pool)
{
	cons_pool=insert(compute(2),cons_pool);
	cons_pool=insert(compute(2),cons_pool);
	return cons_pool;
}
table *constant_string(table *cons_pool)
{
	cons_pool=insert(compute(2),cons_pool);
	return cons_pool;	
}
table * constant_integer(table *cons_pool)
{
	cons_pool=insert(compute(4),cons_pool);
	return cons_pool;
}
table *constant_pool(table *cons_pool)
{
	if(cons_pool->dat==7)
	cons_pool=constant_class(cons_pool);
	else
	if(cons_pool->dat==9)
	cons_pool=constant_f_m_im_ref(cons_pool);
	else
	if(cons_pool->dat==10)
	cons_pool=constant_f_m_im_ref(cons_pool);
	else
	if(cons_pool->dat==11)
	cons_pool=constant_f_m_im_ref(cons_pool);
	else
	if(cons_pool->dat==12)
	cons_pool=constant_name_type(cons_pool);
	else
	if(cons_pool->dat==1)
	cons_pool=constant_utf8(cons_pool);
	else
	if(cons_pool->dat==8)
	cons_pool=constant_string(cons_pool);
	else
	if(cons_pool->dat==3)
	cons_pool=constant_integer(cons_pool);

	return cons_pool;
}
/******************************************************************************/
char* str(int index,table* cons_pool[25])
{
	int i;	
	char* a;
	a=malloc(sizeof(100));
	table *temp=NULL;
	if(cons_pool[index]->dat==1)
	{
		temp=cons_pool[index]->next;
		for(i=0;temp!=NULL;i++)
		{
			a[i]=temp->dat;
			temp=temp->next;
		}
		a[i]='\0';
	return a;
	}
	return 0;
}
/******************************************************************************/
attribute_info attribute(table *cons_pool[25])
{
	int i,j;
	char* a;
	attribute_info att;
	att.attribute_name_index=compute(2);
	att.attribute_length=compute(4);
	a=str(att.attribute_name_index,cons_pool);
	if(strcmp(a,"Code")==0)
	{
		att.attr.code.max_stack=compute(2);
		att.attr.code.max_locals=compute(2);
		att.attr.code.code_length=compute(4);
		for(i=0;i<att.attr.code.code_length;i++)
			att.attr.code.code[i]=compute(1);
		att.attr.code.exceptiontable_length=compute(2);
		for(i=0;i<att.attr.code.exceptiontable_length;i++)
		{
			att.attr.code.exception[i].start_pc=compute(2);
			att.attr.code.exception[i].end_pc=compute(2);
			att.attr.code.exception[i].handler_pc=compute(2);
			att.attr.code.exception[i].catch_type=compute(2);
		}
		att.attr.code.attribute_count=compute(2);
		if(att.attr.code.attribute_count)
		{
			att.attr.code.attribute_name_index=compute(2);
			att.attr.code.attribute_length=compute(4);
			att.attr.code.line.linenumber_length=compute(2);
			for(i=0;i<att.attr.code.line.linenumber_length;i++)
			{
			   att.attr.code.line.line[i].start_pc=compute(2);
			   att.attr.code.line.line[i].line_number=compute(2);
		        }
		}
	}
	else if(strcmp(a,"SourceFile")==0)
		att.attr.source.sourcefile_index=compute(2);
	else if(strcmp(a,"ConstantValue")==0)
		att.attr.constantvalue.constantvalue_index=compute(2);
	else if(strcmp(a,"Exceptions")==0)
	{
		att.attr.exception.number_of_exception=compute(2);
		for(i=0;i<att.attr.exception.number_of_exception;i++)
			att.attr.exception.exception_index_table[i]=compute(2);
	}
	else if(strcmp(a,"LineNumberTable")==0)
	{	
		att.attr.linenumber.linenumber_length=compute(2);
		for(i=0;i<att.attr.linenumber.linenumber_length;i++)
		{
			att.attr.linenumber.line[i].start_pc=compute(2);
			att.attr.linenumber.line[i].line_number=compute(2);
		}
	}
	else if(strcmp(a,"LocalVariableTable")==0)
	{
		att.attr.localvariable.localvariable_length=compute(2);
		for(i=0;i<att.attr.localvariable.localvariable_length;i++)
		{
		    att.attr.localvariable.local[i].start_pc=compute(2);
		    att.attr.localvariable.local[i].length=compute(2);
		    att.attr.localvariable.local[i].name_index=compute(2);
		    att.attr.localvariable.local[i].descriptor_index=compute(2);
		    att.attr.localvariable.local[i].index=compute(2);
		}
	}
	return att;
	
}
/****************************************************************************/
int class_index(int index,table *cons_pool[25])
{
	table *temp;
	temp=cons_pool[index]->next;
	return temp->dat;
}
/******************************************************************************/
void field_descriptor(char* a)
{
	if(strcmp(a,"I")==0)
	printf("int ");
	else
	if(strcmp(a,"C")==0)
	printf("char ");
	else 
	if(strcmp(a,"D")==0)
	printf("double ");
	else
	if(strcmp(a,"B")==0)
	printf("byte ");
	else
	if(strcmp(a,"F")==0)
	printf("float ");
	else
	if(strcmp(a,"J")==0)
	printf("long ");
	else
	if(strcmp(a,"S")==0)
	printf("short ");
	else
	if(strcmp(a,"Z")==0)
	printf("boolean ");

}
/*****************************************************************************/
void javap(classfile cfile)
{
	int i,j,k;
	char* a,*b;
  /******************************************************************/
	printf("Compiled from  ");
	for(i=0;i<cfile.attribute_count;i++)
	{
	 a=str(cfile.attributes[i].attribute_name_index,cfile.cons_pool);
	 if(strcmp(a,"SourceFile")==0)
	b=str(cfile.attributes[i].attr.source.sourcefile_index,cfile.cons_pool);
	printf("%s\n",b);
	}
 /********************************************************************/
	printf("class %s extends ",str(class_index(cfile.this_class,cfile.cons_pool),cfile.cons_pool));
	printf("%s\n{\n",str(class_index(cfile.super_class,cfile.cons_pool),cfile.cons_pool));
 /*****************************************************************/
 for(i=0;i<cfile.field_count;i++)
 {
 	printf("\t");
 	field_descriptor(str(cfile.fields[i].descriptor_index,cfile.cons_pool));
 	printf("%s",str(cfile.fields[i].name_index,cfile.cons_pool));
	printf(";\n");
	
 }
 /*****************************************************************/
 for(i=0;i<cfile.method_count;i++)
 {
 	printf("\t");
 	if(strcmp(str(cfile.methods[i].name_index,cfile.cons_pool),"<init>")==0)
		printf("%s();\n",str(class_index(cfile.this_class,cfile.cons_pool),cfile.cons_pool));
	else
	printf("%s();\n",str(cfile.methods[i].name_index,cfile.cons_pool));
 }
 printf("}\n");
 	for(i=0;i<cfile.method_count;i++)
	{
		printf("method %s()\n",str(cfile.methods[i].name_index,cfile.cons_pool));
		for(j=0;j<cfile.methods[i].attribute_count;j++)
		{
			a=str(cfile.methods[i].attributes[j].attribute_name_index,cfile.cons_pool);
			if(strcmp(a,"Code")==0)
			{
				for(k=0;k<cfile.methods[i].attributes[j].attr.code.code_length;k++)
				{
					printf("%d: ",k);
					print_opcode(cfile.methods[i].attributes[j].attr.code.code[k]);
				}
			}
		}
	}
}
/******************************************************************************/
main(int argc,char* argv[])
{
	int k,c,i,j;
	fp=fopen(argv[1],"r");
	classfile cfile;
/****************************************************************************/
	cfile.magic=compute(4);
	cfile.minor=compute(2);
	cfile.major=compute(2);
	cfile.con_pool_count=compute(2);
	for(i=1;i<cfile.con_pool_count;i++)
	    cfile.cons_pool[i]=NULL;
	for(i=1;i<cfile.con_pool_count;i++)
	  {
	    cfile.cons_pool[i]=insert(getc(fp),cfile.cons_pool[i]);
	    cfile.cons_pool[i]=constant_pool(cfile.cons_pool[i]);
	  }
/*****************************************************************************/
	cfile.access_flag=compute(2);
/******************************************************************************/
	cfile.this_class=compute(2);
/******************************************************************************/
	cfile.super_class=compute(2);
/******************************************************************************/
	cfile.interface_count=compute(2);
	for(i=0;i<cfile.interface_count;i++)
		cfile.interfaces[i]=compute(2);
/******************************************************************************/
	cfile.field_count=compute(2);
	for(i=0;i<cfile.field_count;i++)
	{
		cfile.fields[i].access_flags=compute(2);
		cfile.fields[i].name_index=compute(2);
		cfile.fields[i].descriptor_index=compute(2);		
		cfile.fields[i].attribute_count=compute(2);
		for(j=0;j<cfile.fields[i].attribute_count;j++)
		    cfile.fields[i].attributes[j]=attribute(cfile.cons_pool);	
		
	}
/******************************************************************************/
	cfile.method_count=compute(2);
	for(i=0;i<cfile.method_count;i++)
	{
		cfile.methods[i].access_flags=compute(2);
		cfile.methods[i].name_index=compute(2);
		cfile.methods[i].descriptor_index=compute(2);
		cfile.methods[i].attribute_count=compute(2);
		for(j=0;j<cfile.methods[i].attribute_count;j++)
		   cfile.methods[i].attributes[j]=attribute(cfile.cons_pool);
		
	}
/*****************************************************************************/
	cfile.attribute_count=compute(2);
	for(j=0;j<cfile.attribute_count;j++)
			cfile.attributes[j]=attribute(cfile.cons_pool);
/******************************************************************************/
	javap(cfile);
/******************************************************************************/
	fclose(fp);
}




