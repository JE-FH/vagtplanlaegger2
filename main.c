#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_NAME_LENGTH 50

enum Day {
	DAY_MONDAY,
	DAY_TUESDAY,
	DAY_WEDNESDAY,
	DAY_THURSDAY,
	DAY_FRIDAY,
	DAY_SATURDAY,
	DAY_SUNDAY,
	DAY_INVALID
};

enum Shift {
	SHIFT_NIGHT,
	SHIFT_DAY,
	SHIFT_EVENING,
	SHIFT_INVALID
};

struct Block {
	enum Day day;
	enum Shift shift;
};

struct Worker {
	char name[MAX_NAME_LENGTH + 1];
	enum Day desired_day_off;
	enum Shift desired_shift;
	unsigned int uuid;
};

struct BlockSchedule {
	struct Worker** workers;
};

struct Schedule {
	struct BlockSchedule blocks[21];
	double score;
};

struct RequiredWorkers {
	unsigned int night_workers;
	unsigned int day_workers;
	unsigned int evening_workers;
};

/**
 * Laver et schema til antallet af mearbejdere
 * @param[in] workers alle workers
 * @param[in] worker_count antallet af workers i workers arrayen
 * @param[in] required_workers en required workers struktur
 * @returns et skema over hvem der skal arbejde
 */
struct Schedule make_schedule(struct Worker* workers[], const size_t worker_count, const struct RequiredWorkers required_workers);

/**
 * Laver den første generation af individer
 * @param[in] required_workers the amount of required workers
 * @param[in] worker array af alle medarbejdere
 * @param[in] worker_count antallet af mebarbejdere
 * @param[in] population_size mængden af
 * @param[out] schedules der hvor populationen bliver gemt
 */
void generate_initial_population(struct RequiredWorkers required_workers, struct Worker* worker[], size_t worker_count, struct Schedule schedules[], unsigned int population_size);

/**
 * Laver et enkelt tilfældigt skema
 * @param[in] workers alle medarbejdere
 * @param[in] worker_count antallet af medarbejdere i workers
 * @param[in] required_workers antallet af medarbejdere der er brug for
 * @param[out] schedule der hvor det tilfældige skema bliver gemt
 */
void generate_random_schedule(struct Worker* workers[], const size_t worker_count, const struct RequiredWorkers required_workers, struct Schedule* schedule);


/**
 * Fitness funktionen for skemaer
 * @param[in] schedule vagtplanen som bliver evalueret
 * @param[in] required_workers mængden af medarbejdere der er brug for
 * @returns en værdi som siger hvor god planen er, nu højere nu bedre
 */
double evaluate_schedule(struct Schedule* schedule, const struct RequiredWorkers required_workers);


/**
 * Læser Workers ind fra den givne file pointer
 * @param[in, out] file en åben fil
 * @param[out] antallet af workers som blev indlæst fra filens
 * @returns en array som er worker_count stor af Worker
 */
struct Worker* read_workers(FILE* file, size_t* worker_count);

/**
 * Skriver schedule ud i en fil
 * @param[in, out] file en åben fil hvor schedule skal skrives til
 * @param[in] schedule skemaet som skal skrives
 */
void write_schedule(FILE* file, const struct Schedule* schedule);

/**
 * Læser en schedule som er gemt i en fil
 * @param[in, out] file en åben fil hvor schedule skal læses fra
 * @return det indlæste skema
 */
struct Schedule read_schedule(FILE* file);

/**
 * skriver et pænt skema for en enkelt person til en fil
 * @param[in, out] file en åben fil hvor skemaet skal skrives til
 * @param[in] worker den medarbejder skemaet skal printes til
 * @param[in] skemaet som er lavet for medarbejderen
 */
void print_worker_schedule(FILE* file, const struct Worker* worker, const struct Schedule* schedule);

void fatal_error(const char* reason);

enum Shift string_to_shift(char* input);

enum Day string_to_day(char* input);

unsigned int get_required_workers(struct RequiredWorkers required_workers, enum Shift shift);

int compare_schedule(const void* a, const void* b);

void combine_schedule(const struct Schedule* a, const struct Schedule* b, struct Schedule* out);

int random_number(int min, int max);

int get_required_for_shift(struct RequiredWorkers required_workers, enum Shift shift);

int main(int argc, char** argv) {
	FILE* fil = fopen("input.csv", "r");
	struct Worker* workers_direct;
	size_t worker_count = 0;
	struct Schedule schedule;
	struct RequiredWorkers required_workers;
	struct Worker **workers;
	size_t i = 0;

	srand(time(NULL));
	printf("Starter programmet\n");
	required_workers.night_workers = 40;
	required_workers.day_workers = 80;
	required_workers.evening_workers = 80;

	if (fil == NULL) {
		fatal_error("Kunne ikke åbne input csv filen");
	}

	workers_direct = read_workers(fil, &worker_count);

	workers = malloc(worker_count * sizeof(struct Worker*));
	if (workers == NULL) {
		fatal_error("ikke nok hukkomelse");
	}
	for (i = 0; i < worker_count; i++) {
		workers[i] = &workers_direct[i];
	}
	printf("Så laver vi et skema\n");

	schedule = make_schedule(workers, worker_count, required_workers);
	getchar();
	return 0;
}

struct Worker* read_workers(FILE* fil, size_t* worker_count) {
	size_t allocated_workers = 10;
	struct Worker* workers = malloc(allocated_workers * sizeof(struct Worker));

	if (workers == NULL) {
		fatal_error("Ikke mere hukommelse");
	}

	*worker_count = 0;

	while (!feof(fil)) {
		int res = 0;
		char shift_text[33];
		char day_text[33];
		if (*worker_count + 1 >= allocated_workers) {
			allocated_workers += 10;
			workers = realloc(workers, allocated_workers * sizeof(struct Worker));
			if (workers == NULL) {
				fatal_error("Ikke mere hukommelse");
			}
		}

		res = fscanf(fil,
			" %50[^;,] %*1[;,] %32[^;,] %*1[;,] %32[^;,] %*1[;,] %u%*[^\n]\n", 
			workers[*worker_count].name, day_text, shift_text, &workers[*worker_count].uuid
		);

		if (res != 4) {
			printf("Fejl i medarbejder list på linje %u og kolonne %d\n", *worker_count + 1, res);
			fatal_error(NULL);
		}

		shift_text[32] = 0;
		day_text[32] = 0;

		workers[*worker_count].desired_day_off = string_to_day(day_text);
		if (workers[*worker_count].desired_day_off == DAY_INVALID) {
			printf("Fejl i medarbejder list på linje %u ved dag: %s var invalid\n", *worker_count + 1, shift_text);
			fatal_error(NULL);
		}

		workers[*worker_count].desired_shift = string_to_shift(shift_text);
		if (workers[*worker_count].desired_shift == SHIFT_INVALID) {
			printf("Fejl i medarbejder list på linje %u ved vagt: %s var invalid\n", *worker_count + 1, day_text);
			fatal_error(NULL);
		}

		*worker_count += 1;
	}
	return realloc(workers, *worker_count * sizeof(struct Worker));
}

void fatal_error(const char* reason) {
	if (reason != NULL) {
		printf("Fatal fejl, programmet kan ikke fortsætte: %s\n", reason);
	} else {
		printf("Fatal fejl, programmet kan ikke fortsætte\n");
	}
	exit(EXIT_FAILURE);

}


enum Shift string_to_shift(char* input) {
	if (strcmp(input, "nat") == 0) {
		return SHIFT_NIGHT;
	} else if (strcmp(input, "dag") == 0) {
		return SHIFT_DAY;
	} else if (strcmp(input, "aften") == 0) {
		return SHIFT_EVENING;
	} else {
		return SHIFT_INVALID;
	}
}

enum Day string_to_day(char* input) {
	if (strcmp(input, "mandag") == 0) {
		return DAY_MONDAY;
	} else if (strcmp(input, "tirsdag") == 0) {
		return DAY_TUESDAY;
	} else if (strcmp(input, "onsdag") == 0) {
		return DAY_WEDNESDAY;
	} else if (strcmp(input, "torsdag") == 0) {
		return DAY_THURSDAY;
	} else if (strcmp(input, "fredag") == 0) {
		return DAY_FRIDAY;
	} else if (strcmp(input, "lørdag") == 0) {
		return DAY_SATURDAY;
	} else if (strcmp(input, "søndag") == 0) {
		return DAY_SUNDAY;
	} else {
		return DAY_INVALID;
	}
}

struct Schedule make_schedule(struct Worker *workers[], const size_t worker_count, const struct RequiredWorkers required_workers)
{

	struct Schedule *population = malloc(sizeof(struct Schedule) * 1000);
	int generation = 1;
	generate_initial_population(required_workers, workers, worker_count, population, 1000);

	while (1)
	{
		int i;
		for (i = 0; i < 1000; i++) {
			population[i].score = evaluate_schedule(&population[i], required_workers);
		}

		qsort(population, 1000, sizeof(struct Schedule), compare_schedule);
		printf("Generation nummer %d. Max fitness er %f\n", generation, population[0]);

		for (i = 0; i < 40; i++) {
			int random = random_number(40, 960);
			combine_schedule(&population[i], &population[random], &population[960 + i]);
		}
		generation++;
	}
}

void generate_initial_population(struct RequiredWorkers required_workers, struct Worker *worker[], size_t worker_count, struct Schedule schedules[], unsigned int population_size) {
	int i;
	for (i = 0; i < population_size; i++) {
		generate_random_schedule(worker, worker_count, required_workers, &schedules[i]);
	}
}

void generate_random_schedule(
	struct Worker *workers[],
	const size_t worker_count,
	const struct RequiredWorkers required_workers,
	struct Schedule *schedule)
{
	int day;
	for (day = 0; day < 7; day++) {
		int j = worker_count;
		int vagt;
		for (vagt = 0; vagt < 3; vagt++) {
			int required_workers_yep = get_required_for_shift(required_workers, (enum Shift) vagt);
			int b;
			schedule->blocks[day * 3 + vagt].workers = malloc(required_workers_yep * sizeof(struct Worker*));
			if (schedule->blocks[day * 3 + vagt].workers == NULL) {
				fatal_error("Hukkomelse er tom");
			}
			for (b = 0; b < required_workers_yep; b++) {
				if (j <= 0) {
					fatal_error("Not enough workers to fulfill a single day");
				}
				schedule->blocks[day * 3 + vagt].workers[b] = workers[random_number(0, j)];
				j--; 
			}
		}
	}
}

int get_required_for_shift(struct RequiredWorkers required_workers, enum Shift shift) {
	switch (shift)
	{
	case SHIFT_NIGHT:
		return required_workers.night_workers;
	case SHIFT_DAY:
		return required_workers.day_workers;
	case SHIFT_EVENING:
		return required_workers.evening_workers;
	default:
		fatal_error("fuck dig biyyyyytch");
	}
	return 0;
}

int random_number(int min, int max) {
	return (int) round(min + (((double)rand()) / RAND_MAX) * (max - min));
}

double evaluate_schedule(struct Schedule* schedule, const struct RequiredWorkers required_workers) {
	return 0;
}

int compare_schedule(const void* a, const void* b) {
	const struct Schedule* sa = a;
	const struct Schedule* sb = b;

	return sb->score - sa->score;
}

void combine_schedule(const struct Schedule* a, const struct Schedule* b, struct Schedule* out) {
	int crossover_start = random_number(0, 20);
	int crossover_end = random_number(crossover_start + 1, 21);

	const struct BlockSchedule* a_block = (const struct BlockSchedule*) a->blocks;
	const struct BlockSchedule* b_block = (const struct BlockSchedule*) b->blocks;
	struct BlockSchedule* out_block = (struct BlockSchedule*) out->blocks;

	int j;

	for (j = 0; j < 21; j++) {
		if (j >= crossover_start && j < crossover_end) {
			out_block[j].workers = b_block[j].workers;
		} else {
			out_block[j].workers = a_block[j].workers;
		}
	}
}