#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#define NTHREADS  3
#define PLAYER    0
#define BANKER    1
#define CROUPIER  2
#define MAXCARDS  3
typedef enum { Go, Stop, Draw } cmd_t;
typedef enum { Unread, Read   } bufstate_t;
/* function prototypes */
void *player(void *arg);
void *banker(void *arg);
void  croupier(int nrounds);
/* global shared variables */
int pcards[3]; 
int bcards[3]; 
bufstate_t croupier_to_player; 
bufstate_t croupier_to_banker; 
bufstate_t player_to_croupier; 
bufstate_t player_to_banker; 
bufstate_t banker_to_croupier; 
pthread_mutex_t player_mutex; 
pthread_mutex_t banker_mutex; 
pthread_mutex_t command_mutex;
cmd_t cmd; 
pthread_cond_t command; 
pthread_cond_t player_cond; 
pthread_cond_t banker_cond; 

void *player(void *arg)  { 
	cmd_t temp_cmd; 
	int pcard1; 
	int pcard2; 
        int player_card3; 	
	int pcard_sum; 
	int pcard_sum;  
	char * p_temp_card3; 
	setbuf(stdout, NULL);  

	pthread_mutex_lock(&command_mutex); 
	while (croupier_to_player == Read){
		pthread_cond_wait(&command, &command_mutex); 
	}
	temp_cmd = Go; 
	croupier_to_player = Read; 
	pthread_mutex_unlock(&command_mutex); 

	while(temp_cmd == Go){  	 

		p_temp_card3 = ""; 

		pthread_mutex_lock(&player_mutex);  
		pcard1 = rand() % 14; 
	        pcard2 = rand() % 14; 	
		pcards[0] = pcard1; 
		pcards[1] = pcard2; 	
		player_to_croupier = Unread; 
 		player_to_banker = Read; 
		pthread_cond_signal(&player_cond); 
		pthread_mutex_unlock(&player_mutex); 
		
		pthread_mutex_lock(&command_mutex); 
		while (croupier_to_player == Read){  
			pthread_cond_wait(&command, &command_mutex); 
		} 
		temp_cmd = cmd; 
		croupier_to_player = Read; 
		pthread_cond_signal(&command);
		pthread_mutex_unlock(&command_mutex); 
		
		if (temp_cmd == Stop){
			break; 
		} 
		else if (temp_cmd == Go){
			continue;
		} 
 		 
		pcard_sum = (pcard1 + pcard2) % 10; 

		pthread_mutex_lock(&player_mutex); 

		if (pcard_sum <= 5){
		if (pcard_sum <= 5){ 
			printf("Player draws "); 
			player_card3 = rand() % 14; 

		        if (player_card3 >= 10){ 
				if (player_card3 == 11){ 
					p_temp_card3 = "J"; 	
				}
				else if (player_card3 == 12){
					p_temp_card3 = "Q";
				} 
				else if (player_card3 == 13){ 
					p_temp_card3 = "K"; 
				} 
				player_card3 = 0; 
			} 

			if (strcmp(p_temp_card3, "") != 0){ 
				printf("%s\n", p_temp_card3); 
			} 
			else {
				printf("%d\n", player_card3); 
			} 

			pcards[2] = player_card3; 
		}
		else {
			player_card3 = 0; 
			printf("The player stands.\n"); 
		      	pcards[2] = player_card3; 
	        } 	      
		player_to_croupier = Unread;
	        player_to_banker = Unread; 	
		pthread_cond_broadcast(&player_cond); 
		pthread_mutex_unlock(&player_mutex);  
		pthread_mutex_lock(&command_mutex); 
		while (croupier_to_player == Read){
			pthread_cond_wait(&command, &command_mutex); 	
		}
		temp_cmd = cmd; 
		croupier_to_player = Read;
		pthread_cond_signal(&command); 
		pthread_mutex_unlock(&command_mutex); 
		
		if (temp_cmd == Stop){
			break;
		} 
		else {
			continue;
		} 
	} 
	return NULL;
  
}

void *banker(void *arg) {

	cmd_t temp_cmd; 
	cmd_t temp_cmd;  
	char * b_temp_card3; 
	int bcard1; 
	int bcard2; 
	int bcard3;
	int player_card3;
	int banker_sum; 
	int to_draw; 
	setbuf(stdout, NULL);  
	pthread_mutex_lock(&command_mutex); 
	while (croupier_to_banker == Read){ 
		pthread_cond_wait(&command, &command_mutex); 
	} 
	temp_cmd = cmd; 
	croupier_to_banker = Read; 
	pthread_mutex_unlock(&command_mutex); 

	while(1){ 
		b_temp_card3 = ""; 
		to_draw = 0;	
		pthread_mutex_lock(&banker_mutex);  
		bcard1 = rand() % 14; 
	        bcard2 = rand() % 14;  
		bcards[0] = bcard1; 
		bcards[1] = bcard2; 	
		banker_to_croupier = Unread; 
		pthread_cond_signal(&banker_cond); 
		pthread_mutex_unlock(&banker_mutex); 
		pthread_mutex_lock(&command_mutex); 
		while (croupier_to_banker == Read){  
			pthread_cond_wait(&command, &command_mutex); 
		} 
		temp_cmd = cmd; 
		croupier_to_banker = Read; 
		pthread_mutex_unlock(&command_mutex);  
		if (temp_cmd == Stop){
			break; 
		} 
		else if (temp_cmd == Go){
			continue;
		} 
		pthread_mutex_lock(&player_mutex); 
		while (player_to_banker == Read){ 
			pthread_cond_wait(&player_cond, &player_mutex); 
		}
	        player_card3 = pcards[2]; 	
		player_to_banker = Read; 
		pthread_mutex_unlock(&player_mutex);
		
	        if (player_card3 >= 10){ 
			player_card3 = 0; 
		} 
		if (bcard1 >= 10){ 
			bcard1 = 0; 
		} 
		if (bcard2 >= 10){ 
			bcard2 = 0;
		} 
		banker_sum = (bcard1 + bcard2) % 10; 
		if (banker_sum <= 2){
			to_draw = 1;
		} 
		else if ( (banker_sum == 3) && (player_card3 == 8) ){ 
			to_draw = 1; 
		} 
		else if ( (banker_sum == 4) && (player_card3 >= 2) && (player_card3 <= 7) ){ 
			to_draw = 1; 
		}	
		else if ( (banker_sum == 5) && (player_card3 >= 4) && (player_card3 <= 7) ) {
			to_draw = 1; 
		}
		else if ( (banker_sum == 6) && ( (player_card3 == 6) || (player_card3 == 7) ) ) { 
			to_draw = 1;  
		} 

		pthread_mutex_lock(&banker_mutex);  

		if (to_draw == 1){ 

			printf("The Bank draws "); 
			bcard3 = rand() % 14; 
			bcards[2] = bcard3;

			if (bcard3 >= 10){ 
				if (bcard3 == 11){ 
					b_temp_card3 = "J"; 
				} 
				else if (bcard3 == 12){ 
					b_temp_card3 = "Q"; 
				} 
				else if (bcard3 == 13){ 
					b_temp_card3 = "K"; 
				} 
				bcard3 = 0; 
			} 

			if (strcmp(b_temp_card3, "") != 0){ 
				printf("%s\n", b_temp_card3); 
			} 
			else { 
				printf("%d\n", bcard3); 
			} 

			bcards[2] = bcard3; 

		} 

		else { 
			bcard3 = 0;
			printf("Bank Stands.\n"); 
			bcards[2] = bcard3; // banker stands, so just update the third card value's as 0. 
		} 

		banker_to_croupier = Unread; 	
		pthread_cond_signal(&banker_cond); 
		pthread_mutex_unlock(&banker_mutex);   
		pthread_mutex_lock(&command_mutex); 
		while (croupier_to_banker == Read){
			pthread_cond_wait(&command, &command_mutex); 	
		}
		temp_cmd = cmd; 
		croupier_to_banker = Read;
		pthread_cond_signal(&command); 
		pthread_mutex_unlock(&command_mutex);  
		// only Stop or Go at this point. 
		if (temp_cmd == Stop){
			break;
		} 
		else {
			continue;
		} 
	} 
	return NULL;
  
}
void croupier(int nrounds) {
	int player_score = 0; 
	int banker_score = 0;
	int ties = 0;
	int pcard1; 
	int pcard2; 
	int pcard3; 
	int pcard_sum; 
	int bcard1; 
	int bcard2; 
	int bcard3;
	int bcard_sum; 
	
	int winner; 
	
	int go_draw = 0; 

	char * p_temp_card1;
        char * p_temp_card2;  
	char * p_temp_card3; 

	char * b_temp_card1;
	char * b_temp_card2; 
	char * b_temp_card3; 

	setbuf(stdout, NULL); 

	for (int i = 0; i < nrounds; i++){ 

		p_temp_card1 = ""; 
		p_temp_card2 = ""; 
	        p_temp_card3 = ""; 

		b_temp_card1 = ""; 
		b_temp_card2 = ""; 
		b_temp_card3 = ""; 
		b_temp_card2 = "";  

		winner = 0; 
		go_draw = 0;  
		printf("--------------------\n");
		printf("Round %d\n", i + 1);  
	        if (i == 0){ 
			pthread_mutex_lock(&command_mutex);
			cmd = Go; 
			croupier_to_player = Unread;
	        	croupier_to_banker = Unread;	
			pthread_cond_broadcast(&command); 
			pthread_mutex_unlock(&command_mutex);
		}	
				
		// get the scores from the banker and player
		
		pthread_mutex_lock(&player_mutex); 
		while (player_to_croupier == Read){
			pthread_cond_wait(&player_cond, &player_mutex); 
		} 
		pcard1 = pcards[0]; // consume 
		pcard2 = pcards[1]; // consume 
		player_to_croupier = Read; 
		pthread_mutex_unlock(&player_mutex); 
		
		// get the banker's score now. 
		
		pthread_mutex_lock(&banker_mutex); 
		while (banker_to_croupier == Read){
			pthread_cond_wait(&banker_cond, &banker_mutex); 
		} 
		bcard1 = bcards[0]; 
		bcard2 = bcards[1]; 
		banker_to_croupier = Read; 
		pthread_mutex_unlock(&banker_mutex); 
  
		// have draw statements using printf here.
		
	        if (pcard1 >= 10){ 
			if (pcard1 == 11){ 
				p_temp_card1 = "J"; 
			} 
			else if (pcard1 == 12){ 
				p_temp_card1 = "Q"; 
			} 
			else if (pcard1 == 13){
				p_temp_card1 = "K";
			} 
			pcard1 = 0; 
		} 
		printf("The Player draws: "); 
		
		if (strcmp(p_temp_card1, "") != 0) { 
			printf("%s,", p_temp_card1);   
		}
		else {
			printf("%d,", pcard1);
		 } 
		if (pcard2 >= 10) { 
			if (pcard2 == 11) { 
				p_temp_card2 = "J"; 
			} 
			else if (pcard2 == 12){ 
				p_temp_card2 = "Q"; 
			} 
			else if (pcard2 == 13){ 
				p_temp_card2 = "K"; 
			} 
			pcard2 = 0;
		} 
		if (strcmp(p_temp_card2, "") != 0 ){
			printf("%s\n", p_temp_card2); 
		} 
		else {
			printf("%d\n", pcard2); 
		}
		pcard_sum = (pcard1 + pcard2) % 10;  
		
		printf("Bank draws: ");  
		if (bcard1 >= 10){ 
			if (bcard1 == 11){ 
				b_temp_card1 = "J"; 
			} 
			else if (bcard1 == 12){
				b_temp_card1 = "Q"; 
			} 
			else if (bcard1 == 13){
				b_temp_card1 = "K"; 
			} 
			bcard1 = 0;  
		} 
		if (strcmp(b_temp_card1, "") != 0){ 
			printf("%s,", b_temp_card1); 
		} 
		else {
			printf("%d,", bcard1); 
		} 
	        if (bcard2 >= 10){ 
			if (bcard2 == 11){ 
				b_temp_card2 = "J"; 
			} 
			else if (bcard2 == 12){ 
				b_temp_card2 = "Q"; 
			} 
			else if (bcard2 == 13){ 
				b_temp_card2 = "K"; 
			} 
			bcard2 = 0;
		}
		if (strcmp(b_temp_card2, "") != 0){
			printf("%s\n", b_temp_card2); 
		}	
		else {
			printf("%d\n", bcard2); 
		} 
		bcard_sum = (bcard1 + bcard2) % 10; 
		
		if ( (pcard_sum > bcard_sum) || ( (pcard_sum >= 8) && (bcard_sum < 8) ) ){ 
			printf("Player wins!\n"); 
			player_score += 1; 
			winner = 1; 
		} 
		
		else if ( (pcard_sum < bcard_sum) || ( (bcard_sum >= 8) && (pcard_sum < 8) ) ){ 
			printf("Bank wins!\n"); 
			banker_score += 1; 
			winner = 1; 
		} 
		
		// now decide whether to keep going or not in this round 
		pthread_mutex_lock(&command_mutex); 
 
 		if ( (i == nrounds - 1) && (winner == 1) ){ 
			cmd = Stop; 
		}		
		else if (winner == 1){ 
			cmd = Go; 
		}
		else {
			cmd = Draw;
			go_draw = 1; 
		} 
		
		croupier_to_player = Unread; 
		croupier_to_banker = Unread;
		pthread_cond_broadcast(&command); 
		pthread_mutex_unlock(&command_mutex);  
								 
		if (go_draw == 1){ 
			 
			pthread_mutex_lock(&player_mutex); 
			while (player_to_croupier == Read){ 
				pthread_cond_wait(&player_cond, &player_mutex); 
			}
			pcard3 = pcards[2]; 
		        player_to_croupier = Read;   
			pthread_mutex_unlock(&player_mutex);
			
			// since we got the player's third card. Now let's get banker's third card. 
			
			pthread_mutex_lock(&banker_mutex); 
			while (banker_to_croupier == Read){
				pthread_cond_wait(&banker_cond, &banker_mutex); 
			} 
			bcard3 = bcards[2]; 
			banker_to_croupier = Read;  
			pthread_mutex_unlock(&banker_mutex); 

			// since we consumed both of their cards, now calculate.   

			printf("The Player draws "); 

			if (pcard3 >= 10){ 
				if (pcard3 == 11){ 
					p_temp_card3 = "J"; 	
				}
				else if (pcard3 == 12){
					p_temp_card3 = "Q";
				} 
				else if (pcard3 == 13){ 
					p_temp_card3 = "K"; 
				} 
				pcard3 = 0; 
			} 

			if (strcmp(p_temp_card3, "") != 0){ 
				printf("%s\n", p_temp_card3); 
			} 
			else {
				printf("%d\n", pcard3); 
			} 

			printf("The Bank draws "); 

			if (bcard3 >= 10){ 
				if (bcard3 == 11){ 
					b_temp_card3 = "J"; 
				} 
				else if (bcard3 == 12){ 
					b_temp_card3 = "Q"; 
				} 
				else if (bcard3 == 13){ 
					b_temp_card3 = "K"; 
				} 
				bcard3 = 0; 
			} 

			if (strcmp(b_temp_card3, "") != 0){ 
				printf("%s\n", b_temp_card3); 
			} 
			else { 
				printf("%d\n", bcard3); 
			} 


			pcard_sum = ((pcard_sum + pcard3) % 10);  

			bcard_sum = ((bcard_sum + bcard3) % 10); 
			
			if (pcard_sum > bcard_sum) { 
				printf("The player Wins!\n"); 
				player_score += 1; 
			} 
			
			else if (pcard_sum < bcard_sum){ 
				printf("The Bank Wins!\n");
				banker_score += 1; 
			} 
			else { 
				printf("Tie!\n"); 
				ties += 1; 
			} 
			// now produce a command. // at this point we only have GO / Stop. 
			pthread_mutex_lock(&command_mutex); 
			
			if (i == nrounds - 1){ 
				cmd = Stop;
			} 
			else {
				cmd = Go;
			} 
			croupier_to_player = Unread; 
			croupier_to_banker = Unread; 
			pthread_cond_broadcast(&command); 
			pthread_mutex_unlock(&command_mutex);  
 
		}
 
	}
	
	//formatting

	printf("--------------------\n");
	printf("The Results: \n"); 
	printf("Player: %d\n", player_score); 
	printf("Bank: %d\n", banker_score); 
	printf("Ties: %d\n", ties); 
	if (player_score > banker_score) { 
		printf("Player wins.\n"); 
	} 
	else if (player_score < banker_score) { 
		printf("Bank wins.\n"); 
	} 
	else {
		printf("They tied!");
	} 
			
}
int main(int argc, char **argv) {
  
  int nrounds = 0;
  struct timeval tv; 
  pthread_t player_id; 
  pthread_t banker_id; 
  if (argc != 2) {
    fprintf(stderr, "usage: %s <nrounds>\n", argv[0]);
    exit(1);
  }
  nrounds = atoi(argv[1]);
  if (nrounds <= 0)
    return 0;
  gettimeofday(&tv, NULL);  
  srand(tv.tv_sec + tv.tv_usec + getpid()); 
  
  croupier_to_player = Read; 
  croupier_to_banker = Read; 
  player_to_croupier = Read; 
  player_to_banker = Read; 
  banker_to_croupier = Read; 
  cmd = Stop;
  if (pthread_cond_init(&command, NULL) != 0){
	perror("conditional vars init"); 
	exit(1);
  }
  if (pthread_cond_init(&player_cond, NULL) != 0){
	perror("conditional vars init"); 
	exit(1); 
  }
  if (pthread_cond_init(&banker_cond, NULL) != 0){ 
	perror("conditional vars init"); 
	exit(1); 
  }
  if (pthread_mutex_init(&command_mutex, NULL) != 0){ 
	perror("mutex init"); 
	exit(1);
  } 
  if (pthread_mutex_init(&player_mutex, NULL) != 0){
	perror("mutex init"); 
       	exit(1);
  } 
  if (pthread_mutex_init(&banker_mutex, NULL) != 0){
	perror("mutex init");
	exit(1); 	
  } 
 
  if (pthread_create(&player_id, NULL, player, NULL) != 0){
	perror("pthread_create"); 
	exit(1); 
  }
  
  if (pthread_create(&banker_id, NULL, banker, NULL) != 0){ 
	perror("pthread_create"); 
	exit(1); 
  }
   
  croupier(nrounds); 
   
  if (pthread_join(player_id, NULL) != 0){ 
	perror("pthread_join"); 
        exit(1); 	
  }  
  
  if (pthread_join(banker_id, NULL) != 0){
	perror("pthread_join"); 
	exit(1); 
  }
  
  if (pthread_mutex_destroy(&command_mutex) != 0){
	perror("mutex destroy"); 
	exit(1); 	
  } 
  if (pthread_mutex_destroy(&player_mutex) != 0){ 
	perror("mutex destroy");
	exit(1); 
  }
  if (pthread_mutex_destroy(&banker_mutex) != 0){
	perror("mutex destroy"); 
  }
  if (pthread_cond_destroy(&command) != 0){
	perror("cond destory");
	exit(1);
  } 
  if (pthread_cond_destroy(&player_cond) != 0){
	perror("cond destory"); 
	exit(1);
  } 
  if (pthread_cond_destroy(&banker_cond) != 0){ 
	perror("cond destroy"); 
	exit(1); 
  } 
}