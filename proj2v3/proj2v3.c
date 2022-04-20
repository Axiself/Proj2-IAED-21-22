/*
 * File: proj2.c
 * Author:  João Paulo Borges Pereira 102081
 * Description: Second project for the IAED subject, 2021/2022.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "proj1v2.h"

#define MAX_COMMAND_SIZE 65535 	/* max number of characters in a single command */
#define REALLOC_INTERVAL 10		/* interval between reallocs */
#define HASHTABLE_SIZE 20011    /* hastable size */

/* Malloc and realloc functions */

void* cMalloc(int size) {
	void *ptr = malloc(size);
	if(ptr == NULL && size != 0) {
		printf("No memory.");
		exit(0);
	}
	return ptr;
}


void* cRealloc(void* ptr, int size) {
	ptr = realloc(ptr, size);
	if(ptr == NULL && size != 0) {
		printf("No memory.");
		exit(0);
	}
	return ptr;
}

/* Hashtable related functions */

int hashfunction(char str[]) {
    int key = 0, i, len = strlen(str);
    for(i = 0; i < len; i++) {
        key += str[i];
    }
    return key;
}


void initTable(struct Reservation *table[]) {
	int i;
	for(i = 0; i < HASHTABLE_SIZE; i++) {
		table[i] = NULL;
	}
}


void insertTable(struct Reservation *table[], struct Reservation *r) {
	int key = hashfunction(r->id);
	struct Reservation *temp = table[key];
	struct Reservation *prev = NULL;
	while(temp != NULL) {
		prev = temp;
		temp = temp->next;
	}
	temp = r;
	if(prev != NULL)
		prev->next = r;
}


struct Reservation* findReservation(char id[], struct Reservation *reservationTable[]) {
	int key = hashfunction(id);
	struct Reservation *temp = reservationTable[key];
	while(temp != NULL) {
		if(strcmp(temp->id, id) == 0) 
			return temp;
		temp = temp->next;
	}
	return NULL;
}


void deleteReservation(char id[], struct Reservation *reservationTable[]) {
	int key = hashfunction(id);
	struct Reservation *temp = reservationTable[key];
	struct Reservation *prev = NULL;
	while((strcmp(temp->id, id) != 0) && (temp != NULL)) {
		prev = temp;
		temp = temp->next;
	}
	if((prev != NULL) && (strcmp(temp->id, id) == 0)) {
		prev->next = temp->next;
		free(temp->id);
		free(temp);
	} else if(strcmp(temp->id, id) == 0) {
		free(temp->id);
		free(temp);
		reservationTable[key] = NULL;
	} else {
		printf("not found\n");
	}
}


void deleteTable(struct Reservation *reservationTable[]) {
	int i;
	struct Reservation *temp, *prev;
	for(i = 0; i < HASHTABLE_SIZE; i++) {
		temp = reservationTable[i];
		while(temp != NULL) {
			prev = temp;
			temp = temp->next;
			free(prev->id);
			free(prev);
		}
	}
}


struct Reservation *createReservation(char *id, int nPassengers, char flightID[],
 int flightDate) {
	struct Reservation *r = malloc(sizeof(struct Reservation));
	r->id = malloc(strlen(id));
	strcpy(r->id, id);
	r->nPassengers = nPassengers;
	strcpy(r->flightID, flightID);
	r->flightDate = flightDate;

	return r;
}


int validateListReservation(char id[], int d, Flight flights[],
 int flightIterator, int currentDate) {
	if (findFlight(id, d, flights, flightIterator) == ERROR)
		printf("%s: flight does not exist\r\n", id);
	else if(validateDate(d, currentDate) == FALSE)
		printf("invalid date\r\n");
	else
		return TRUE;
	return FALSE;
}


void listReservations(Flight flight) {
	int i, j;
	struct Reservation *temp;
	for(i = 1; i < flight.reservationsAmount; i++) {
		temp = flight.reservations[i];
		j = i-1;
		while((j >= 0) && (strcmp(flight.reservations[j]->id, temp->id) > 0)) {
			flight.reservations[j+1] = flight.reservations[j];
			j = j - 1;
		}
		flight.reservations[j+1] = temp;
	}
	for(i = 0; i < flight.reservationsAmount; i++) {
		printf("%s %d\n", flight.reservations[j]->id, flight.reservations[j]->nPassengers);
	}
}


int validateReservationID(char id[]) {
	int i = 0, len = strlen(id);

	if(strlen(id) < 10)
		return FALSE;
	while(i < len) {
		if(!(id[i] >= 'A' && id[i] <= 'Z') && !(id[i] >= '0' && id[i] <= '9'))
			return FALSE;
		i++;
	}
	return TRUE;
}


int validateReservation(struct Reservation *r, struct Reservation *reservationTable[],
 Flight flights[], int flightIterator, int currentDate) {
	int flightIndex = findFlight(r->flightID, r->flightDate, flights, flightIterator);

	if(validateReservationID(r->id) == FALSE)
		printf("invalid reservation code\r\n");
	else if (flightIndex == ERROR)
		printf("%s: flight does not exist\r\n", r->flightID);
	else if(findReservation(r->id, reservationTable) != NULL)
		printf("%s: flight reservation already used\r\n", r->id);
	else if(flights[flightIndex].capacity - flights[flightIndex].occupied < r->nPassengers)
		printf("too many reservations\r\n");
	else if(validateDate(r->flightDate, currentDate) == FALSE)
		printf("invalid date\r\n");
	else if(r->nPassengers < 1)
		printf("invalid passenger number\r\n");
	else
		return TRUE;
	return FALSE;
}


void addReservationToFlight(Flight flights[], int flightIterator, struct Reservation *r) {
	int flightIndex = findFlight(r->flightID, r->flightDate, flights, flightIterator);
	flights[flightIndex].reservations = realloc(flights[flightIndex].reservations, flights[flightIndex].reservationsAmount+1);
	flights[flightIndex].reservations[flights[flightIndex].reservationsAmount] = r;
	flights[flightIndex].reservationsAmount++;
}


void addListReservations(struct Reservation *reservationTable[], Flight flights[],
 int flightIterator, int currentDate) {
	struct Reservation *r = malloc(sizeof(struct Reservation));
	int flightIndex;
	char c, temp[MAX_COMMAND_SIZE+1];
	readNextWord(r->flightID);
	r->flightDate = readDate();
	if((c = getchar()) == '\r' || c == '\n') {
		if(validateListReservation(r->flightID, r->flightDate, flights,
		 flightIterator, currentDate)) {

			flightIndex = findFlight(r->flightID, r->flightDate, flights, flightIterator);
			listReservations(flights[flightIndex]);
		} 
	} else {
		readNextWord(temp);
		r->id = cMalloc(strlen(temp)+1);
		strcpy(r->id, temp);
		scanf("%d", &r->nPassengers);
		if(validateReservation(r, reservationTable, flights, flightIterator,
		currentDate)) {
			insertTable(reservationTable, r);
			addReservationToFlight(flights, flightIterator, r);
		}
	}
}


void destroy(struct Reservation *reservationTable[], Flight flights[], int flightIterator) {
	int i;
	deleteTable(reservationTable);
	for(i = 0; i < flightIterator; i++) {
		free(flights[i].reservations);
	}
}


int main() {
	int c;
	int airportIterator = 0;		/* number of Airports added */
	Airport airports[MAX_AIRPORTS];	/* Airports vector */
	int flightIterator = 0;			/* number of Flights added */
	Flight flights[MAX_FLIGHTS];	/* Flights vector */
	int currentDate = 20220101;		/* current date in system */
    struct Reservation *reservationTable[HASHTABLE_SIZE];	/* Reservation hashtable */
	initTable(reservationTable);

	
	while ((c = getchar()) != EOF) {
		switch (c) {
		case 'q': destroy(reservationTable, flights, flightIterator);
			return 0;
		case 'a': addAirport(airports, &airportIterator);
			break;
		case 'l': listAirports(airports, airportIterator);
			break;
		case 'v': addListflights(airports, airportIterator, flights,
					&flightIterator, currentDate);
			break;
		case 'p': listDepartingFlights(airports, airportIterator, flights,
					flightIterator);
			break;
		case 'c': listArrivingFlights(airports, airportIterator, flights,
					flightIterator);
			break;
		case 't': changeDate(&currentDate);
			break;
		case 'r': addListReservations(reservationTable, flights, flightIterator,
					 currentDate);
			break;
		case 'e': 
			break;
		}
	}

	return 0;
}