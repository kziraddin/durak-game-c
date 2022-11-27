/*  DURAK GAME (durak.c)

    Durak is a game popular in former soviet union countries

    Outline of the program... This is a 2 player (HUMAN/COMPOUTER implementation.....)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h> /* uppercase */

/* ENUMS */
/* We need to enumerate to make code readable and keep track of essential parts*/
typedef enum Rank {SIX = 6, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE} Rank;
typedef enum Suit {HEARTS, DIAMONDS, CLUBS, SPADES} Suit;
typedef enum Color {RED, BLACK} Color;
typedef enum Location {HOUSE, COMPUTER, HUMAN, TABLE, GARBAGE, TRUMP} Location;
typedef enum boolean {FALSE, TRUE} boolean;

/* GLOBAL VARIABLES */
int gameCounter = 1;
int trumpCard; /* index of trump card */
int trumpSuit; /* suit of trump card */
int turn; /* COMPUTER or HUMAN */
int attacker; /* COMPUTER or HUMAN */
boolean gameOver = FALSE;
int winner; /* COMPUTER or HUMAN */

/* game (table) area */
int attacks[6]; /* indexes of Deck cards */
int defenses[6]; /* indexes of Deck cards */
int tableLevel = 0; /* level of attacks and defenses on table */

/* GLOBAL CONSTANTS */
const int DECKSIZE = 36;
const int EMPTY = -1;

/* Represents a player card --- Location keeps track of where the card is at any given time*/
typedef struct Card {
    int rank;
    int suit;
    int color;
    /* where this card is at any give time */
    int location; 
} Card;

/* Durak is played with DECKSIZE cards which change hands, or end up on table or are expired (garbage)*/
/* This is main storage with information about cards and it keeps track of where the cards are*/
Card Deck[36];

/* FUNCTION HEADINGS */


void initializeDeck();
/* converts Location from integer (where the card is in the deck) to a string to print to the screen */
void locationToString(int lctn, char str[]);
/* converts Color from integer (which are Red, Black (0,1)) to a string to print to the screen */
void colorToString(int clr, char str[]);
/* converts Suit from integer (which are Hearts, Diamonds, Clubs, Spades (0,1,2,3)) to a string to print to the screen */
void suitToString(int ste, char str[]);
/* converts Rank from integer (6, 7, 8, 9, 10, Jack, Queen, King, Ace (which starts from 6)) to a string to print to the screen */
void rankToString(int rnk, char str[]);
/* counts cards in any location */
int countLocation(int lctn);
/* randomly assigns 6 cards to computer and 6 cards to human uniquely */
void assignCardsToPlayers();
/* Trump card will be a powerfull card during the game, it's selected randomly */
void initializeTrumpCard();
/* defines who plays first (computer or human) */
void setInitialTurn();
void printDeck();
void printGame();
void printComputer();
void printHuman();
void computerAttacks();
void computerDefends();
void playComputer();
/* Converts player (Human) input to index of Deck */
int userSelectedToIndex(int x);
/* Checks if the player (human) entered the corect command */
boolean isHumanCommandValid(char cmd);
/* Checks if the player (Human) entered the correct Move which means (if computer attacked 6 of hearts human can play only the same suit and higher rank) */
boolean isHumanMoveValid(int move);
/* Checks if input is valid, then plays human selected card */
void playHuman();
void playGame();
/* after clearing table players must have 6 cards again */
void completeCards();
/* Clears table when no more cards to attack */
void tableClear();
void printGameOverMessage();
/* Checks every turn that game is over or not*/
boolean gameIsOver();
/* This Clears Screen */
void clearScreen(void) {
	printf("%s\n", "\033[2J\033[1;1H");
}


/* ********************************************************* */


int main() {

    initializeDeck();
    // printDeck(); // for debugging
    printGame();

    /* Game Loop */
    do {
        playGame();
        if(gameIsOver()) 
            printGameOverMessage();
        else 
            printGame();
    } while (!gameOver);

    return 0;
}



/* FUNCTIONS */

void playGame() {

    //gameCounter++;

    if (turn == COMPUTER) {
        playComputer();
        turn = HUMAN;
        gameCounter++;
    } else { 
        playHuman();
        turn = COMPUTER;
        gameCounter++;
    }
}

/* converts Location from integer (where the card is in the deck) to a string to print to the screen */
void locationToString(int lctn, char str[]) {

    switch (lctn)
    {
        case HOUSE:
            stpcpy(str, "HSE");
            break;
        case COMPUTER:
            stpcpy(str, "COMPUTER");
            break;
        case HUMAN:
            stpcpy(str, "HUMAN");
            break;
        case TABLE:
            stpcpy(str, "TABLE");
            break;
        case GARBAGE:
            stpcpy(str, "GARBAGE");
            break;
        case TRUMP:
            stpcpy(str, "TRUMP");
            break;
    }
}

/* randomly assigns 6 cards to computer and 6 cards to human uniquely */
void assignCardsToPlayers() {
    
    int i;
    int randNum;
    int count = 0;
    int x;

    int tempArr[12];

    boolean duplicate = FALSE;
    time_t t;

    /* set random seed from time */
    srand((unsigned)time(&t));

    /* Part 1: need to get 12 unique random numbers to assign card to players 6 uniquely each */

    /* Initialize temp array */
    for (i = 0; i < 12; i++)
        tempArr[i] = EMPTY;

    count = 0;

    /* get 12 unique random numbers */
    do {
        randNum = rand() % DECKSIZE;
        for (i = 0; i < 12; i++) {
            if (tempArr[i] == EMPTY) {
                tempArr[i] = randNum;
                count++;
                break;
            } else if (tempArr[i] == randNum) {
                break;
            }
        }
    } while(count < 12);

    /* Part 2: assign 6 of 12 unique random numbers to HUMAN, the others to COMPUTER */
    for (i = 0; i < 12; i++) {
        x = tempArr[i];
        if(i % 2 == 0)
            Deck[x].location = HUMAN;
        else
            Deck[tempArr[i]].location = COMPUTER;
    }

    return;
}

/* picks one of HSE cards randomly and marks it as TRUMP */
void initializeTrumpCard() {

    int i;
    int count =0;
    int rv;

    rv = rand()%24;
    for ( i = 0; i < DECKSIZE; i++)
    {
        if (Deck[i].location == HOUSE)
            count++;
        if (count == rv)
        {   
            Deck[i].location = TRUMP;
            trumpCard = i;
            trumpSuit = Deck[trumpCard].suit;
            break;
        }
    }
}

/* defines who plays first (computer or human) */
void setInitialTurn() {

    turn = EMPTY;
    int i;
    int rv;
    char s[10];

    /* pick who (COMPUTER/HUMAN) has the lowest trump card.
       if nobody has a trump card, then randimly assign turn to COMPUTER or HUMAN */
    for ( i = 0; i < DECKSIZE; i++)
    {
        if (Deck[i].location == COMPUTER || Deck[i].location == HUMAN)
        {
            if (Deck[i].suit == trumpSuit)
            {
                turn = Deck[i].location;
                break;
            }
        }
    }
    /* sometimes we can not find lowest card of the same suit as trump, randomly pick first player */
    if(turn == EMPTY){
        rv = rand() % 2;
        turn = rv + 1;
    }

    /* game begins with attach, therefore first attacker is the same with the first turn */ 
    attacker = turn;
}

void initializeDeck(){

    int i, j;
    int index = 0;

    /* Initialize cards in the Deck. 9 ranks x 4 suits = 36 cards */
    for (i = SIX; i <= ACE; i++) {
        for(j = HEARTS; j <= SPADES; j++) {
            Deck[index].rank = i;
            Deck[index].suit = j;
            if (index > 17)
                Deck[index].color = BLACK;
            else
                Deck[index].color = RED;
            Deck[index].location = HOUSE;
            index++;
        }
    }

    assignCardsToPlayers();
    initializeTrumpCard();
    setInitialTurn();

    /* initializes attacks[] and defenses[] arrays*/
    for ( i = 0; i < 6; i++)
    {
        attacks[i] = EMPTY;
        defenses[i] = EMPTY;
    }
}

/* used for printing on screen */
void colorToString(int clr, char str[]) {
    
    switch (clr) {
        case RED:
            strcpy(str, "RED");
            break;
        case BLACK:
            strcpy(str, "BLACK");
            break;
    }
} 

void suitToString(int ste, char str[]) {

    switch (ste){
        case HEARTS:
            strcpy(str, "HEARTS");
            break;
        case DIAMONDS:
            strcpy(str, "DIAMONDS");
            break;
        case CLUBS:
            strcpy(str, "CLUBS");
            break;
        case SPADES:
            strcpy(str, "SPADES");
            break;
    }
}

void rankToString(int rnk, char str[]) {

    switch (rnk){
        case SIX:
            stpcpy(str, "6");
            break;
        case SEVEN:
            stpcpy(str, "7");
            break;
        case EIGHT:
            stpcpy(str, "8");
            break;
        case NINE:
            stpcpy(str, "9");
            break;
        case TEN:
            stpcpy(str, "10");
            break;
        case JACK:
            stpcpy(str, "JACK");
            break;
        case QUEEN:
            stpcpy(str, "QUEEN");
            break;
        case KING:
            stpcpy(str, "KING");
            break;
        case ACE:
            stpcpy(str, "ACE");
            break;
        }
}


/* counts cards in any location */
int countLocation(int lctn){

    int i;
    int count = 0;

    for ( i = 0; i < DECKSIZE ; i++)
        if (Deck[i].location == lctn)
            count++;

    return count;
} 

void printComputer(){

    int i;
    int count = 1;
    char strColor[6];
    char strSuit [9];
    char strLocation [9];
    char strRank [6];

    /* Count Computer Cards*/
   printf("Computer (%d Cards):\n", countLocation(COMPUTER));

    count = 1;
    for(i = 0; i < DECKSIZE; i++){

        if(Deck[i].location == COMPUTER){
            colorToString(Deck[i].color, strColor);
            suitToString(Deck[i].suit, strSuit);
            rankToString(Deck[i].rank, strRank);
            printf("%2d %7s of %-9s %-7s\n", count,strRank, strSuit, strColor);
            count++;
        }
    }
}

void printHuman(){

    int i;
    int count = 1;
    char strColor[6];
    char strSuit [9];
    char strLocation [9];
    char strRank [6];

    /* Counts Human Cards*/
    printf("Your %d Cards:\n\n", countLocation(HUMAN));

    /* Prints Player Cards */
    count = 1;
    for(i = 0; i < DECKSIZE; i++){

        if(Deck[i].location == HUMAN){  
            colorToString(Deck[i].color, strColor);
            suitToString(Deck[i].suit, strSuit);
            rankToString(Deck[i].rank, strRank);
            printf("%2d %7s of %-9s %-7s\n", count,strRank, strSuit, strColor);
            count++;
        }
    }
    printf("\n\n");
    printf("=============================================================\n\n");
    printf("T: Take Cards\t\t C: Clear table\t\t Q: Quit Game\t\t");

}

/* after clearing table players must have 6 cards again */
void completeCards() {

    int i, j;
    int countHouse, countTrump;
    int rv;  
    int humanMissing;
    int computerMissing;

    /* count the missing cards */
    humanMissing = 6 - countLocation(HUMAN);
    computerMissing = 6 - countLocation(COMPUTER);

    /* the rules of the game says, attacker gets the cards first */
        if(humanMissing > 0) {

            /* asssing to human attacker first */
            for(i = 0; i < humanMissing; i++) {
                for(j = 0; j < DECKSIZE; j++){
                    if(Deck[j].location == HOUSE){
                        Deck[j].location = HUMAN;
                        break;
                    }
                }
            }
        }
        if(computerMissing > 0){
            /* move to computer attacker */    
            for(i = 0; i < computerMissing; i++) {
                for(j = 0; j < DECKSIZE; j++){
                    if(Deck[j].location == HOUSE){
                        Deck[j].location = COMPUTER;
                        break;
                    }
                }
            }
        }

    /* gets how many cards are left in the house and whether trump card is available */
    countHouse = countLocation(HOUSE);
    countTrump = countLocation(TRUMP);

    /* no more cards in the house including the trump card - nothing to assign */
    if(countHouse == 0 && countTrump == 0)
        return;

    /* only trump card left, change it to house card */
    if (countHouse == 0 && countTrump == 1){
        Deck[trumpCard].location = HOUSE;
        countTrump = 0; 
        countHouse++; /* only one house card left */
        for (i = 0; i < DECKSIZE; i++) {
            if(Deck[i].location == HOUSE){
                rv = rand() % 2 + 1;
                if(rv == 1){
                    Deck[i].location = COMPUTER;
                }
                else if(rv == 2){
                    Deck[i].location = HUMAN;
                }
                break;
            }
        }
    }
}

/* Clears table when no more cards to attack */
void tableClear(){

    int i;

    /* Checks attacks and defenses arrays at each table level */
    for(i = 0; i < 6; i++){
        if(attacks[i]!= EMPTY && defenses[i]!= EMPTY){
            Deck[attacks[i]].location = GARBAGE;
            Deck[defenses[i]].location = GARBAGE;
            attacks[i] = EMPTY;
            defenses[i] = EMPTY;
        }
    }

    /* After clearing table players must have 6 cards again */
    completeCards();

    /* changes attacker */
    if(attacker == COMPUTER)
        attacker = HUMAN;
    else
        attacker = COMPUTER;

    /* resets tablelevel */
    tableLevel = 0;
}

/* Player = Human or Computer */
void tableTake(int player){

    int i;

    /* Checks attacks and defenses arrays at each table level */
    for(i = 0; i < 6; i++){
        if(attacks[i]!= EMPTY){
            Deck[attacks[i]].location = player;
            attacks[i] = EMPTY;
        }
    }

    for(i = 0; i < 6; i++) {
        if(defenses[i] != EMPTY){
            Deck[defenses[i]].location = player;
            defenses[i] = EMPTY;
        }
    }
    completeCards();
    
    /* Resets tablelevel */
    tableLevel = 0;
}

void computerAttacks(){

    int i;
    int j;

    switch (tableLevel) {
        case 0:
            /* initial attack. picks the lowest non trump card */
            for(i = 0; i < DECKSIZE; i++) {
                if (Deck[i].location == COMPUTER && Deck[i].suit != trumpSuit) {
                    Deck[i].location = TABLE;
                    attacks[tableLevel] = i;
                    return;  
                }
            }
            /* Couldn't find computer card that is not trump suit, so get the smallest card that is trump */
            for ( i = 0; i < DECKSIZE; i++){
                if(Deck[i].location == COMPUTER){
                    Deck[i].location = TABLE;
                    attacks[tableLevel] = i;
                    return;  
                }
            }   
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            for(i = 0; i < DECKSIZE; i++) {
                if(Deck[i].location == COMPUTER) {
                    for(j = 0; j < tableLevel; j++) {
                        /* look through attacks array */
                        if(attacks[j] != EMPTY) {
                            if(Deck[attacks[j]].rank == Deck[i].rank && Deck[i].suit != trumpSuit) {
                                Deck[i].location = TABLE;
                                attacks[tableLevel] = i;
                                return; 
                            }
                        }
                        /* look through defenses array */
                        if(defenses[j] != EMPTY) {
                            if(Deck[defenses[j]].rank == Deck[i].rank && Deck[i].suit != trumpSuit) {
                                Deck[i].location = TABLE;
                                attacks[tableLevel] = i;
                                return; 
                            }
                        }
                    }
                }
            }
            /* If we can't find a non-trump card, then computer chooses to clear table (rather than put a trump card) */
            tableClear();
            break;
        default:
            /* If somehow table level > 5, clear table (rules of the game) */
            tableClear();
    }
}


void computerDefends(){

    int i;

    /* Looking for a higher rank card of the same suit */
    for(i = 0; i < DECKSIZE; i++){
        if(Deck[i].location == COMPUTER){
            if(Deck[i].suit == Deck[attacks[tableLevel]].suit && Deck[i].rank > Deck[attacks[tableLevel]].rank){
                Deck[i].location = TABLE;
                defenses[tableLevel] = i;
                tableLevel++;
                return;  // for
            }
        }
    }
    /* Looking for the smallest Trump card */
    for ( i = 0; i < DECKSIZE; i++){
        if(Deck[i].location == COMPUTER && Deck[i].suit == trumpSuit){
            Deck[i].location = TABLE;
            defenses[tableLevel] = i;
            tableLevel++;
            return;  
        }
    }

    /* Couldn't find a card to play, therefore Computer must take it */
    tableTake(COMPUTER);

    /* TableLevel++ is goinf to be a PROBLEM when tableLevel is already 5, fix it by clearing table */
    if(tableLevel > 5)
        tableClear();

}

void playComputer(){

    if(attacker == COMPUTER)
        computerAttacks();
    else
        computerDefends();
}


/* Converts player (Human) input to index of Deck */
int userSelectedToIndex(int x) {

    int count = 0;
    int i;
    /* Change entered value to index */
    for ( i = 0; i < DECKSIZE; i++)
    {
        if(Deck[i].location == HUMAN)
            count++;
        if(count == x)
            break;
    }

    return i;    
}

/* Checks if the player (human) entered the corect command */
boolean isHumanCommandValid(char cmd){

    switch(cmd){

        case 'Q': 
            return TRUE;
            break;
        case 'C':
            //if(tableLevel == 0){
            if(defenses[tableLevel] != EMPTY)
                return TRUE;
             else{
                if(attacks[tableLevel] == EMPTY)
                    return TRUE;
            }
            break;
        case 'T':
            if(defenses[tableLevel] == EMPTY)
                return TRUE;
        default:
            return FALSE;
    }

    return FALSE;
}  



/* Checks if the player (Human) entered the correct Move which means (if computer attacked 6 of hearts human can play only the same suit and higher rank) */
boolean isHumanMoveValid(int move){

    /* idx converts user input to selected move as a index */
    int idx = userSelectedToIndex(move);
    int cardRank = Deck[idx].rank;
    int cardSuit = Deck[idx].suit;
    boolean rankValid = FALSE;
    int i;

    if(attacker == HUMAN) {
        switch(tableLevel) {
            case 0:
                /* Human plays first so, all cards are okay */
                return TRUE;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                /* For human to attack on tableLevel 1 or higher */
                for(i = 0; i < tableLevel; i++) {
                    if(attacks[i] != EMPTY) 
                        if(Deck[attacks[i]].rank == cardRank) 
                            rankValid = TRUE;
                    if(defenses[i] != EMPTY) 
                        if(Deck[defenses[i]].rank == cardRank) 
                            rankValid = TRUE;
                }
                return rankValid;
                break;
            default:
                /* If tableLevel > 5, rules of game says you can't move */
                return FALSE;
                break;
        }
    } 

    if(attacker == COMPUTER) {
        if(Deck[idx].suit == trumpSuit)
            /* If selected card is Trump then it's ok */
            return TRUE;
        if(cardRank > Deck[attacks[tableLevel]].rank && cardSuit == Deck[attacks[tableLevel]].suit)
            /* Selected card is same suit but higher rank */
           return TRUE;
    }

    return FALSE;    
}


void humanAttacks(int idx){


    /* We don't need to check anything because move was already validated when human entered move */
    Deck[idx].location = TABLE;
    attacks[tableLevel] = idx;
}

void humanDefends(int idx){

    /* We don't need to check anything because move was already validated */
    Deck[idx].location = TABLE;
    defenses[tableLevel] = idx;

    tableLevel++;
    /* This is going to be a PROBLEM when tableLevel is already 5, fix it by clearing table */
    if(tableLevel > 5)
        tableClear();
}

/* Checks if input is valid, then plays human selected card */
void playHuman(){

    char enteredStr[3];
    int enteredVal;
    char c = 'X';
    int idx;
    int count;
    char bell = 7;
    boolean valid;
    gameOver = FALSE;

    /* check if input is valid otherwise loop for a valid input */
    do{

        valid = TRUE; /* assume valid is TRUE */
        printf("Enter move: ");
        scanf("%s", enteredStr);

        /* atoi converts character array to integer. returns 0 if it can't convert*/
        enteredVal = atoi(enteredStr);

        /* if we have an integer*/
        if(enteredVal > 0){
            if(enteredVal > countLocation(HUMAN))
                /* integer too big */
                valid  = FALSE;   
            else
                valid = isHumanMoveValid(enteredVal);

        } else {
            /* we have a character */
            if(strlen(enteredStr) == 1){
                c = toupper(enteredStr[0]);
                if(c != 'Q' && c != 'C' && c != 'T') {
                    valid = FALSE;
                    c = 'X';
                } else {
                    valid = isHumanCommandValid(c);
                }
            } else {
                valid = FALSE;
            } 
        }

        if(!valid)
            printf("%c", bell);
            printf("Invalid Value\n");
    } while (!valid);

    /* if input is a valid command, handle the command */
    if(c == 'Q'){
        printf("\n  >>> Goodbye <<<\n\n");
        gameOver = TRUE;
        exit(0);
        return;
    }
    if(c == 'C'){
        tableClear();
        return;
    }
    if(c == 'T'){
        tableTake(HUMAN);
        return;
    }

    /* Input is a valid number, find index of selected card */
    idx = userSelectedToIndex(enteredVal);

    if(attacker == HUMAN)
        humanAttacks(idx);
    else
        humanDefends(idx);
}



void printGame(){

    if(gameOver)
        return;

    clearScreen();

    int i;
    int count = 0;
    // Change the names of these to more descriptive names
    char s1[10];
    char s2[10];
    char s3[10];
    char s4[10];

    /* Needs "s" because turn is an integer. "s1" return integer to string like (COMPUTER OR HUMAN) */
    locationToString(turn, s1);   
    /* Needs "s" because attacker is an integer. "s4" return integer to string like (COMPUTER OR HUMAN) */
    locationToString(attacker, s4);   

    rankToString(Deck[trumpCard].rank, s2);
    suitToString(Deck[trumpCard].suit, s3);
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n");
    printf("                                         GAME OF DURAK \n\n");
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n");
    printf("                                                                          Game Counter: %d\n", gameCounter);
    printf("                                                                          Current Turn: %s\n", s1);
    printf("                                                                          Table Level: %d\n\n", tableLevel);
    printf("                                                                          Trump Card: %s of %s\n", s2, s3);
    printf("                                                                          Trump Suit: %s\n\n", s3);

    /* Count House Cards, include trump card */
    printf("House (%d cards)\n\n", countLocation(HOUSE) + countLocation(TRUMP));
    
    /* Count Garbage Cards*/
    printf("Garbage (%d cards)\n\n", countLocation(GARBAGE));

    printComputer(); 

    /* Table area */
    printf("********************************************************************************************************\n\n");

    for ( i = 0; i < 6; i++){   
        rankToString(Deck[attacks[i]].rank, s2);
        suitToString(Deck[attacks[i]].suit, s3);

        if(attacks[i] != EMPTY)
            printf("%5s of %-8s ", s2, s3);
    }
    printf("\n\n");
    
    printf("********************************************************************************************************\n\n");

    for ( i = 0; i < 6; i++){   
        rankToString(Deck[defenses[i]].rank, s2);
        suitToString(Deck[defenses[i]].suit, s3);

        if(defenses[i] != EMPTY)
            printf("%5s of %-8s ", s2, s3);
    }
    printf("\n\n");

    printf("********************************************************************************************************\n\n");
    
    printHuman();
    printf("\n\n");

}

void printDeck(){

    int i;
    char strColor[6];
    char strSuit [9];
    char strLocation [9];
    char strRank [6];

    printf("All cards in the Deck:\n\n");

    for(i = 0; i < DECKSIZE; i++){

        /* convert everything to strings for displaying */
        colorToString(Deck[i].color, strColor);
        suitToString(Deck[i].suit, strSuit);
        rankToString(Deck[i].rank, strRank);
        locationToString(Deck[i].location, strLocation);

        printf("%2d %7s of %-9s %-7s Owner: %-12s\n", i, strRank, strSuit, strColor, strLocation);
    }
}

/* Checks every turn that game is over or not*/
boolean gameIsOver() {

    /* Nothing left in house */
    if(countLocation(HOUSE) == 0 && countLocation(TRUMP) == 0) {
        if(countLocation(COMPUTER) == 0 && turn == COMPUTER) {
            gameOver = TRUE;
            winner = COMPUTER;
            return TRUE;
        }
        if(countLocation(HUMAN) == 0 && turn == HUMAN) {
            gameOver = TRUE;
            winner = HUMAN;
            return TRUE;
        }
    }
    /* Game is not over */
    gameOver = FALSE;
    winner = EMPTY;
    return FALSE;
}

void printGameOverMessage() {
    
    char strLocation [9];
    locationToString(winner, strLocation);
    
    if(winner == HUMAN) {
        printf("\n***********************************************************\n");
        printf("\tGame Over! Winner is %s. Congatulations.", strLocation);
        printf("\n***********************************************************\n\n");
    } else {
        printf("\n***********************************************************\n");
        printf("\tGame Over! Winner is %s. You DURAK!", strLocation);
        printf("\n***********************************************************\n\n");
    }

}


