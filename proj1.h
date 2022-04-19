#ifndef PROJ1_H_
#define PROJ1_H_

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
#define SYSTEM_MONTH 100		/* um mes no sistema */
#define SYSTEM_DAY 1		/* um dia no sistema */
#define SYSTEM_HOUR 100	/* uma time no sistema */
#define SYSTEM_MINUTE 1	/* um minuto no sistema */

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

/* Funcoes leitura */

int readTime();
int readDate();
int readNextWord(char str[]);
void readWordUntilEndOfLine(char str[]);
void readUntilEndOfLine();

/* Funcoes data e hora */

void showDate(int d);
void showTime(int h);
int convertDateAndTimeIntoNum(int d, int h);
void addTimeToDateAndTime(Flight f, int *d, int *t);
int validateDate(int d, int currentDate);
int validateTime(int h);
void changeDate(int *currentDate);

/* Funcoes sort */

int cmpAirports(int a, int b, Airport airports[]);
int cmpArrivingFlights(int a, int b, Flight flights[]);
int cmpDepartingFlights(int a, int b, Flight flights[]);
void bubbleSortAirports(int indexes[], int size, Airport airports[]);
void bubbleSortFlights(int indexes[], int size, Flight flights[], int isDeparting);

/* Funcoes Airports */

int invalidAirportID(char id[]);
int findAirport(char id[], Airport airports[], int airportIterator);
void addAirport(Airport airports[], int *airportIterator);
void showAirport(int index, Airport airports[]);
int cmpAirports(int a, int b, Airport airports[]);
void listAllAirports(Airport airports[], int airportIterator);
void listAirports(Airport airports[], int airportIterator);

/* Funcoes Flights */

void showFlight(Flight flight);
void showDepartingFlight(int index, Flight flights[]);
void showArrivingFlight(int index, Flight flights[]);
int findFlight(char id[], int d, Flight flights[], int flightIterator);
int validateFlightID(char id[]);
int validateFlight(Flight v, Flight flights[], int flightIterator, Airport airports[], int airportIterator, int currentDate);
void createFlight(Flight v, Flight flights[], int *flightIterator);
void addListflights(Airport airports[], int airportIterator, Flight flights[], int *flightIterator, int currentDate);
int cmpDepartingFlights(int a, int b, Flight flights[]);
int cmpArrivingFlights(int a, int b, Flight flights[]);
void listDepartingFlights(Airport airports[], int airportIterator, Flight flights[], int flightIterator);
void listArrivingFlights(Airport airports[], int airportIterator, Flight flights[], int flightIterator);

#endif