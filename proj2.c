/*
 * File: proj2.c
 * Author:  João Paulo Borges Pereira 102081
 * Description: Second project for the IAED subject, 2021/2022.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "proj1.h"

#define MAX_COMMAND_SIZE 65535 	/* max number of characters in a single command */
#define REALLOC_INTERVAL 10		/* interval between reallocs */

/* Data types */

typedef struct {
	char *id;
	int nPassengers;
	char flightID[MAX_FLIGHT_ID];
	int flightDate;
} Reservation;

/* Malloc and realloc functions */

void* cMalloc(int size) {
	void *ptr = malloc(size);
	if(ptr == NULL) {
		printf("No memory.");
		exit(0);
	}
	return ptr;
}


void* cRealloc(void* ptr, int size) {
	ptr = realloc(ptr, size);
	if(ptr == NULL) {
		printf("No memory.");
		exit(0);
	}
	return ptr;
}

/* Sorting algorithm BubbleSort */

int cmpReservations(Reservation a, Reservation b);


Reservation* bubbleSortReservations(Reservation *tempReservations, int size) {
	int i, j, done;
	Reservation aux;

	for (i = 0; i < size - 1; i++) {
		done = 1;
		for (j = size - 1; j > i; j--)
			if (cmpReservations(tempReservations[j - 1], tempReservations[j])) {
				aux = tempReservations[j];
				tempReservations[j] = tempReservations[j - 1];
				tempReservations[j - 1] = aux;
				done = 0;
			}
		if (done)
			break;
	}
	return tempReservations;
}

/* Reservation functions */

int cmpReservations(Reservation a, Reservation b) {
	return (strcmp(a.id, b.id) > 0);
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


int findReservation(char id[], Reservation *reservations, int reservationIterator) {
	int i = 0;

	while(i < reservationIterator) {
		if(strcmp(id, reservations[i].id) == 0)
			return i;
		i++;
	}
	return ERROR;
}


Reservation* findReservationFromFlight(Reservation *tempReservations, char id[],
 int d, int *size, Reservation reservations[], int reservationIterator) {
	int i = 0;

	while(i < reservationIterator) {
		if((strcmp(id, reservations[i].flightID) == 0) &&
		 (d == reservations[i].flightDate)) {
			if(*size%REALLOC_INTERVAL == 0) 
				tempReservations = cRealloc(tempReservations, (*size+REALLOC_INTERVAL)
				 * sizeof(Reservation));

			tempReservations[*size].id = cMalloc((strlen(reservations[i].id)+1));
			strcpy(tempReservations[*size].id, reservations[i].id);
			tempReservations[*size].nPassengers = reservations[i].nPassengers;
			strcpy(tempReservations[*size].flightID, reservations[i].flightID);
			tempReservations[*size].flightDate = reservations[i].flightDate;
			*size += 1;
		}
		i++;
	}

	return tempReservations;
}


int validateReservation(Reservation r, Reservation *reservations,
 int reservationIterator, Flight flights[], int flightIterator, int currentDate) {
	int flightIndex = findFlight(r.flightID, r.flightDate, flights, flightIterator);

	if(validateReservationID(r.id) == FALSE)
		printf("invalid reservation code\r\n");
	else if (flightIndex == ERROR)
		printf("%s: flight does not exist\r\n", r.flightID);
	else if(findReservation(r.id, reservations, reservationIterator) != ERROR)
		printf("%s: flight reservation already used\r\n", r.id);
	else if(flights[flightIndex].capacity - flights[flightIndex].occupied < r.nPassengers)
		printf("too many reservations\r\n");
	else if(validateDate(r.flightDate, currentDate) == FALSE)
		printf("invalid date\r\n");
	else if(r.nPassengers < 1)
		printf("invalid passenger number\r\n");
	else
		return TRUE;
	return FALSE;
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


Reservation* createReservation(Reservation r, Reservation *reservations,
 int *reservationIterator) {
	if(*reservationIterator%REALLOC_INTERVAL == 0) 
		reservations = cRealloc(reservations, (*reservationIterator+REALLOC_INTERVAL)
		 * sizeof(Reservation));
	reservations[*reservationIterator].id = cMalloc((strlen(r.id)+1));

	strcpy(reservations[*reservationIterator].id, r.id);
	reservations[*reservationIterator].nPassengers = r.nPassengers;
	strcpy(reservations[*reservationIterator].flightID, r.flightID);
	reservations[*reservationIterator].flightDate = r.flightDate;
	*reservationIterator += 1;

	return reservations;
}


void listReservations(char id[], int d, Reservation *reservations,
 int reservationIterator) {
	Reservation *tempReservations = cMalloc(sizeof(Reservation));
	int size = 0, i = 0;

	tempReservations = findReservationFromFlight(tempReservations, id, d, &size,
	 reservations, reservationIterator);
	tempReservations = bubbleSortReservations(tempReservations, size);

	while(i < size) {
		printf("%s %d\r\n", tempReservations[i].id, tempReservations[i].nPassengers);
		i++;
	}

	for(i = 0; i < size; i++) {
		free(tempReservations[i].id);
	}
	free(tempReservations);
}


Reservation* addListReservations(Reservation *reservations,
 int *reservationIterator, Flight flights[], int flightIterator, int currentDate) {
	Reservation r;
	char c, temp[MAX_COMMAND_SIZE+1];
	readNextWord(r.flightID);
	r.flightDate = readDate();

	if((c = getchar()) == '\r' || c == '\n') {
		if(validateListReservation(r.flightID, r.flightDate, flights,
		 flightIterator, currentDate)) {
			listReservations(r.flightID, r.flightDate, reservations,
			 *reservationIterator);
		}
	} else {
		readNextWord(temp);
		r.id = cMalloc(strlen(temp)+1);
		strcpy(r.id, temp);
		scanf("%d", &r.nPassengers);
		if(validateReservation(r, reservations, *reservationIterator, flights,
		 flightIterator, currentDate)) {
			reservations = createReservation(r, reservations, reservationIterator);
			flights[findFlight(r.flightID, r.flightDate, flights,
			 flightIterator)].occupied += r.nPassengers;
		}
		free(r.id);
	}

	return reservations;
}


Reservation* removeReservation(int index, int *amount, Reservation *reservations,
 int *reservationIterator, Flight flights[], int flightIterator) {
	int i, flightIndex;

	flightIndex = findFlight(reservations[index].flightID,
	 reservations[index].flightDate, flights, flightIterator);
	flights[flightIndex].occupied -= reservations[index].nPassengers;
	for(i = index+1; i < *reservationIterator; i++) {
		reservations[i-1] = reservations[i];
	}
	*amount += 1;
	*reservationIterator -= *amount;
	reservations = cRealloc(reservations, (*reservationIterator+1) * sizeof(Reservation));
	return reservations;
}


Reservation* removeFlight(char id[], Flight *flights, int *flightIterator, int *flightRemoveAmount,
 Reservation *reservations, int *reservationIterator, int *reservationRemoveAmount) {
	int i = 0;
	while(i < *flightIterator) {
		if(strcmp(flights[i].id, id) == 0) {
			*flightRemoveAmount += 1;
		} else {
			flights[i-*flightRemoveAmount] = flights[i];
		}
		i++;
	}
	*flightIterator -= *flightRemoveAmount;

	i = 0;
	while(i < *reservationIterator) {
		if(strcmp(reservations[i].flightID, id) == 0) {
			*reservationRemoveAmount += 1;
		} else {
			reservations[i-*reservationRemoveAmount] = reservations[i];
		}
		i++;
	}
	*reservationIterator -= *reservationRemoveAmount;
	return reservations;
}


Reservation* removeFlightsReservation(Reservation *reservations, int *reservationIterator,
 Flight flights[], int *flightIterator) {
	char temp[MAX_COMMAND_SIZE+1], *id;
	int index; 
	int flightRemoveAmount = 0;
	int reservationRemoveAmount = 0;

	readNextWord(temp);
	id = cMalloc(strlen(temp)+1);
	strcpy(id, temp);

	if((index = findReservation(id, reservations, *reservationIterator)) != ERROR) {
		removeReservation(index, &reservationRemoveAmount, reservations, reservationIterator,
		 flights, *flightIterator);
		free(id);
		return reservations;
	} 

	removeFlight(id, flights, flightIterator, &flightRemoveAmount, reservations,
	 reservationIterator, &reservationRemoveAmount);
	free(id);

	if(reservationRemoveAmount == 0 && flightRemoveAmount == 0)
		printf("not found\r\n");

	return reservations;
}


void destroy(Reservation *reservations, int reservationIterator) {
	int i = 0;

	while(i<reservationIterator) {
		free(reservations[i].id);
		i++;
	}
	free(reservations);
}


int main() {
	int c;
	int airportIterator = 0;		/* number of Airports added */
	Airport airports[MAX_AIRPORTS];	/* Airports vector */
	int flightIterator = 0;			/* number of Flights added */
	Flight flights[MAX_FLIGHTS];	/* Flights vector */
	int reservationIterator = 0;	/* number of Reservations added */
	Reservation *reservations = cMalloc(sizeof(Reservation));	/* Rerservations vector */
	int currentDate = 20220101;		/* current date in system */
	
	while ((c = getchar()) != EOF) {
		switch (c) {
		case 'q': destroy(reservations, reservationIterator);
			return 0;
		case 'a': addAirport(airports, &airportIterator);
			break;
		case 'l': listAirports(airports, airportIterator);
			break;
		case 'v': addListflights(airports, airportIterator, flights, &flightIterator,
					currentDate);
			break;
		case 'p': listDepartingFlights(airports, airportIterator, flights, flightIterator);
			break;
		case 'c': listArrivingFlights(airports, airportIterator, flights,
					flightIterator);
			break;
		case 't': changeDate(&currentDate);
			break;
		case 'r': reservations = addListReservations(reservations, &reservationIterator, 
					flights, flightIterator, currentDate);
			break;
		case 'e': reservations = removeFlightsReservation(reservations, &reservationIterator,
					flights, &flightIterator);
			break;
		}
	}

	return 0;
}
