#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#define DECK_SIZE 52
typedef struct {
	int value;
	int type;//1-bich,2-tep,3-ro
	char showvalue[3];
	char showtype[3];
}Card;
typedef struct 
{
	int rank;//1-10 la tu sanh rong den mau thau
	int high_value;// so sanh cac tay bai co ranh bang nhau
	int second_value;//so sanh trong truong hop ca 2 
	int third_value;//so sanh con lai
	int fouth_value;
	int last_value;
}Handcard;
typedef struct
{
	char nickname[20];
	int money;
	Handcard hand;
}Player;
struct box 
{
	int id;
	int status;//1 = private or  0 = public
	char password[20];
	int canPlay;
	Player player1;//creator
	Player player2;//joiner
	int start;//slot start
	int total;//total bet
	struct box* next;
};
typedef struct box* Room;
Room room = NULL;
char* convertType(int type)
{
	if(type ==1)
		return "♠";
	if(type == 2)
		return "♣";
	if(type == 3)
		return "♦";
	if(type == 4)
		return "♥";
}
char* convertValue(int value)
{
	if(value== 1)
		return "A";
	if(value== 2)
		return "2";
	if(value== 3)
		return "3";
	if(value== 4)
		return "4";
	if(value== 5)
		return "5";
	if(value== 6)
		return "6";
	if(value== 7)
		return "7";
	if(value== 8)
		return "8";
	if(value== 9)
		return "9";
	if(value== 10)
		return "10";
	if(value== 11)
		return "J";
	if(value== 12)
		return "Q";
	if(value== 13)
		return "K";
}
Card newcard(int input)
{
	Card new;	
	new.value = input%13+1;
	new.type =  input/13 + 1;
	strcpy(new.showvalue,convertValue(new.value));
	strcpy(new.showtype,convertType(new.type));
	return new;
}
void newRoom(int status,int id, char* password,Player creator_room)
{
	Room new = (Room)calloc(1,sizeof(struct box));
	new->id = id;
	new->status = status;
	new->canPlay = 0;
	strcpy(new->password,password);
	new->player1 = creator_room;
	new->player2 = creator_room;
	new->next = NULL;
	if(room == NULL)
	{
		room = new;
	}
	else
	{
		Room p = room;
		while(p->next!=NULL)
		{
			p=p->next;
		}
		p->next = new;
	}
}
Player newPlayer(char* nickname,int money)
{
	Player new;
	strcpy(new.nickname,nickname);
	new.money = money;
	return new;
}
void joinRoom(Player newPlayer)
{
	if(room->canPlay==1)
	{
		printf("Full slot\n");
		return;
	}
	room->player2 = newPlayer;
	room->canPlay = 1;
}

int checkRoom()
{
	if(room->player1.money==0)
	{
		printf("Player %s failed\n", room->player1.nickname);
		return 2;
	}
	if(room->player2.money==0)
	{
		printf("Player %s failed\n", room->player2.nickname);
		return 1;
	}
	return 0;
}
void showPlayer()
{
	printf("Player %s: %dVND\n",room->player1.nickname,room->player1.money);
	printf("Player %s: %dVND\n",room->player2.nickname,room->player2.money);
}
int* rollcard()
{
	int* deck = (int*)calloc(DECK_SIZE,sizeof(int));
	int j, k, temp;
	for(int i = 0; i < 52; i++)
	{
		deck[i] = i;
	}
	srand(time(NULL));
	for(int i = 0;i<200; i++)
	{
		j = rand()%52;
		k = rand()%52;
		temp = deck[j];
		deck[j] = deck[k];
		deck[k] = temp;
	}
	return deck;
	// int* swap = (int*)calloc(DECK_SIZE,sizeof(int));
	// memset(deck,0,DECK_SIZE*sizeof(int));
	// int value;
	// srand(time(NULL));
	// int count = 0;
	// while(count < DECK_SIZE)
	// {
	// 	do{
	// 		value = rand()%DECK_SIZE;
	// 	}while(deck[value] == 1);
	// 	deck[value] = 1;
	// 	swap[count] = value;
	// 	count++;
	// }
	// return swap;
}
//check thung return mang [type,high-value,....]
int* checkF(Card handcard[])//check thung
{
	int* result = (int*)calloc(6,sizeof(int));
	int count[] = {0,0,0,0};
	int filter[4][7];
	int max = 0,index1,index2 = 1;
	for(int i = 0; i < 7;i++)
	{
		if(handcard[i].type == 1)
		{
			filter[0][count[0]] = handcard[i].value;
			count[0]++;
		}
		else if(handcard[i].type == 2)
		{
			filter[1][count[1]] = handcard[i].value;
			count[1]++;
		}
		else if(handcard[i].type == 3)
		{
			filter[2][count[2]] = handcard[i].value;
			count[2]++;
		}
		else
		{
			filter[3][count[3]] = handcard[i].value;
			count[3]++;
		}
	}
	for(int i = 0; i < 4; i++)
	{
		if(max < count[i])
		{
			max = count[i];
			result[0] = i + 1;
		}
	}
	if(max<5)
	{
		memset(result,0,6*sizeof(int));
	}
	else
	{
		index1 = max - 1;
		if(filter[result[0]-1][0] == 1)
		{
			result[1] = 14;
			index2 = 2;
		}
		while(index2 < 6)
		{
			result[index2] = filter[result[0]-1][index1];
			index2++;
			index1--;
		}
	}
	return result;
}
//check thung pha sanh return bac cao nhat
int checkSF(Card handcard[])
{
	int* flush = checkF(handcard);
	int type = flush[0];
	int i = 0,count = 0,index = 0;
	if(type == 0)
		return 0;
	int straight[7];
	for(int i = 0; i < 7; i ++)
	{
		if(handcard[i].type == type)
		{
			straight[index] = handcard[i].value;
			index++;
		}
	}
	if(straight[0] == 1 &&straight[index-1] == 13&&straight[index-2] == 12 &&straight[index-3] == 11 &&straight[index-4] == 10)
	{
		return 14;
	}
	for(int i = 0; i < index - 1;i++)
	{
		if(straight[i] +1 == straight[i+1])
		{
			count++;
		}
		else
		{
			if(count>=4)
			{
				return straight[i];
			}
			count = 0;
		}
	}
	if(count>=4)
		return straight[index-1];
	return 0;	
}
//check tu quy return [gia tri tu quy,so le cao nhat]
int* checkFour(Card handcard[])
{
	int* result = (int*)calloc(2,sizeof(int));
	result[0] = 0;
	int count = 0,i = 1;
	while(i < 7)
	{
		if(handcard[i].value == handcard[i-1].value)
		{
			count++;
		}
		else
		{
			count = 0;
		}
		if(count == 3)
		{
			if(handcard[i-1].value == 1)
				result[0] =  14;
			result[0] = handcard[i-1].value;
		}
		i++;
	}
	i = 6;
	while(i>0)
	{
		if(handcard[i].value != result[0])
		{
			result[1] = handcard[i].value;
			break;
		}
		i--;
	}
	if(handcard[0].value==1&&result[0]!=14)
		result[1] = 14;
	return result;
}
//check doi return doi cao nhat va 3 gia tri le cao nhat
int* checkPair(Card handcard[])
{
	int* result = (int*)calloc(4,sizeof(int));
	result[0]  = 0;
	int count = 0,i = 1;
	while(i < 7)
	{
		if(handcard[i].value == handcard[i - 1].value)
		{
			count++;
		}
		else
		{
			if(count==1)
			{
				if(handcard[i-1].value==1)
				{
					result[0] = 14;
				}
				else if(handcard[i-1].value > result[0])
				{
					result[0] = handcard[i-1].value;
				}
			}
			count = 0;
		}
		i++;
	}
	if(count==1&&handcard[6].value > result[0])
	{
		result[0] = handcard[6].value;
	}
	count = 1;
	i = 6;
	if(handcard[0].value==1&&result[0]!=14)
	{
		result[count++] = 14;
	}
	while(i > -1)
	{
		if(handcard[i].value != result[0])
		{
			result[count++] = handcard[i].value;
		}
		if(count > 3)
			break;
		i--;
	}
	return result;
}
//check bo ba return gia tri bo ba va cap bai le cao nhat
int* checkThree(Card handcard[])
{
	int* result = (int*)calloc(3,sizeof(int));
	result[0] = 0;
	int count = 0,i = 1;
	while(i < 7)
	{
		if(handcard[i].value == handcard[i - 1].value)
		{
			count++;
		}
		else
		{
			if(count==2)
			{
				if(handcard[i-1].value==1)
				{
					result[0] = 14;
				}
				else if(handcard[i-1].value > result[0])
				{
					result[0] = handcard[i-1].value;
				}
			}
			count = 0;
		}
		i++;
	}
	if(count==2&&handcard[6].value > result[0])
	{
		result[0] = handcard[6].value;
	}
	count = 1;
	if(handcard[0].value==1&&result[0]!=14)
	{
		result[count++] = 14;
	}
	i = 6;
	while(i > -1)
	{
		if(handcard[i].value != result[0])
		{
			result[count++] = handcard[i].value;
		}
		if(count > 2)
			break;
		i--;
	}
	return result;
}
//check full hourse return gia tri bo ba va gia tri doi
int* checkFH(Card handcard[])
{
	int* result = (int*)calloc(2,sizeof(int));
	int* pair = checkPair(handcard);
	int* three = checkThree(handcard);
	result[0] = three[0];
	result[1] = pair[0];
	if(result[0] == 0 || result[1] ==0)
	{
		result[0] = 0;
		result[1] = 0;
	}
	return result;
}
//check sang return gia tri cao nhat cua sanh
int checkS(Card handcard[])
{
	int count = 1,num = 0;
	int value[7];
	value[0] = handcard[0].value;
	for(int i = 1; i < 7; i++)
	{
		if(handcard[i].value != handcard[i-1].value)
		{
			value[count] = handcard[i].value;
			count++;
		}
	}
	if(value[0]==1&&value[count-1]==13&&value[count-2]==12&&value[count-3]==11&&value[count-4]==10)
		return 14;
	for(int i = 0; i < count - 1 ; i++ )
	{
		if(value[i]+ 1 == value[i+1])
			num++;
		else
		{
			if(num>=4)
				return value[i];
			num = 0;
		}
	}
	if(num>=4)
		return value[count - 1];
	return 0;
}
//check two pair return 2 gia tri cua doi va gia tri le cao nhat
int* check2Pair(Card handcard[])
{
	int* result = (int*)calloc(3,sizeof(int));
	result[1] = 0;
	result[0] = 15;
	int count = 0;
	if(handcard[0].value==1&&handcard[1].value==1)
	{
		result[count++] = 14;
	}
	for(int i = 6; i > 0;i--)
	{
		if(handcard[i].value == handcard[i-1].value && handcard[i].value !=result[0])
		{
			result[count] = handcard[i].value;
			count++;
		}
		if(count == 2)
			break;
	}
	if(result[1] == 0||result[0] == 15)
	{
		result[0] = 0;
	}
	if(handcard[0].value==1)
	{
		result[2] = 14;
		return result;
	}
	count = 6;
	while(count>0)
	{
		if(handcard[count].value != result[0]&&handcard[count].value!=result[1])
		{
			result[2] = handcard[count].value;
			break;
		}
		count--;
	}
	return result;
}

//check 5 gia tri cao nhat cua mau thau
int* checkHC(Card handcard[])
{
	int* result = (int*)calloc(5,sizeof(int));
	int count = 0;
	if(handcard[0].value == 1)
	{
		result[0] = 14;
		count++;
	}
	result[count] = handcard[6].value;
	count++;
	result[count] = handcard[5].value;
	count++;
	result[count] = handcard[4].value;
	count++;
	result[count] = handcard[3].value;
	count++;
	if(count!=5)
		result[count] = handcard[2].value;
	return result;
}
void sortHandCard(Card handcard[])
{
	Card temp;
	for(int i = 0; i < 6; i++)
	{
		for(int j = i+1; j < 7; j++)
		{
			if(handcard[i].value > handcard[j].value)
			{
				temp = handcard[i];
				handcard[i] = handcard[j];
				handcard[j] = temp;
			}
		}
	}
}
Handcard calculate(Card handcard[])
{
	Handcard new;
	memset(&new,0,sizeof(new));
	sortHandCard(handcard);
	int SF = checkSF(handcard),S = checkS(handcard);
	int* Four = checkFour(handcard);
	int* F = checkF(handcard);
	int* FH = checkFH(handcard);
	int* twoP = check2Pair(handcard);
	int* Three = checkThree(handcard);
	int* Pair = checkPair(handcard);
	int* HC = checkHC(handcard);
	if(SF ==14)//sanh rong
	{
		new.rank = 10;
		return new;
	}
	if(SF != 0)//thung pha sanh
	{
		new.rank = 9;
		new.high_value = SF;
		return new;
	}
	if(Four[0]!=0)//tu quy
	{
		new.rank = 8;
		new.high_value = Four[0];
		new.second_value = Four[1];
		return new;
	}
	if(FH[0]!=0)//full house
	{
		new.rank = 7;
		new.high_value = FH[0];
		new.second_value = FH[1];
		return new;
	}
	if(F[0]!=0)//sanh
	{
		new.rank = 6;
		new.high_value = F[1];
		new.second_value = F[2];
		new.third_value = F[3];
		new.fouth_value = F[4];
		new.last_value = F[5];
		return new;
	}
	if(S!=0)//thung
	{
		new.rank = 5;
		new.high_value = S;
		return new;
	}
	if(Three[0]!=0)//bo ba
	{
		new.rank = 4;
		new.high_value = Three[0];
		new.second_value = Three[1];
		new.third_value = Three[2];
		return new;
	}
	if(twoP[0]!=0)//2 doi
	{
		new.rank = 3;
		new.high_value = twoP[0];
		new.second_value = twoP[1];
		new.third_value = twoP[2];
		return new;
	}
	if(Pair[0]!=0)//doi
	{
		new.rank = 2;
		new.high_value = Pair[0];
		new.second_value = Pair[1];
		new.third_value = Pair[2];
		new.fouth_value = Pair[3];
		return new;
	}
	new.rank = 1;//mau thau
	new.high_value = HC[0];
	new.second_value = HC[1];
	new.third_value = HC[2];
	new.fouth_value = HC[3];
	new.last_value = HC[4];
	return new;
}
int compare(int num1,int num2)
{
	if(num1>num2)
		return 1;
	else if(num1<num2)
		return -1;
	return 0;
}

void drawTable(int round, Card deck[],int start)
{
	if(round==0)
	{
		printf("------------------------------------------\n");
		printf("|\t| | \t| | \t| | \t| | \t|\n");
		printf("|\t| | \t| | \t| | \t| | \t|\n");
		printf("------------------------------------------\n");
	}
	if(round==1)
	{
		printf("------------------------------------------\n");
		printf("|%s\t| |%s\t| |%s\t| | \t| | \t|\n",deck[start].showvalue,deck[start+1].showvalue,deck[start+2].showvalue);
		printf("|%s\t| |%s\t| |%s\t| | \t| | \t|\n",deck[start].showtype,deck[start+1].showtype,deck[start+2].showtype);
		printf("------------------------------------------\n");
	}
	if(round==2)
	{
		printf("------------------------------------------\n");
		printf("|%s\t| |%s\t| |%s\t| |%s\t| | \t|\n",deck[start].showvalue,deck[start+1].showvalue,deck[start+2].showvalue,deck[start+3].showvalue);
		printf("|%s\t| |%s\t| |%s\t| |%s\t| | \t|\nn",deck[start].showtype,deck[start+1].showtype,deck[start+2].showtype,deck[start+3].showtype);
		printf("------------------------------------------\n");
	}
	if(round==3)
	{
		printf("------------------------------------------\n");
		printf("|%s\t| |%s\t| |%s\t| |%s\t| |%s\t|\n",deck[start].showvalue,deck[start+1].showvalue,deck[start+2].showvalue,deck[start+3].showvalue,deck[start+4].showvalue);
		printf("|%s\t| |%s\t| |%s\t| |%s\t| |%s\t|\n",deck[start].showtype,deck[start+1].showtype,deck[start+2].showtype,deck[start+3].showtype,deck[start+4].showtype);
		printf("------------------------------------------\n");
	}
}
void drawHand(char* nickname,Card hand[])
{
	printf("  Player %s:  \n",nickname);
	printf("------------------\n");
	printf("|%s\t| |%s\t|\n",convertValue(hand[0].value),convertValue(hand[1].value));
	printf("|%s\t| |%s\t|\n",convertType(hand[0].type),convertType(hand[1].type));
	printf("------------------\n");
}
int handCompare(Handcard hand1,Handcard hand2)
{
	int result = compare(hand1.rank,hand2.rank);
	if(result!=0)
		return result;
	result = compare(hand1.high_value,hand2.high_value);
	if(result!=0)
		return result;
	result = compare(hand1.second_value,hand2.second_value);
	if(result!=0)
		return result;
	result = compare(hand1.third_value,hand2.third_value);
	if(result!=0)
		return result;
	result = compare(hand1.fouth_value,hand2.fouth_value);
	if(result!=0)
		return result;
	result = compare(hand1.last_value,hand2.last_value);
	if(result!=0)
		return result;
	return 0;
}

// int checkbet(int bet[],int exist[])
// {
// 	int balance = 0;
// 	int index = 0;
// 	int count = 0;
// 	for(int i = 0; i< room->number;i++)
// 	{
// 		if(exist[i]==-1)
// 			count++:
// 	}
// 	if(count==room->number-1)
// 		return 1;
// 	while(exist[index]==0&&bet[index]!=0)
// 	{
// 		index++;
// 	}
// 	if(index==room->number)
// 		return 0;
// 	balance = bet[index];
// 	for(int i = index+1;i<room->number;i++)
// 	{
// 		if(exist[i]!=0&&bet[i]!=balance)
// 			return 0;
// 	}
// 	return 1;
// }
// int round1(int exist[])
// {
// 	int bumber = room->number;
// 	int pot = 0;
// 	int bet[number];
// 	int count = 0;
// 	int high_bet = 0;
// 	for(int i = 0; i< number;i++)
// 	{
// 		bet[i]=0;
// 	}
// 	Player p = room->player;
// 	while(1)
// 	{
// 		while(1)
// 		{
// 			if(p==NULL)
// 				p = room->player;
// 			if(exist[p->slot-1]==0)
// 				p = p->next;
// 			else break;
// 		}
// 		printf("%s's turn:\n",p->nickname);
		
// 	}

// 	return pot;
// }
void showHand(Player player)
{
	printf("Rank: %d|High: %d|Second: %d|Third: %d|Fouth: %d|Last: %d\n", player.hand.rank,player.hand.high_value,player.hand.second_value,player.hand.third_value,player.hand.fouth_value,player.hand.last_value);
}
// void cpyHand(Hand hand1,Hand hand2)
// {

// }
void match()
{
	int *deck = rollcard();
	Card Deck[DECK_SIZE];
	Card playercard[2][7];
	for(int i = 0; i < DECK_SIZE;i++)
	{
		Deck[i] = newcard(deck[i]);
	}
	for(int i = 0; i < 2;i++)
	{
		playercard[i][0] = Deck[i];
		playercard[i][1] = Deck[2 + i];
		for(int j = 2; j < 7; j++)
		{
			playercard[i][j] = Deck[2+j];
		}
	}
	drawTable(0,Deck,4);
	// round1(exist);
	drawHand(room->player1.nickname,playercard[0]);
	room->player1.hand = calculate(playercard[0]);
	drawHand(room->player2.nickname,playercard[1]);
	room->player2.hand = calculate(playercard[1]);
	showHand(room->player1);
	showHand(room->player2);
	if(handCompare(room->player1.hand,room->player2.hand)>0)
	{
		printf("%s wins this match\n",room->player1.nickname );
		room->player1.money+=10000;
		room->player2.money-=10000;
	}
	else if(handCompare(room->player1.hand,room->player2.hand)<0)
	{
		printf("%s wins this match\n",room->player2.nickname );
		room->player2.money+=10000;
		room->player1.money-=10000;
	}
	else printf("Draw:)!!!\n" );
}
int main(int argc, char const *argv[])
{
	Player p1 = newPlayer("Bao",40000);
	Player p2 = newPlayer("Du",40000);
	newRoom(0,123,"",p1);
	joinRoom(p2);
	int _continue = 0;
	while(_continue==0)
	{
		match();
		showPlayer();
		printf("--------------------------------------------------\n");	
		_continue = checkRoom();
	}
	if(_continue==1)
		printf("Winner is %s\n",room->player1.nickname );
	if(_continue==2)
		printf("Winner is %s\n",room->player2.nickname );
	return 0;
}

