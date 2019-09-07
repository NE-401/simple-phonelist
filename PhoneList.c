// 簡易電話帳
// 構造体のリストとテキストファイルで管理
// ソート機能も追加してみる

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INBUF_SIZE 32

const char *filename="addresslist";

typedef enum {
	EXIT,ADD,DELETE,SEARCH,SORTNAMEASC,PRINT,DEBUG,INIT,INVALID
} Menu;

typedef enum {
	SearchName=1,SearchAddress,SearchNumber
} SearchMode;

typedef struct _addlist {
	char name[INBUF_SIZE];		// 名前
	char addr[INBUF_SIZE];		// 住所
	char numbr[INBUF_SIZE];		// 電話番号
	struct _addlist *next;
} TelephoneDir;

TelephoneDir *top=NULL;

// 文字列入力用関数
void get_str(char *s,unsigned size) {
	fgets(s,size,stdin);
	while(*s) {
		if(*s=='\n') *s='\0';
		s++;
	}
}

// 文字列中の空文字をアンダーバーに変換
void conv_spacetobar(char *s) {
	while(*s) {
		if(isspace(*s)) *s='_';
		s++;
	}
}

// メニュー選択
Menu SelectMenu(void) {
	char ch;
	char buff[INBUF_SIZE];
	do {
		printf("(1)追加 (2)削除 (3)検索 (4)名前の昇順に整頓 (5)表示 (6)デバッグ (7)初期化 (0)終了 --> ");
		get_str(buff,INBUF_SIZE-1);
		ch=buff[0];
	}while((ch<EXIT+'0'||ch>=INVALID+'0')||strlen(buff)!=1);

	return (Menu)ch-'0';
}

// 新規ノードを確保
TelephoneDir *NewNode(void) {
	return calloc(1,sizeof(TelephoneDir));
}

void RemoveNode(TelephoneDir *p) {
	free(p);
}

// どれを探すか選択
int SwitchData(void) {
	char c;
	char buff[INBUF_SIZE];
	puts("着目するデータ");
	do {
		printf("(1)名前  (2)住所  (3)電話番号 --> ");
		get_str(buff,INBUF_SIZE-1);
		c=buff[0];
	}while(c<'1'||c>'3'||strlen(buff)!=1);
	return c-'0';
}

// データがおかしいことを表示
void DataMismatch(void) {
	fprintf(stderr,"データの不整合を発見.\n");
	fprintf(stderr,"データ形式 : [名前] [住所] [電話番号]\n");
}

// ファイルからデータを読み込み
int ReadData(void) {
	FILE *fp;
	int i;
	char _dbuff[130];
	TelephoneDir Dummy,*newp,*crnt=top;
	Dummy.next=NULL;

	if((fp=fopen(filename,"r"))==NULL) {
		puts("\aファイルオープンに失敗.");
	}
	else {
		while(fgets(_dbuff,128,fp)!=NULL) {
			if(sscanf(_dbuff,"%31s %31s %31s",&Dummy.name,&Dummy.addr,&Dummy.numbr)!=3) {
				DataMismatch();
				exit(1);
			}
			if((newp=NewNode())==NULL) {
				printf("\aメモリ割り当てが出来ません.\n");
				exit(1);
			}
			else {
				if(top==NULL) {
					top=newp;
				}
				else {
					crnt->next=newp;
				}
				crnt=newp;
				*newp=Dummy;
			}
		}
	}
	fclose(fp);
	return 0;
}

// データを探索
// 見つかれば見つかったノードへのポインタを返し、無かったらNULLを返す
TelephoneDir *SearchData(const char *s,int sw) {
	TelephoneDir *crnt=top;
	switch(sw) {
		case SearchName:		// 名前で探索
		while(crnt!=NULL) {
			if(!strcmp(crnt->name,s)) return crnt;
			crnt=crnt->next;
		}
		break;
		case SearchAddress:		// 住所で検索
		while(crnt!=NULL) {
			if(!strcmp(crnt->addr,s)) return crnt;
			crnt=crnt->next;
		}
		break;
		case SearchNumber:		// 電話番号で検索
		while(crnt!=NULL) {
			if(!strcmp(crnt->numbr,s)) return crnt;
			crnt=crnt->next;
		}
		break;
	}
	return NULL;		// 見つからなかった
}

// リストを分割
TelephoneDir *DivideList(TelephoneDir *h) {
	TelephoneDir *a=h;
	TelephoneDir *b=h->next;
	TelephoneDir *p;

	if(h==NULL||h->next==NULL) return h;

	if(b) b=b->next;

	while(b) {
		a=a->next;
		b=b->next;
		if(b) b=b->next;
	}
	p=a->next;
	a->next=NULL;
	return p;
}

// 2つのリストを結合
static TelephoneDir *MergeList(TelephoneDir *a,TelephoneDir *b) {
	static TelephoneDir Dummy;
	TelephoneDir *head=&Dummy;
	TelephoneDir *p=head;

	while(a&&b) {
		if(strcmp(a->name,b->name)<0) {
			p->next=a;
			p=a;
			a=a->next;
		}
		else {
			p->next=b;
			p=b;
			b=b->next;
		}
	}
	if(b) p->next=b;
	else p->next=a;

	return head->next;
}

// 配列をソートする(マージソート)
static TelephoneDir *MergeSort(TelephoneDir *h) {
	TelephoneDir *a=h;
	TelephoneDir *b=DivideList(h);

	return MergeList(MergeSort(a),MergeSort(b));
}

void SortData(void) {
	top=MergeSort(top);
}

// ファイルにデータを追加する
int AddData(void) {
	int i;
	TelephoneDir Dummy,*crnt=top;
	Dummy.next=NULL;
	puts("データを追加");
	printf("名前 --> ");	get_str(Dummy.name,INBUF_SIZE-1);
	printf("住所 --> ");	get_str(Dummy.addr,INBUF_SIZE-1);
	printf("電話番号 --> ");	get_str(Dummy.numbr,INBUF_SIZE-1);
	conv_spacetobar(Dummy.name);
	conv_spacetobar(Dummy.addr);
	conv_spacetobar(Dummy.numbr);
	if(SearchData(Dummy.name,SearchName)==NULL&&
		SearchData(Dummy.numbr,SearchNumber)==NULL) {	// 重複していない
		TelephoneDir *newp;
		if((newp=NewNode())==NULL) {
			printf("\aメモリ割り当てが出来ません.\n");
			exit(1);
		}
		else {
			if(top==NULL) top=newp;
			else {
				while(crnt->next!=NULL) crnt=crnt->next;
				crnt->next=newp;
			}
			*newp=Dummy;
		}
	}
	else {
		fprintf(stderr,"\a名前か電話番号が重複しています.\n");
		return 1;
	}
	return 0;
}

// 指定したデータを削除(削除したいデータがなかった場合何もしない)
int DeleteData(void) {
	int i;
	int sw;
	TelephoneDir Dummy,*crnt=top,*res;
	Dummy.next=NULL;
	if(crnt==NULL) {
		puts("\aデータが無い.");
		return 1;
	}
	sw=SwitchData();
	switch(sw) {
		case SearchName:
		printf("名前 --> ");	get_str(Dummy.name,INBUF_SIZE-1);
		conv_spacetobar(Dummy.name);
		res=SearchData(Dummy.name,SearchName);
		break;
		case SearchAddress:
		printf("住所 --> ");	get_str(Dummy.addr,INBUF_SIZE-1);
		conv_spacetobar(Dummy.addr);
		res=SearchData(Dummy.addr,SearchAddress);
		break;
		case SearchNumber:
		printf("電話番号 --> ");	get_str(Dummy.numbr,INBUF_SIZE-1);
		conv_spacetobar(Dummy.numbr);
		res=SearchData(Dummy.numbr,SearchNumber);
		break;
	}
	if(res==NULL) {	// 存在しない
		printf("削除に失敗.\n");
		return 1;
	}
	else {		// 見つかった?
		TelephoneDir *prv=NULL;
		printf("データ %s %s %sを削除.\n",res->name,res->addr,res->numbr);
		while(strcmp(crnt->name,res->name)&&crnt!=NULL) {
			prv=crnt;
			crnt=crnt->next;
		}
		if(crnt==top) {
			top=res->next;
		}
		else if(crnt==NULL) {
			prv->next=NULL;
		}
		else prv->next=res->next;
		RemoveNode(res);
	}
	return 0;
}

// ファイルに変更内容を反映
void WriteData(void) {
	FILE *fp;
	TelephoneDir *crnt=top;

	if((fp=fopen(filename,"w"))==NULL) {
		puts("\aファイルをオープンできない.");
	}
	else {
		while(crnt!=NULL) {
			fprintf(fp,"%s %s %s\n",crnt->name,crnt->addr,crnt->numbr);
			crnt=crnt->next;
		}
	}
	fclose(fp);
}

// 確保されているノードを解放
void ClearList(void) {
	TelephoneDir *tmp,*crnt=top;
	while(crnt!=NULL) {
		tmp=crnt;
		crnt=crnt->next;
		RemoveNode(tmp);
	}
	top=NULL;
}

// ファイルが無い場合、新規作成
void MakeFile(void) {
	FILE *fp;
	if((fp=fopen(filename,"r"))==NULL) {
		printf("ファイル%sが存在しないので作成します.\n",filename);
		fp=fopen(filename,"w");
		if(fp==NULL) puts("\a作成に失敗.");
		fclose(fp);
	}
	else {
		printf("%sが見つかった.\n",filename);
		fclose(fp);
	}
}

// データ出力
void PrintData(void) {
	int i=0;
	TelephoneDir *crnt=top;
	puts("+---------+----------------------+----------------------+----------------------+");
	printf("| Index   | Name%16s | Address%13s | Number%14s |\n","","","");
	while(crnt!=NULL) {
		puts("+---------+----------------------+----------------------+----------------------+");
		printf("| No.%-4d | %-20s | %-20s | %-20s |\n",++i,crnt->name,crnt->addr,crnt->numbr);
		crnt=crnt->next;
	}
	puts("+---------+----------------------+----------------------+----------------------+");
}

// デバッグ用
void PrintDebug(void) {
	TelephoneDir *crnt=top;
	while(crnt!=NULL) {
		printf("0x%08p -> %s %s %s\n",crnt,crnt->name,crnt->addr,crnt->numbr);
		crnt=crnt->next;
	}
	putchar('\n');
}

int main() {
	Menu menu;
	TelephoneDir Dummy,*res;
	MakeFile();
	ReadData();
	do {
		switch(menu=SelectMenu()) {
			case ADD:
			AddData();
			break;
			case DELETE:
			DeleteData();
			break;
			case SEARCH:
			switch(SwitchData()) {
				case SearchName:
				printf("名前 --> ");	get_str(Dummy.name,INBUF_SIZE-1);
				conv_spacetobar(Dummy.name);
				res=SearchData(Dummy.name,SearchName);
				break;
				case SearchAddress:
				printf("住所 --> ");	get_str(Dummy.addr,INBUF_SIZE-1);
				conv_spacetobar(Dummy.addr);
				res=SearchData(Dummy.addr,SearchAddress);
				break;
				case SearchNumber:
				printf("電話番号 --> ");	get_str(Dummy.numbr,INBUF_SIZE-1);
				conv_spacetobar(Dummy.numbr);
				res=SearchData(Dummy.numbr,SearchNumber);
				break;
			}
			if(res==NULL) puts("\a見つからなかった.");
			else printf("%-s\t%10s\t%s\n",res->name,res->addr,res->numbr);
			break;
			case SORTNAMEASC:
			printf("SortData実行\n");
			SortData();
			break;
			case PRINT:
			PrintData();
			break;
			case DEBUG:
			PrintDebug();
			break;
			case INIT:
			ClearList();
			break;
		}
	}while(menu!=0);

	WriteData();
	ClearList();

	return 0;
}