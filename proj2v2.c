#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMAND_SIZE 65535 /* número máximo de caracteres num commando */
#define REALLOC_INTERVAL 10

#define MAX_AIRPORTS 40	/* número máximo de areoportos */
#define MAX_FLIGHTS 30000	/* número máximo de Flights */

#define MAX_AIRPORT_ID 4	/* dimensão do código do Airport */
#define MAX_AIRPORT_COUNTRY 31	/* dimensão do nome do country */
#define MAX_AIRPORT_CITY 51	/* dimensão do nome da city */

#define MAX_FLIGHT_ID 7	/* dimensão do código do Flight */
#define MAX_DATE 11		/* dimensão da date */
#define MAX_TIME 6		/* dimensão da time */

#define ERROR -1		/* código de erro */

#define SYSTEM_YEAR 10000	/* um ano no sistema */
#define SYSTEM_MONTH 100	/* um mes no sistema */
#define SYSTEM_DAY 1		/* um dia no sistema */
#define SYSTEM_HOUR 100		/* uma hora no sistema */
#define SYSTEM_MINUTE 1		/* um minuto no sistema */

#define TRUE 1			/* verdadeiro */
#define FALSE 0			/* falso */

/* Tipos de Dados */

typedef struct {
	char id[MAX_AIRPORT_ID];
	char country[MAX_AIRPORT_COUNTRY];
	char city[MAX_AIRPORT_CITY];
	int nFlights;
} Airport;

typedef struct {
	char id[MAX_FLIGHT_ID];
	char partida[MAX_AIRPORT_ID];
	char chegada[MAX_AIRPORT_ID];
	int date;
	int time;
	int arrivalDate;
	int arrivalTime;
	int duration;
	int capacity;
	int occupied;
} Flight;

typedef struct {
	char *id;
	int nPassengers;
	char flightID[MAX_FLIGHT_ID];
	int flightDate;
} Reservation;

/* Funcoes Leitura */

int readTime() {
	int temph, tempm, h;
	scanf("%d:%d", &temph, &tempm);
	h = temph*SYSTEM_HOUR + tempm*SYSTEM_MINUTE;
	return h;
}


int readDate() {
	int tempy, tempm, tempd, d;
	scanf("%d-%d-%d", &tempd, &tempm, &tempy);
	d = tempy*SYSTEM_YEAR + tempm*SYSTEM_MONTH + tempd*SYSTEM_DAY;
	return d;
}


int readNextWord(char str[]) {
	char c = getchar();
	int i = 0;
	while (c == ' ' || c == '\t')
		c = getchar();
	while (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
	return (c == '\n' || c == '\r');
}


void readWordUntilEndOfLine(char str[]) {
	char c = getchar();
	int i = 0;
	while (c == ' ' || c == '\t')
		c = getchar();
	while (c != '\n' && c != '\r') {
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
}


void readUntilEndOfLine() {
	char c = getchar();
	while (c != '\n' && c != '\r')
		c = getchar();
}

/* Funcoes malloc e realloc */

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

/* Funcoes dates e times */

void showDate(int d) {
	int tempy = d/SYSTEM_YEAR;
	int tempm = (d-(tempy*SYSTEM_YEAR))/SYSTEM_MONTH;
	int tempd = d-(tempy*SYSTEM_YEAR)-(tempm*SYSTEM_MONTH);
	if (tempd < 10)
		printf("0");
	printf("%d-", tempd);
	if (tempm < 10)
		printf("0");
	printf("%d-%d", tempm, tempy);
}


void showTime(int h) {
	int temph = h/SYSTEM_HOUR;
	int tempm = h-(temph*SYSTEM_HOUR);
	if (temph < 10)
		printf("0");
	printf("%d:", temph);
	if (tempm < 10)
		printf("0");
	printf("%d", tempm);
}


int convertDateAndTimeIntoNum(int d, int h) {
	return d * 10000 + h;
}


void addTimeToDateAndTime(Flight f, int *d, int *t) {
	int day = f.date%SYSTEM_HOUR;
	int month = ((f.date - day)%SYSTEM_YEAR)/SYSTEM_MONTH;
	int year = f.date/SYSTEM_YEAR;
	int hour = f.time/SYSTEM_HOUR + f.duration/SYSTEM_HOUR;
	int minute = f.time%SYSTEM_HOUR + f.duration%SYSTEM_HOUR;
	int _diasMes[] =	/* dias por mês (jan=1) */
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if(minute >= 60) {
		minute -= 60; 
		hour++;
	}
	if(hour >= 24) {
		hour -= 24;
		day++;
	}
	if(day > _diasMes[month-1]) {
		day -= _diasMes[month-1];
		month++;
	}
	if(month > 12) {
		month -= 12;
		year++;
	}

	*d = year*SYSTEM_YEAR + month*SYSTEM_MONTH + day*SYSTEM_DAY;
	*t = hour*SYSTEM_HOUR + minute*SYSTEM_MINUTE;
}


int validateDate(int d, int currentDate) {
	int proximoAno = currentDate + SYSTEM_YEAR;
	return !(d < currentDate || d > proximoAno);
}


int validateTime(int h) {
	int temph = h/SYSTEM_HOUR;
	int tempm = h-(temph*SYSTEM_HOUR);
	return !(temph > 12 || (temph == 12 && tempm > 0));
}


void changeDate(int *currentDate) {
	int d;

	d = readDate();
	readUntilEndOfLine();
	if (validateDate(d, *currentDate) == FALSE)
		printf("invalid date\r\n");
	else {
		*currentDate = d;
		showDate(*currentDate);
		printf("\r\n");
	}
}

/* Algoritmo de ordenação BubbleSort */

int cmpAirports(int a, int b, Airport airports[]);
int cmpArrivingFlights(int a, int b, Flight flights[]);
int cmpDepartingFlights(int a, int b, Flight flights[]);
int cmpReservations(Reservation a, Reservation b);


void bubbleSortAirports(int indexes[], int size, Airport airports[]) {
	int i, j, done;

	for (i = 0; i < size - 1; i++) {
		done = 1;
		for (j = size - 1; j > i; j--)
			if (cmpAirports(indexes[j - 1], indexes[j], airports)) {
				int aux = indexes[j];
				indexes[j] = indexes[j - 1];
				indexes[j - 1] = aux;
				done = 0;
			}
		if (done)
			break;
	}
}


void bubbleSortFlights(int indexes[], int size, Flight flights[], int isDeparting) {
	int i, j, done;

	for (i = 0; i < size - 1; i++) {
		done = 1;
		for (j = size - 1; j > i; j--)
			if(isDeparting) {
				if (cmpDepartingFlights(indexes[j - 1], indexes[j], flights)) {
					int aux = indexes[j];
					indexes[j] = indexes[j - 1];
					indexes[j - 1] = aux;
					done = 0;
				}
			} else {
				if (cmpArrivingFlights(indexes[j - 1], indexes[j], flights)) {
					int aux = indexes[j];
					indexes[j] = indexes[j - 1];
					indexes[j - 1] = aux;
					done = 0;
				}
			}
		if (done)
			break;
	}
}


Reservation** bubbleSortReservations(Reservation **tempReservations, int size) {
	int i, j, done;
	Reservation *aux = malloc(sizeof(Reservation));

	for (i = 0; i < size - 1; i++) {
		done = 1;
		for (j = size - 1; j > i; j--)
			if (cmpReservations(*tempReservations[j - 1], *tempReservations[j])) {
				*aux = *tempReservations[j];
				*tempReservations[j] = *tempReservations[j - 1];
				*tempReservations[j - 1] = *aux;
				done = 0;
			}
		if (done)
			break;
	}
	free(aux);
	return tempReservations;
}

/* Funcoes Airports */

int invalidAirportID(char id[]) {
	int i;
	for (i = 0; id[i] != '\0'; i++)
		if (!(id[i] >= 'A' && id[i] <= 'Z'))
			return TRUE;
	return FALSE;
}


int findAirport(char id[], Airport airports[], int airportIterator) {
	int i;
	for (i = 0; i < airportIterator; i++)
		if (!strcmp(id, airports[i].id))
			return i;
	return ERROR;
}


void addAirport(Airport airports[], int *airportIterator) {
	Airport a;

	readNextWord(a.id);
	readNextWord(a.country);
	readWordUntilEndOfLine(a.city);

	if (invalidAirportID(a.id))
		printf("invalid airport ID\r\n");
	else if (*airportIterator == MAX_AIRPORTS)
		printf("too many airports\r\n");
	else if (findAirport(a.id, airports, *airportIterator) != ERROR)
		printf("duplicate airport\r\n");
	else {
		strcpy(airports[*airportIterator].id, a.id);
		strcpy(airports[*airportIterator].country, a.country);
		strcpy(airports[*airportIterator].city, a.city);
		airports[*airportIterator].nFlights = 0;
		*airportIterator += 1;
		printf("airport %s\r\n", a.id);
	}
}


void showAirport(int index, Airport airports[]) {
	printf("%s %s %s %d\r\n", airports[index].id,
		airports[index].city, airports[index].country,
		airports[index].nFlights);
}


int cmpAirports(int a, int b, Airport airports[]) {
	return (strcmp(airports[a].id, airports[b].id) > 0);
}


void listAllAirports(Airport airports[], int airportIterator) {
	int i;
	int indexAirports[MAX_AIRPORTS];

	for (i = 0; i < airportIterator; i++)
		indexAirports[i] = i;

	bubbleSortAirports(indexAirports, airportIterator, airports);

	for (i = 0; i < airportIterator; i++)
		showAirport(indexAirports[i], airports);
}


void listAirports(Airport airports[], int airportIterator) {
	char id[MAX_AIRPORT_ID];
	int indexAirport, ultima = 0;

	ultima = readNextWord(id);
	if (strlen(id) == 0)
		listAllAirports(airports, airportIterator);
	else {
		while (strlen(id) != 0) {
			indexAirport = findAirport(id, airports, airportIterator);
			if (indexAirport == ERROR)
				printf("%s: no such airport ID\r\n", id);
			else
				showAirport(indexAirport, airports);
			if (!ultima)
				ultima = readNextWord(id);
			else
				break;
		}
	}
}

/* Funcoes Flights */

void showFlight(Flight flight) {
	printf("%s %s %s ", flight.id, flight.partida,
		flight.chegada);
	showDate(flight.date);
	printf(" ");
	showTime(flight.time);
	printf("\r\n");
}


void showDepartingFlight(int index, Flight flights[]) {
	printf("%s %s ", flights[index].id, flights[index].chegada);
	showDate(flights[index].date);
	printf(" ");
	showTime(flights[index].time);
	printf("\r\n");
}


void showArrivingFlight(int index, Flight flights[]) {
	printf("%s %s ", flights[index].id, flights[index].partida);
	showDate(flights[index].arrivalDate);
	printf(" ");
	showTime(flights[index].arrivalTime);
	printf("\r\n");
}


int findFlight(char id[], int d, Flight flights[], int flightIterator) {
	int i;
	for (i = 0; i < flightIterator; i++)
		if (!strcmp(id, flights[i].id)
			&& d == flights[i].date)
			return i;
	return ERROR;
}


int validateFlightID(char id[]) {
	int i = 0, l = strlen(id);
	if (l < 3)
		return FALSE;
	for (i = 0; i < 2; i++)
		if (!(id[i] >= 'A' && id[i] <= 'Z'))
			return FALSE;

	while (id[i] != '\0') {
		if (!(id[i] >= '0' && id[i] <= '9'))
			return FALSE;
		i++;
	}
	return TRUE;
}


int validateFlight(Flight v, Flight flights[], int flightIterator, Airport airports[], int airportIterator, int currentDate) {
	if (validateFlightID(v.id) == FALSE)
		printf("invalid flight code\r\n");
	else if (findFlight(v.id, v.date, flights, flightIterator) != ERROR)
		printf("flight already exists\r\n");
	else if (findAirport(v.partida, airports, airportIterator) == ERROR)
		printf("%s: no such airport ID\r\n", v.partida);
	else if (findAirport(v.chegada, airports, airportIterator) == ERROR)
		printf("%s: no such airport ID\r\n", v.chegada);
	else if (flightIterator == MAX_FLIGHTS)
		printf("too many flights\r\n");
	else if (validateDate(v.date, currentDate) == FALSE)
		printf("invalid date\r\n");
	else if (validateTime(v.duration) == FALSE)
		printf("invalid duration\r\n");
	else if (v.capacity < 10)
		printf("invalid capacity\r\n");
	else
		return TRUE;
	return FALSE;
}


void createFlight(Flight v, Flight flights[], int *flightIterator) {
	int arrivalDate, arrivalTime;
	addTimeToDateAndTime(v, &arrivalDate, &arrivalTime);

	strcpy(flights[*flightIterator].id, v.id);
	strcpy(flights[*flightIterator].partida, v.partida);
	strcpy(flights[*flightIterator].chegada, v.chegada);
	flights[*flightIterator].date = v.date;
	flights[*flightIterator].time = v.time;
	flights[*flightIterator].duration = v.duration;
	flights[*flightIterator].capacity = v.capacity;
	flights[*flightIterator].arrivalDate = arrivalDate;
	flights[*flightIterator].arrivalTime = arrivalTime;
	flights[*flightIterator].occupied = 0;
	*flightIterator += 1;
}


void addListflights(Airport airports[], int airportIterator, Flight flights[], int *flightIterator, int currentDate) {
	Flight v;
	int i;

	if (readNextWord(v.id)) {
		for (i = 0; i < *flightIterator; i++)
			showFlight(flights[i]);
		return;
	} else {
		readNextWord(v.partida);
		readNextWord(v.chegada);
		v.date = readDate();
		v.time = readTime();
		v.duration = readTime();
		scanf("%d", &v.capacity);
		readUntilEndOfLine();
	}

	if (validateFlight(v, flights, *flightIterator, airports, airportIterator, currentDate))
		createFlight(v, flights, flightIterator);
}


int cmpDepartingFlights(int a, int b, Flight flights[]) {
	int tempA = convertDateAndTimeIntoNum(flights[a].date, flights[a].time);
	int tempB = convertDateAndTimeIntoNum(flights[b].date, flights[b].time);
	return (tempA > tempB);
}


int cmpArrivingFlights(int a, int b, Flight flights[]) {
	int tempA = convertDateAndTimeIntoNum(flights[a].arrivalDate, flights[a].arrivalTime);
	int tempB = convertDateAndTimeIntoNum(flights[b].arrivalDate, flights[b].arrivalTime);
	return (tempA > tempB);
}


void listDepartingFlights(Airport airports[], int airportIterator, Flight flights[], int flightIterator) {
	int indexFlights[MAX_FLIGHTS], i, n = 0, isDeparting = 1;
	char partida[MAX_AIRPORT_ID];

	readWordUntilEndOfLine(partida);

	if (findAirport(partida, airports, airportIterator) == ERROR)
		printf("%s: no such airport ID\r\n", partida);

	for (i = 0; i < flightIterator; i++) {
		if (!strcmp(flights[i].partida, partida))
			indexFlights[n++] = i;
	}

	bubbleSortFlights(indexFlights, n, flights, isDeparting);

	for (i = 0; i < n; i++)
		showDepartingFlight(indexFlights[i], flights);
}


void listArrivingFlights(Airport airports[], int airportIterator, Flight flights[], int flightIterator) {
	int indexFlights[MAX_FLIGHTS], i, n = 0, isNotDeparting = 0;
	char chegada[MAX_AIRPORT_ID];

	readWordUntilEndOfLine(chegada);

	if (findAirport(chegada, airports, airportIterator) == ERROR)
		printf("%s: no such airport ID\r\n", chegada);

	for (i = 0; i < flightIterator; i++) {
		if (!strcmp(flights[i].chegada, chegada))
			indexFlights[n++] = i;
	}

	bubbleSortFlights(indexFlights, n, flights, isNotDeparting);

	for (i = 0; i < n; i++)
		showArrivingFlight(indexFlights[i], flights);
}

/* Funcoes reservas */

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


int findReservation(char id[], Reservation **reservations, int reservationIterator) {
	int i = 0;

	while(i < reservationIterator) {
		if(strcmp(id, reservations[i]->id) == 0)
			return i;
		i++;
	}
	return ERROR;
}


Reservation** findReservationFromFlight(Reservation **tempReservations, char id[], int d, int *size, Reservation **reservations, int reservationIterator) {
	int i = 0;

	while(i < reservationIterator) {
		if((strcmp(id, reservations[i]->flightID) == 0) && (d == reservations[i]->flightDate)) {
			if(*size%REALLOC_INTERVAL == 0) 
				tempReservations = cRealloc(tempReservations, (*size+REALLOC_INTERVAL) * sizeof(Reservation));
			tempReservations[*size] = malloc(sizeof(Reservation));
			tempReservations[*size]->id = cMalloc((strlen(reservations[i]->id)+1));
			
			strcpy(tempReservations[*size]->id, reservations[i]->id);
			tempReservations[*size]->nPassengers = reservations[i]->nPassengers;
			strcpy(tempReservations[*size]->flightID, reservations[i]->flightID);
			tempReservations[*size]->flightDate = reservations[i]->flightDate;
			*size += 1;
		}
		i++;
	}

	return tempReservations;
}


int validateReservation(Reservation r, Reservation **reservations, int reservationIterator, Flight flights[], int flightIterator, int currentDate) {
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


int validateListReservation(char id[], int d, Flight flights[], int flightIterator, int currentDate) {
	if (findFlight(id, d, flights, flightIterator) == ERROR)
		printf("%s: flight does not exist\r\n", id);
	else if(validateDate(d, currentDate) == FALSE)
		printf("invalid date\r\n");
	else
		return TRUE;
	return FALSE;
}


Reservation** createReservation(Reservation r, Reservation **reservations, int *reservationIterator) {
	if(*reservationIterator%REALLOC_INTERVAL == 0) 
		reservations = cRealloc(reservations, (*reservationIterator+REALLOC_INTERVAL) * sizeof(Reservation));
	reservations[*reservationIterator] = malloc(sizeof(Reservation));
	reservations[*reservationIterator]->id = cMalloc((strlen(r.id)+1));

	strcpy(reservations[*reservationIterator]->id, r.id);
	reservations[*reservationIterator]->nPassengers = r.nPassengers;
	strcpy(reservations[*reservationIterator]->flightID, r.flightID);
	reservations[*reservationIterator]->flightDate = r.flightDate;
	*reservationIterator += 1;

	return reservations;
}


void listReservations(char id[], int d, Reservation **reservations, int reservationIterator) {
	Reservation **tempReservations = cMalloc(sizeof(Reservation));
	int size = 0, i = 0;

	tempReservations = findReservationFromFlight(tempReservations, id, d, &size, reservations, reservationIterator);
	tempReservations = bubbleSortReservations(tempReservations, size);

	while(i < size) {
		printf("%s %d\r\n", tempReservations[i]->id, tempReservations[i]->nPassengers);
		i++;
	}

	for(i = 0; i < size; i++) {
		free(tempReservations[i]->id);
		free(tempReservations[i]);
	}
	free(tempReservations);
}


Reservation** addListReservations(Reservation **reservations, int *reservationIterator, Flight flights[], int flightIterator, int currentDate) {
	Reservation r;
	char c, temp[MAX_COMMAND_SIZE+1];
	readNextWord(r.flightID);
	r.flightDate = readDate();

	if((c = getchar()) == '\r' || c == '\n') {
		if(validateListReservation(r.flightID, r.flightDate, flights, flightIterator, currentDate)) {
			listReservations(r.flightID, r.flightDate, reservations, *reservationIterator);
		}
	} else {
		readNextWord(temp);
		r.id = cMalloc(strlen(temp)+1);
		strcpy(r.id, temp);
		scanf("%d", &r.nPassengers);
		if(validateReservation(r, reservations, *reservationIterator, flights, flightIterator, currentDate)) {
			reservations = createReservation(r, reservations, reservationIterator);
			flights[findFlight(r.flightID, r.flightDate, flights, flightIterator)].occupied += r.nPassengers;
		}
		free(r.id);
	}

	return reservations;
}


void removeReservation(int index, int *amount, Reservation **reservations, int *reservationIterator, Flight flights[], int flightIterator) {
	int i, flightIndex;

	flightIndex = findFlight(reservations[index]->flightID, reservations[index]->flightDate, flights, flightIterator);
	flights[flightIndex].occupied -= reservations[index]->nPassengers;

	for(i = index+1; i < *reservationIterator; i++) {
		reservations[i-1] = reservations[i];
	}
	free(reservations[i-1]);
	*amount += 1;
	*reservationIterator -= *amount;
	reservations = cRealloc(reservations, (*reservationIterator+1) * sizeof(Reservation));
}


void removeFlight(char id[], Flight *flights, int *flightIterator, int *flightAmount, Reservation **reservations, int *reservationIterator, int *reservationAmount) {
	int i = 0;
	while(i < *flightIterator) {
		if(strcmp(flights[i].id, id) == 0) {
			*flightAmount += 1;
		} else {
			flights[i-*flightAmount] = flights[i];
		}
		i++;
	}
	*flightIterator -= *flightAmount;

	i = 0;
	while(i < *reservationIterator) {
		if(strcmp(reservations[i]->flightID, id) == 0) {
			*reservationAmount += 1;
		} else {
			reservations[i-*reservationAmount] = reservations[i];
		}
		i++;
	}
	for(i = *reservationIterator-*reservationAmount; i < *reservationAmount; i++) {
		free(reservations[i]->id);
		free(reservations[i]);
	}
	*reservationIterator -= *reservationAmount;
	reservations = cRealloc(reservations, (*reservationIterator + 1)*sizeof(Reservation));
}


void removeFlightsReservation(Reservation **reservations, int *reservationIterator, Flight flights[], int *flightIterator) {
	char temp[MAX_COMMAND_SIZE+1], *id;
	int index; 
	int flightAmount = 0;
	int reservationAmount = 0;

	readNextWord(temp); /* can be optimized */
	id = cMalloc(strlen(temp)+1);
	strcpy(id, temp);

	if((index = findReservation(id, reservations, *reservationIterator)) != ERROR) {
		removeReservation(index, &reservationAmount, reservations, reservationIterator, flights, *flightIterator);
		free(id);
		return;
	} 

	removeFlight(id, flights, flightIterator, &flightAmount, reservations, reservationIterator, &reservationAmount);
	free(id);

	if(reservationAmount == 0 && flightAmount == 0)
		printf("not found\r\n");

}


void destroy(Reservation **reservations, int reservationIterator) {
	int i = 1;
	while(i<reservationIterator) {
		free(reservations[i]->id);
		free(reservations[i]);
		i++;
	}
	free(reservations);
}


int main() {
	int c;
	int airportIterator = 0;		/* número de Airports introduzidos */
	Airport airports[MAX_AIRPORTS];	/* vetor de Airports */
	int flightIterator = 0;			/* número de Flights introduzidos */
	Flight flights[MAX_FLIGHTS];	/* vetor de Flights */
	int reservationIterator = 0;	/* número de Reservations introduzidos */
	Reservation **reservations = cMalloc(sizeof(Reservation));
	int currentDate = 20220101;		/* date atual do sistema */

	while ((c = getchar()) != EOF) {
		switch (c) {
		case 'q': destroy(reservations, reservationIterator);
			return 0;
		case 'a': addAirport(airports, &airportIterator);
			break;
		case 'l': listAirports(airports, airportIterator);
			break;
		case 'v': addListflights(airports, airportIterator, flights, &flightIterator, currentDate);
			break;
		case 'p': listDepartingFlights(airports, airportIterator, flights, flightIterator);
			break;
		case 'c': listArrivingFlights(airports, airportIterator, flights, flightIterator);
			break;
		case 't': changeDate(&currentDate);
			break;
		case 'r': reservations = addListReservations(reservations, &reservationIterator, flights, flightIterator, currentDate);
			break;
		case 'e': removeFlightsReservation(reservations, &reservationIterator, flights, &flightIterator);
			break;
		}
	}

	return 0;
}
