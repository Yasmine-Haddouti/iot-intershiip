#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct User{
	char * username;
	char * password;
  int * acc_led;
  int * acc_url;
  int * acc_reg;
} User;

typedef struct Element{
	User* user;
	struct Element* next;
	struct Element* prev;
}Element;

typedef struct List{
	Element* start;
	Element* endl;
	int nb_users;
}List;

Element* create_element(User* user){
	Element* element = (Element*)malloc(sizeof(Element));
	element->user=user;
	element->next=NULL;
	element->prev=NULL;
	return element;
}

void init_list(List*list){
	list->start=NULL;
	list->endl=NULL;
	list->nb_users=0;
}

User* build_user(const char* create_username, const char* create_password,int acc_led, int acc_url, int acc_reg){
	User* user;
	user = (User*)malloc(sizeof(User));

	user->username=(char*)malloc((strlen(create_username))*sizeof(char*));
	strcpy(user->username,create_username);

	user->password=(char*)malloc((strlen(create_password))*sizeof(char*));
	strcpy(user->password,create_password);

  user->acc_led=acc_led;
  user->acc_url=acc_url;
  user->acc_reg=acc_reg;

	return user; 
}
 
void insert_beg(List*list, User* user){
	Element *element=create_element(user);
	if (list->start==NULL) {
		list->start=element;
		list->endl=element;
		list->nb_users++;
	}
	else {
		list->start->prev=element;
		element->next=list->start;
		list->start=element;
		list->nb_users++;
	}
}

void print_user(User * user) {
	printf("Username: %s \t Password: %s \t", user->username, user->password);
  if (user->acc_led==1)
    printf("Can modifie the IOT Led state\n");
  else
    printf("Cannot accès the led IOT\n");
}

void print_list(List*list){
	if (list->start==NULL)
		printf("The list is empty.\n");
	else {
		printf("List of users :\n");
		Element*current=list->start;
		while (current!=NULL){
			print_user(current->user);
			current=current->next;
		}
	}
}

void free_memoire(List*list){
	Element*current=list->start;
	while (current!=NULL){
		current=current->next;
		free(current);
	}
	free(list);
}
