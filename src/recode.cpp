/* This file has been taken from HPT tosser.
 * Modifications: header fcommon.h removed as unnecessary, type of getctab() 
 * function changed from VOID to INT and now returns the result of processing 
 * recoding-table file: SUCCESS, WARNING or ERROR. Becouse of adding new 
 * variable of type bool which is supported by C++ and not by C, the extension 
 * of file has been renamed to "cpp". Recoding functions have got additional
 * parameter "max" - maximum number of bytes to process. Hope, it's clear? :)
 *
 * P.S. Now header recode.h has also been removed as unnecessary. I'd personally
 * prefere separate compilation, but the maintainer's always right! :)
 * Therefore types VOID, INT, CHAR and UCHAR (as in original version) have
 * been changed to usual void int, char and unsigned char.
 *
 * Eduard Dulich, 2:464/98@FidoNet.
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int	DoRecode = 0;	// 0 - don't recode, 1 - do the recoding

char intab[] = {
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"
};

char outtab[] = {
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"
};

void recodeToInternalCharset(char *string, size_t max)
{
int c;
size_t bytes_processed;

    if (string != NULL) {

	for(bytes_processed = 0; *string != '\000' && bytes_processed < max; string++)
	    {
	    c=((int)*string)&0xFF;
        *string = intab[c];
        }

    }

}

void recodeToTransportCharset(char *string, size_t max)
{
int c;
size_t bytes_processed;

    if (string != NULL) {

	for(bytes_processed = 0; *string != '\000' && bytes_processed < max; string++)
	    {
	    c=((int)*string)&0xFF;
        *string = outtab[c];
        }

    }

}


int ctoi(char *s)
{
	char *foo;
	int res = strtoul(s, &foo, 0);
	if (*foo)	/* parse error */
		return 0;
	return res;
}

int getctab (char *dest, char *charMapFileName)
{
FILE *fp;
char buf[512],*p,*q;
int in,on,count;
int line;
bool have_warnings = false;

	if ((fp=fopen(charMapFileName,"r")) == NULL)
	 {
		fprintf(stderr,"getctab: cannot open mapchan file \"%s\"\n", charMapFileName);
		return FATAL;
	 }

	count=0;	 
	line = 0;
	while (fgets((char*)buf,sizeof(buf),fp))
	{
		line++;
		p=strtok(buf," \t\n#");
		q=strtok(NULL," \t\n#");

		if (p && q)
		{
			in = ctoi(p);
			if (in > 255) {
				fprintf(stderr, "getctab: %s: line %d: char val too big\n", charMapFileName, line);
				have_warnings = true;
				break;
			}

			on=ctoi(q);
			if (in && on) 
			 if (count++ < 256) dest[in]=on; 
			 else 
			  { 
			  fprintf(stderr,"getctab: char map table \"%s\" is big\n",charMapFileName); 
			  have_warnings = true;
			  break;
			  }
		}
	}
	fclose(fp);
	
	DoRecode = 1;
	return have_warnings ? WARNING : SUCCESS;
}
