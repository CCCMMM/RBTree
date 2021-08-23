#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
/*
 * 定义辅助阅读的常量
 */
const int COLOR_RED=1;
const int COLOR_BLACK=0;
const int ROTATE_LEFT=1;
const int ROTATE_RIGHT=0;
const int DATA_WIDTH=4; //标记长度2加上数据最大长度x(这里都是2位数以内，取2)
/*
 * 定义红黑树结构
 */
typedef struct node{
	int data; //所含数据
	int color; //颜色 对应COLOR_RED，COLOR_BLACK
	struct node *lchild,*rchild;//左右孩子指针
	struct node *parent; //父指针
}RBTree;
/*
 * 定义辅助树显示的结构
 */
typedef struct list{
	RBTree *t;
	struct list *next;
}L;
typedef struct head{
	L *l;
	struct head *next;
}H;


/*
 * 整数幂运算函数 a为底数 b为幂指数
 */
int mi(int a,int b){
	int r=1;
	for(int k=0;k<b;k++){
		r*=a;
	}
	return r;
}
/*
 * 节点数据比较大小函数 暂设定为整型
 */
int compare(int a ,int b){
	int tag=a-b;
	if(tag>0){
		return 1;
	}else if(tag==0){
		return 0;
	}else{
		return -1;
	}
}
/*
 * 树节点旋转，tree为树指针 x为旋转支点 type对应ROTATE_LEFT，ROTATE_RIGHT 返回修改后的树指针
 */
RBTree *rotate(RBTree *tree,RBTree *x,int type){
	RBTree *y;
	if(type==ROTATE_LEFT){
		//左旋
		y=x->rchild;
		x->rchild=y->lchild;
		if(y->lchild){
			y->lchild->parent=x;
		}
		y->parent=x->parent;
		if(!y->parent){
			tree=y;
		}else if(x==x->parent->lchild){
			x->parent->lchild=y;
		}else{
			x->parent->rchild=y;
		}
		y->lchild=x;
		x->parent=y;
	}else{
		//右旋
		y=x->lchild;
		x->lchild=y->rchild;
		if(x->lchild){
			x->lchild->parent=x;
		}
		y->parent=x->parent;
		if(!y->parent){
			tree=y;
		}else if(x==x->parent->lchild){
			x->parent->lchild=y;
		}else{
			x->parent->rchild=y;
		}
		y->rchild=x;
		x->parent=y;
	}
	return tree;
}
/*
 * 根据数据遍历查找树，tree为树指针 data为查找的数据 如果查找到该节点，则返回该节点的指针，如果没有找到，则返回父节点的指针
 */
RBTree *check(RBTree *tree,int data){
	RBTree *now=tree,*parent=NULL;
	while(now){
		parent=now;
		switch(compare(now->data,data)){
			case 1:
				now=now->lchild;
				break;
			case -1:
				now=now->rchild;
				break;
			default:
				return now;
		}
	}
	return parent;
}
/*
 * 查找替代节点，这里选取右子树的最左节点，right为右子树指针 返回找到节点的指针
 */
RBTree *successor(RBTree *right){
	//寻找最左节点
	if(right){
		while(right->lchild){
			right=right->lchild;
		}
	}
	return right;
}
/*
 * 释放某个根节点，node为节点指针
 */
void freeRBTreeNode(RBTree *node){
	if(node->parent->lchild==node){
		node->parent->lchild=NULL;
	}else{
		node->parent->rchild=NULL;
	}
	free(node);
}
/*
 * 格式化打印红黑树节点
 */
void showNode(RBTree *tree){
	//根据整体长度定义2个缓存字符串buf;
	char str[DATA_WIDTH+1],re[DATA_WIDTH+1];
	sprintf(str, "%d", tree->data);
	int indention=(DATA_WIDTH-2-strlen(str))/2;
	if(tree->color==COLOR_RED){
		sprintf(re,"%*s}",indention,"");
	}else{
		sprintf(re,"%*s]",indention,"");
	}
	strcat(str,re);
	if(tree->color==COLOR_RED){
		sprintf(re,"{%*s",DATA_WIDTH-1,str);
	}else{
		sprintf(re,"[%*s",DATA_WIDTH-1,str);
	}
	printf("%s",re);
}
/*
 * 释放辅助展示结构的指针,h为主链指针
 */
void freeHl(H *h){
	//定义释放结构锁需要的辅助指针H和L
	H *dh;
	L *dl;
	while(h){
		while(h->l){
			dl=h->l;
			h->l=dl->next;
			free(dl);
		}
		dh=h;
		h=h->next;
		free(dh);
	}
}

/*
 * 插入节点后的平衡，tree为树指针 x为开始平衡的节点 返回修改后的树指针
 */
RBTree *addBalance(RBTree *tree,RBTree *x){
	RBTree *uncle;
	int flag;
	while(x->parent&&x->parent->color==COLOR_RED){
		flag=0;
		if(x->parent==x->parent->parent->lchild){
			uncle=x->parent->parent->rchild;
			flag=1;
		}else{
			uncle=x->parent->parent->lchild;
		}
		if(uncle&&uncle->color==COLOR_RED){
			//情况1 叔红，标记爷红，父叔黑，节点切换为爷
			uncle->parent->color=COLOR_RED;
			uncle->color=COLOR_BLACK;
			x->parent->color=COLOR_BLACK;
			x=uncle->parent;
		}else{
			if(flag){
				//叔叔节点是右节点，近叔子为右子，远叔子为左子，内旋为右旋，外旋为左旋
				if(x==x->parent->rchild){
					//情况2 其为近叔子且叔黑，节点切换为父节点后外旋，转为情况3
					x=x->parent;
					tree=rotate(tree,x,ROTATE_LEFT);
				}else{
					//情况3 其为远叔子且叔黑，标记爷红父黑，爷节点内旋，结束循环
					x->parent->color=COLOR_BLACK;
					x->parent->parent->color=COLOR_RED;
					tree=rotate(tree,x->parent->parent,ROTATE_RIGHT);
					break;
				}
			}else{
				//叔叔节点是左节点，近叔子为左子，远叔子为右子，内旋为左旋，外旋为右旋
				if(x==x->parent->lchild){
					//情况2 其为近叔子且叔黑，节点切换为父节点后外旋，转为情况3
					x=x->parent;
					tree=rotate(tree,x,ROTATE_RIGHT);
				}else{
					//情况3 其为远叔子且叔黑，标记爷红父黑，爷节点内旋，结束循环
					x->parent->color=COLOR_BLACK;
					x->parent->parent->color=COLOR_RED;
					tree=rotate(tree,x->parent->parent,ROTATE_LEFT);
					break;
				}
			}
		}
	}
	tree->color=COLOR_BLACK;
	return tree;
}
/*
 * 删除节点后的平衡，tree为树指针 x为开始平衡的节点 返回修改后的树指针
 */
RBTree *delBalance(RBTree *tree,RBTree *x){
	//情况1,黑节点被删补红，则直接变黑返回
	if(x->color==COLOR_RED){
		x->color=COLOR_BLACK;
		return tree;
	}
	RBTree *brother=NULL,*del=x;
	while(x!=tree){
		//取得兄弟节点
		if(x==x->parent->lchild){
			brother=x->parent->rchild;
			//x为左子，则内旋指的左旋，外旋指右旋，兄近子指的兄左子，兄远子指的兄右子
			if(brother->color==COLOR_BLACK){
				if(brother->rchild&&brother->rchild->color==COLOR_RED){
					//情况2 兄黑，兄远子红，标记兄远子黑，交换父兄色，父节点内旋，结束
					brother->color=x->parent->color;
					x->parent->color=COLOR_BLACK;
					brother->rchild->color=COLOR_BLACK;
					tree=rotate(tree,x->parent,ROTATE_LEFT);
					break;
				}else if(brother->lchild&&brother->lchild->color==COLOR_RED){
					//情况3 兄黑，兄近子红，兄远子黑，标记兄红，兄近子黑，兄节点外旋，转为情况2
					brother->color=COLOR_RED;
					brother->lchild->color=COLOR_BLACK;
					tree=rotate(tree,brother,ROTATE_RIGHT);
				}else{
					if(x->parent->color==COLOR_BLACK){
						//情况4 兄黑，兄子全黑，父黑，标记兄红，节点切换为父节点，向上迭代
						brother->color=COLOR_RED;
						x=x->parent;
					}else{
						//情况5 兄黑，兄子全黑，父红，标记兄红父黑，结束
						brother->color=COLOR_RED;
						x->parent->color=COLOR_BLACK;
						break;
					}
				}
			}else{
				//情况6 兄红，标记父红兄黑，父节点内旋，转为情况2或3或5
				brother->color=COLOR_BLACK;
				x->parent->color=COLOR_RED;
				tree=rotate(tree,x->parent,ROTATE_LEFT);
			}
			/* 更加细分的情况 2-9	
			if(brother->color==COLOR_BLACK){
				if(brother->lchild&&brother->lchild->color==COLOR_RED&&brother->rchild&&brother->rchild->color==COLOR_RED){
					if(x->parent->color==COLOR_BLACK){
						//情况2 兄黑，兄子全红，父黑，标记兄远子黑，父节点内旋，结束循环;
						brother->rchild->color=COLOR_BLACK;
						tree=rotate(tree,x->parent,ROTATE_LEFT);
						break;
					}else{
						//情况3 兄黑，兄子全红，父红，标记父黑兄红兄远子黑，父节点内旋，结束循环;
						brother->color=COLOR_RED;
						brother->rchild->color=COLOR_BLACK;
						x->parent->color=COLOR_BLACK;
						tree=rotate(tree,x->parent,ROTATE_LEFT);
						break;
					}
				}else if(brother->lchild&&brother->lchild->color==COLOR_RED){
					//情况4 兄黑，兄近子红，标记兄红，兄近子黑，兄节点外旋,转为情况5或6
					brother->color=COLOR_RED;
					brother->lchild->color=COLOR_BLACK;
					tree=rotate(tree,brother,ROTATE_RIGHT);
				}else if(brother->rchild&&brother->rchild->color==COLOR_RED){
					if(x->parent->color==COLOR_BLACK){
						//情况5 兄黑，兄远子红，父黑，标记父红，父支点内旋，节点切换为(之前的)兄弟节点
						x->parent->color=COLOR_RED;
						tree=rotate(tree,x->parent,ROTATE_LEFT);
						x=brother;
					}else{
						//情况6 兄黑，兄远子红，父红，父支点内旋，结束循环
						tree=rotate(tree,x->parent,ROTATE_LEFT);
						break;
					}
				}else{
					if(x->parent->color==COLOR_BLACK){
						//情况7 兄黑，兄子全黑，父黑，标记兄红，节点切换父节点
						brother->color=COLOR_RED;
						x=x->parent;
					}else{
						//情况8 兄黑，兄子全黑，父红，标记父黑兄红，结束循环
						brother->color=COLOR_RED;
						x->parent->color=COLOR_BLACK;
						break;
					}
					
				}
			}else{
				//情况9 兄红，标记父红兄黑，父节点内旋，转为情况3或4或6或8
				brother->color=COLOR_BLACK;
				x->parent->color=COLOR_RED;
				tree=rotate(tree,x->parent,ROTATE_LEFT);
			}
			*/
		}else{
			brother=x->parent->lchild;
			//x为右子，则内旋指的右旋，外旋指左旋，兄近子指的兄右子，兄远子指的兄左子
			if(brother->color==COLOR_BLACK){
				if(brother->lchild&&brother->lchild->color==COLOR_RED){
					//情况2 兄黑，兄远子红，标记兄远子黑，交换父兄色，父节点内旋，结束
					brother->color=x->parent->color;
					x->parent->color=COLOR_BLACK;
					brother->lchild->color=COLOR_BLACK;
					tree=rotate(tree,x->parent,ROTATE_RIGHT);
					break;
				}else if(brother->rchild&&brother->rchild->color==COLOR_RED){
					//情况3 兄黑，兄近子红，兄远子黑，标记兄红，兄近子黑，兄节点外旋，转为情况2
					brother->color=COLOR_RED;
					brother->rchild->color=COLOR_BLACK;
					tree=rotate(tree,brother,ROTATE_LEFT);
				}else{
					if(x->parent->color==COLOR_BLACK){
						//情况4 兄黑，兄子全黑，父黑，标记兄红，节点切换为父节点，向上迭代
						brother->color=COLOR_RED;
						x=x->parent;
					}else{
						//情况5 兄黑，兄子全黑，父红，标记兄红父黑，结束
						brother->color=COLOR_RED;
						x->parent->color=COLOR_BLACK;
						break;
					}
				}
			}else{
				//情况6 兄红，标记父红兄黑，父节点内旋，转为情况2或3或5
				brother->color=COLOR_BLACK;
				x->parent->color=COLOR_RED;
				tree=rotate(tree,x->parent,ROTATE_RIGHT);
			}
		}
	}
	freeRBTreeNode(del);
	return tree;
}

/*
 * 插入节点 tree为树指针，data为数据 返回修改后的树指针
 */
RBTree *insert(RBTree *tree,int data){
	int flag=0;
	RBTree *match=NULL;
	//如果树不为空，则进行查找
	if(tree){
		//取得当前数据父节点所在位置
		match=check(tree,data);
		//如果位置的数据与返回父节点位置相同，则不往下处理，直接返回原树
		flag=compare(match->data,data);
		if(match&&!flag){
			return tree;
		}
	}
	//初始化
	RBTree *now=(RBTree *)malloc(sizeof(RBTree));
	now->data=data;
	now->color=COLOR_RED;
	now->parent=match;
	if(flag>0){
		match->lchild=now;
	}else if(flag<0){
		match->rchild=now;
	}else{
		tree=now;
	}
	//树节点可能会变，这里返回树节点
	return addBalance(tree,now);
}
/*
 * 删除节点 tree为树指针，data为插入数据 返回修改后的树指针
 */
RBTree *delete(RBTree *tree,int data){
	RBTree *del=NULL,*rep=NULL;
	//树为空，直接返回，不处理
	if(!tree)
		return tree;
	//取得当前数据所在位置
	RBTree *match=check(tree,data);
	//如果数据和当前值不同，意味着没找到，直接结束
	if(compare(match->data,data))
		return tree;
	//正式开始删除操作
	//如果被删节点含有左右子节点，则找到被删节点的替代节点
	if(match->lchild&&match->rchild){
		del=successor(match->rchild);
		//将del的数据拷贝到match中
		match->data=del->data;
	}else{
		del=match;
	}
	//被删除的如有子节点则为替换节点
	if(del->lchild){
		rep=del->lchild;
	}else if(del->rchild){
		rep=del->rchild;
	}else if(del->color==COLOR_RED){
		//如果无子节点，并且为红色，则直接删掉，返回
		freeRBTreeNode(del);
		return tree;
	}
	//根据查找到的替代节点进行替代
	if(rep){
		rep->parent=del->parent;
		if(rep->parent){
			//判断是左树还是右树
			if(del==del->parent->lchild){
				del->parent->lchild=rep;
			}else{
				del->parent->rchild=rep;
			}
		}else{
			tree=rep;
		}
		free(del);
		del=rep;
	}
	//节点的删除规律：有孩子被删的，肯定是黑补红，直接涂黑，无孩子被删的，只关注被删黑的情况
	return delBalance(tree,del);
}
/*
 * 格式化打印树 tree为树指针
 */
void show(RBTree *tree){
	//将整颗数插入新的数据结构
	if(tree==NULL){
		return;
	}
	//把头根先进头链条hh
	H *hh=(H *)malloc(sizeof(H));
	//把头根的头节点设置红黑树指针并压入头根
	hh->l=(L *)malloc(sizeof(L));
	hh->l->t=tree;
	//定义遍历用的H和L指针
	H *h=hh;
	L *l;
	//定义初始高度
	int high=1;
	//定义for循环中使用到的count变量
	int count;
	if(hh->l->t->lchild||hh->l->t->rchild){
		//定义生成新结构所需要的L指针
		L *nl,*nhl;
		//定义一次性跨过排在前面的空节点数;
		int nil_count;
		high=0;
		//每条链表循环
		while(h){
			high++;
			l=h->l;
			nil_count=0;
			nhl=NULL;
			//链表内循环
			while(l&&(!l->t||!l->t->lchild&&!l->t->rchild)){
				nil_count+=2;
				l=l->next;
			}
			if(l){
				h->next=(H *)malloc(sizeof(H));
				//先将之前没有完成的插入
				if(nil_count){
					for(count=0;count<nil_count;count++){
						if(!nhl){
							nhl=(L *)malloc(sizeof(L));
							nl=nhl;
						}else{
							nl->next=(L *)malloc(sizeof(L));
							nl=nl->next;
						}
						nl->t=NULL;
						nl->next=NULL;
					}
				}
				while(l){
					if(!nhl){
						nhl=(L *)malloc(sizeof(L));
						nl=nhl;
						nl->t=NULL;
						nl->next=NULL;
						if(l->t->lchild)
							nl->t=l->t->lchild;
						nl->next=(L *)malloc(sizeof(L));
						nl->next->t=NULL;
						nl->next->next=NULL;
						if(l->t->rchild)
							nl->next->t=l->t->rchild;
						nl=nl->next;
					}else{
						nl->next=(L *)malloc(sizeof(L));
						nl->next->t=NULL;
						nl->next->next=NULL;
						if(l->t&&l->t->lchild)
							nl->next->t=l->t->lchild;
						nl=nl->next;
						nl->next=(L *)malloc(sizeof(L));
						nl->next->t=NULL;
						nl->next->next=NULL;
						if(l->t&&l->t->rchild)
							nl->next->t=l->t->rchild;
						nl=nl->next;
					}
					l=l->next;
				}
				h->next->l=nhl;
			}
			h=h->next;
		}
	}
	//h指针归位，并开始打印
	h=hh;
	//定义初始的层级，对应high
	int level=1;
	//计算最底层的节点总数(包含空节点)，作为定义枝缓存大小依据
	int bottom_count=mi(2,high-1);
	//定义枝缓存buff数组
	int cache[bottom_count];
	memset(cache, 0, sizeof(cache));
	//初始化第一层的枝的数量，每经过一层*2；
	int trunk_length=1;
	//定义并初始化枝缓存buff的游标
	int cache_cursor=0;
	//定义level=1时下一个节点需要跨越的步长，每次循环除以2
	int step=DATA_WIDTH*bottom_count;
	//定义其他变量以辅助编程---trunk_high为当前枝高度(high-level);now_center为当前遍历L的中心位置；now_point为枝游标当前到达的位置；little_step为2层枝之间横向的距离差值
	int trunk_high,now_center,now_point;
	while(h){
		if(level!=1){
			//遍历一下所有的cache然后清空并且缓存数组的游标归零
			for(count=0;count<bottom_count;count++){
				if(cache[count]){
					if(count%trunk_length==0){
						printf("\n");
						now_point=0;
					}
					if(cache[count]>0){
						printf("%*s",cache[count]-now_point,"/");
						now_point=cache[count];
					}else{
						printf("%*s",-cache[count]-now_point,"\\");
						now_point=-cache[count];
					}
				}
			}
			//打印下一层树节点之前，先换行
			printf("\n");
			//清空数组
			memset(cache, 0, sizeof(cache));
			cache_cursor=0;
		}
		//头L赋值给l指针，开始遍历
		l=h->l;
		//每一层初始的中心点为当前level步长的一半
		now_center=step/2;
		//计算当前level的枝高度，以供使用
		trunk_high=high-level;
		//当前level和总高度不相同时，需要处理首部的缩进；
		if(level!=high){
			//处理开头的空格
			printf("%*s",now_center-DATA_WIDTH/2,"");
		}
		//要准备下一层的枝的数量了，这里*2
		trunk_length*=2;
		while(l){
			//判断左右子树，将需要标记的位存入到cache中
			if(l->t){
				if(l->t->lchild){
					if(trunk_high==1){
						cache[cache_cursor]=now_center-1;
					}else{
						cache[cache_cursor]=now_center;
						cache[cache_cursor+(trunk_high-1)*trunk_length]=now_center-step/4+1;
						for(count=2;count<trunk_high;count++){
							//little_step=(step/4-1)/(trunk_high-1);
							cache[cache_cursor+(count-1)*trunk_length]=now_center-(count-1)*(step/4-1)/(trunk_high-1);
						}
					}
					cache_cursor++;
				}
				if(l->t->rchild){
					if(trunk_high==1){
						cache[cache_cursor]=-(2+now_center);
					}else{
						cache[cache_cursor]=-(2+now_center);
						cache[cache_cursor+(trunk_high-1)*trunk_length]=-(now_center+step/4+1);
						for(count=2;count<trunk_high;count++){
							//little_step=(step/4)/(trunk_high-1);
							cache[cache_cursor+(count-1)*trunk_length]=-(2+now_center+(count-1)*(step/4-1)/(trunk_high-1));
						}
					}
					cache_cursor++;
				}
				//处理树结点
				showNode(l->t);
			}else{
				printf("%*s",DATA_WIDTH,"");
			}
			if(l->next){
				//打印步长的空格
				printf("%*s",step-DATA_WIDTH,"");
			}
			now_center+=step;
			l=l->next;
		}
		//遍历过当前层，指针移动
		h=h->next;
		//遍历过当前层，level自增；
		level++;
		//遍历过当前层，步长/2;
		step/=2;
	}
	freeHl(hh);
}



/*
 * 主函数
 */
int main()
{
	RBTree *tree=NULL;
	//随机生成一组数据
	int a,k;
	srand((unsigned int)time(0));
	printf("插入数据：");
	for(int i=0;i<15;i++){
		a=rand()%99;
		if(!i)
			k=a;
		printf("%d ",a);
		tree=insert(tree,a);
	}
	printf("\n");
	show(tree);
	printf("\n--------------------delete:%d---------------------\n",k);
	tree=delete(tree,k);
	show(tree);
	printf("\n");
	return 0;
}


