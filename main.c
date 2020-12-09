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

typedef struct Block {
	enum Day day;
	enum Shift shift;
} Block;

typedef struct Worker {
	char name[MAX_NAME_LENGTH + 1];
	enum Day desired_day_off;
	enum Shift desired_shift;
	int last_block; 
	unsigned int consecutive_night_shifts;
	bool day_off;
	unsigned int uuid;
} Worker;

typedef struct BlockSchedule {
	 Worker** workers;
} BlockSchedule;

typedef struct Schedule {
	BlockSchedule blocks[21];
	double score;
} Schedule;

typedef struct RequiredWorkers {
	unsigned int night_workers;
	unsigned int day_workers;
	unsigned int evening_workers;
} RequiredWorkers;

/**
 * Laver et schema til antallet af mearbejdere
 * @param[in] workers alle workers
 * @param[in] worker_count antallet af workers i workers arrayen
 * @param[in] required_workers en required workers struktur
 * @returns et skema over hvem der skal arbejde
 */
 Schedule make_schedule( Worker* workers[], const size_t worker_count, const  RequiredWorkers required_workers);

/**
 * Laver den første generation af individer
 * @param[in] required_workers the amount of required workers
 * @param[in] worker array af alle medarbejdere
 * @param[in] worker_count antallet af mebarbejdere
 * @param[in] population_size mængden af
 * @param[out] schedules der hvor populationen bliver gemt
 */
void generate_initial_population( RequiredWorkers required_workers,  Worker* worker[], size_t worker_count, Schedule* schedules, unsigned int population_size);

/**
 * Laver et enkelt tilfældigt skema
 * @param[in] workers alle medarbejdere
 * @param[in] worker_count antallet af medarbejdere i workers
 * @param[in] required_workers antallet af medarbejdere der er brug for
 * @param[out] schedule der hvor det tilfældige skema bliver gemt
 */
void generate_random_schedule( Worker* workers[], const size_t worker_count, const  RequiredWorkers required_workers,  Schedule* schedule);


/**
 * Fitness funktionen for skemaer
 * @param[in] schedule vagtplanen som bliver evalueret
 * @param[in] required_workers mængden af medarbejdere der er brug for
 * @returns en værdi som siger hvor god planen er, nu højere nu bedre
 */
double evaluate_schedule(Schedule* schedule, const RequiredWorkers required_workers, Worker* worker[], unsigned int amount_of_workers);


/**
 * Læser Workers ind fra den givne file pointer
 * @param[in, out] file en åben fil
 * @param[out] antallet af workers som blev indlæst fra filens
 * @returns en array som er worker_count stor af Worker
 */
 Worker* read_workers(FILE* file, size_t* worker_count);

/**
 * Skriver schedule ud i en fil
 * @param[in, out] file en åben fil hvor schedule skal skrives til
 * @param[in] schedule skemaet som skal skrives
 */
void write_schedule(FILE* file, const  Schedule* schedule, RequiredWorkers required_workers);

/**
 * Læser en schedule som er gemt i en fil
 * @param[in, out] file en åben fil hvor schedule skal læses fra
 * @param[out] out der hvor mængden af arbejdere hver dag bliver gemt
 * @return det indlæste skema
 */
Schedule read_schedule(FILE* file, RequiredWorkers* out, Worker** workers, size_t worker_count);

/**
 * skriver et pænt skema for en enkelt person til en fil
 * @param[in, out] file en åben fil hvor skemaet skal skrives til
 * @param[in] worker den medarbejder skemaet skal printes til
 * @param[in] skemaet som er lavet for medarbejderen
 */
void print_worker_schedule(FILE* file, const Worker* worker, const Schedule* schedule, RequiredWorkers required_workers);

void fatal_error(const char* reason);

void evaluate_schedule_required_workers( Schedule* schedule, const RequiredWorkers required_workers, unsigned int block_number, size_t j);

enum Shift string_to_shift(char* input);

enum Day string_to_day(char* input);

unsigned int get_required_workers(RequiredWorkers required_workers, enum Shift shift);

void evaluate_schedule_preferred_day_off(Schedule* schedule, Worker* worker[], size_t m, size_t amount_assigned_workers, Worker** assigned_workers, unsigned int amount_of_workers, unsigned int i);

int compare_schedule(const void* a, const void* b);

void combine_schedule(Worker* workers[], size_t worker_count, RequiredWorkers required_workers, const  Schedule* a, const  Schedule* b,  Schedule* out);

int random_number(int min, int max);

unsigned int get_required_for_shift(RequiredWorkers required_workers, enum Shift shift);

void set_required_for_shift(RequiredWorkers* required_workers, enum Shift shift, unsigned int val);

Worker* find_worker_from_uuid(Worker** workers, size_t worker_count, unsigned int uuid);

const char* get_shift_as_string(enum Shift shift);
const char* get_day_as_string(enum Day day);
const char* get_time_slot(enum Shift shift);

void test_vagtplan(const char* vagtplan_fil_navn);
void skab_vagtplan();
void print_vagtplaner(const char* vagtplan_fil_navn);

int main(int argc, char** argv) {
	if (argc >= 3) {
		if (strcmp(argv[1], "test") == 0) {
			test_vagtplan(argv[2]);
		} else if (strcmp(argv[1], "print") == 0) {
			print_vagtplaner(argv[2]);
		} else {
			printf("Forkert parameter, du kan bruge test eller print");
			return EXIT_FAILURE;
		}
	} else {
		skab_vagtplan();
	}
	return 0;
}

void test_vagtplan(const char* vagtplan_fil_navn) {
	FILE* fil = fopen("medarbejdere.csv", "r");
	RequiredWorkers required_workers;
	Worker* workers_direct;
	Worker** workers;
	size_t worker_count = 0;
	Schedule schedule;
	double score = 0;
	size_t i;
	if (fil == NULL) {
		fatal_error("Kunne ikke åbne medarbejdere.csv filen");
	}
	workers_direct = read_workers(fil, &worker_count);
	workers = malloc(worker_count * sizeof(struct Worker*));
	for (i = 0; i < worker_count; i++) {
		workers[i] = &workers_direct[i];
	}

	fclose(fil);
	fil = fopen(vagtplan_fil_navn, "r");
	if (fil == NULL) {
		fatal_error("Kunne ikke åbne vagtplan.csv filen");
	}
	
	schedule = read_schedule(fil, &required_workers, workers, worker_count);
	fclose(fil);

	score = evaluate_schedule(&schedule, required_workers, workers, worker_count);

	printf("Vagtplanen fik en score på %f", score);

	fil = fopen("vagtplan-kopi.csv", "w");
	
	write_schedule(fil, &schedule, required_workers);

	fclose(fil);
}

void skab_vagtplan() {
	FILE* fil = fopen("input.csv", "r");
	Worker* workers_direct;
	size_t worker_count = 0;
	Schedule schedule;
	RequiredWorkers required_workers;
	Worker **workers;
	size_t i = 0;
	
	srand(time(NULL));

	printf("Dit daglige tilfældige tal %d\n", random_number(0, 400));

	printf("Starter programmet\n");
	required_workers.night_workers = 2;
	required_workers.day_workers = 3;
	required_workers.evening_workers = 2;

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
}

void print_vagtplaner(const char* vagtplan_fil_navn) {
	FILE* fil = fopen("medarbejdere.csv", "r");
	RequiredWorkers required_workers;
	Worker* workers_direct;
	Worker** workers;
	size_t worker_count = 0;
	Schedule schedule;
	size_t i;
	size_t worker_i;
	char filnavn[100];

	if (fil == NULL) {
		fatal_error("Kunne ikke åbne medarbejdere.csv filen");
	}
	workers_direct = read_workers(fil, &worker_count);
	workers = malloc(worker_count * sizeof(struct Worker*));
	for (i = 0; i < worker_count; i++) {
		workers[i] = &workers_direct[i];
	}

	fclose(fil);
	fil = fopen(vagtplan_fil_navn, "r");
	if (fil == NULL) {
		fatal_error("Kunne ikke åbne vagtplan.csv filen");
	}
	
	schedule = read_schedule(fil, &required_workers, workers, worker_count);
	fclose(fil);


	for (worker_i = 0; worker_i < worker_count; worker_i++) {
		sprintf(filnavn, "output/%s.%u.txt", workers[worker_i]->name, workers[worker_i]->uuid);
		fil = fopen(filnavn, "w");
		if (fil == NULL) {
			fatal_error("Kunne ikke åbne output filen");
		}
		print_worker_schedule(fil, workers[worker_i], &schedule, required_workers);
		fclose(fil);
	}
}

Worker* read_workers(FILE* fil, size_t* worker_count) {
	size_t allocated_workers = 10;
	Worker* workers = malloc(allocated_workers * sizeof( Worker));

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
			workers = realloc(workers, allocated_workers * sizeof( Worker));
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
	return realloc(workers, *worker_count * sizeof( Worker));
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
 Schedule make_schedule(Worker* workers[], const size_t worker_count, const RequiredWorkers required_workers) {
	struct Schedule *population = malloc(sizeof(struct Schedule) * 1000);
	int generation = 1;
	generate_initial_population(required_workers, workers, worker_count, population, 1000);

	while (1)
	{
		int i;
		for (i = 0; i < 1000; i++) {
			population[i].score = evaluate_schedule(&population[i], required_workers, workers, worker_count);
		}

		qsort(population, 1000, sizeof(struct Schedule), compare_schedule);
		if (generation % 100 == 0) {
			printf("Generation nummer %d. Max fitness er %f, værste: %f\n", generation, population[0].score, population[999].score);
		}
		for (i = 0; i < 40; i++) {
			int random = random_number(40, 840);
			combine_schedule(workers, worker_count, required_workers, &population[i], &population[random], &population[840 + i * 4]);
			combine_schedule(workers, worker_count, required_workers, &population[i], &population[random], &population[840 + i * 4 + 1]);
			combine_schedule(workers, worker_count, required_workers, &population[i], &population[random], &population[840 + i * 4 + 2]);
			combine_schedule(workers, worker_count, required_workers, &population[i], &population[random], &population[840 + i * 4 + 3]);
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
	Worker *workers[],
	const size_t worker_count,
	const RequiredWorkers required_workers,
	Schedule *schedule
) {
	int day;
	
	for (day = 0; day < 7; day++) {
		unsigned int workers_top = worker_count;
		int vagt;
		for (vagt = 0; vagt < 3; vagt++) {
			int required_workers_yep = get_required_for_shift(required_workers, (enum Shift) vagt);
			unsigned int worker_index;
			schedule->blocks[day * 3 + vagt].workers = malloc(required_workers_yep * sizeof(struct Worker*));
			if (schedule->blocks[day * 3 + vagt].workers == NULL) {
				fatal_error("Hukkomelse er tom");
			}
			for (worker_index = 0; worker_index < required_workers_yep; worker_index++) {
				int random_index = random_number(0, workers_top);
				Worker* tmp = NULL;
				if (workers_top <= 0) {
					fatal_error("Not enough workers to fulfill a single day");
				}
				schedule->blocks[day * 3 + vagt].workers[worker_index] = workers[random_index];
				tmp = workers[random_index];
				workers[random_index] = workers[workers_top - 1];
				workers[workers_top - 1] = tmp; 
				workers_top--; 
			}
		}
	}
}

unsigned int get_required_for_shift(RequiredWorkers required_workers, enum Shift shift) {
	switch (shift) {
	case SHIFT_NIGHT:
		return required_workers.night_workers;
	case SHIFT_DAY:
		return required_workers.day_workers;
	case SHIFT_EVENING:
		return required_workers.evening_workers;
	default:
		fatal_error("program fejl");
	}
	return 0;
}

void set_required_for_shift(RequiredWorkers* required_workers, enum Shift shift, unsigned int val) {
	switch (shift) {
	case SHIFT_NIGHT:
		required_workers->night_workers = val;
		break;
	case SHIFT_DAY:
		required_workers->day_workers = val;
		break;
	case SHIFT_EVENING:
		required_workers->evening_workers = val;
		break;
	default:
		fatal_error("program fejl");
	}
}

int random_number(int min, int max) {
	return (int) floor((double) min + (((double)rand()) / ((double) RAND_MAX + 1)) * ((double) max - (double) min));
}

double evaluate_schedule(Schedule* schedule, const RequiredWorkers required_workers, Worker* worker[], unsigned int amount_of_workers){
	unsigned int day, shift, worker_number;

	size_t worker_i = 0;
	for (worker_i = 0; worker_i < amount_of_workers; worker_i++) {
		worker[worker_i]->last_block = 0;
		worker[worker_i]->consecutive_night_shifts = 0;
		worker[worker_i]->day_off = 0;
	}

	schedule->score = 0;
	for (day = 0; day < 7 ; day++){
		
		for (shift = 0; shift < 3; shift++){

			unsigned int block_number = day * 3 + shift;
			Worker** current_worker_array = (schedule->blocks[block_number]).workers;
			
			unsigned int workers_needed = get_required_for_shift(required_workers, shift);

			for (worker_number = 0; worker_number < workers_needed; worker_number++) {
				Worker* current_worker = current_worker_array[worker_number];

				/* Vi gør så kun de workers med i skemaet kan give minuspoint for day_off */
				if ( current_worker->day_off == 0){
					current_worker->day_off == -1;
				}

				/* Tjekker preferred shift */
				if (current_worker->desired_shift == shift){
					schedule->score += 1;
				} 

				/*Tjekker om medarbejderen har arbejdet den samme dag*/
				if(block_number - current_worker->last_block <= 2 && day > 0) {
						if (block_number - current_worker->last_block == 2 && block_number % 3 != 2){
							schedule->score -= 1000;
						}
				}
				
				/* Tjekker om det er cyklisk*/
				if (day > 0) {
					/*Tjekker om vagten er om natten*/
					if (current_worker->last_block % 3 == 2){
						if (!(block_number % 3 == 2 || block_number >= current_worker->last_block + 3)){
							schedule->score -= 1000;
						}
					}
					else if (!(
						block_number % 3 == current_worker->last_block % 3 || 
						block_number == current_worker->last_block % 3 + 1 || 
						block_number >= current_worker->last_block + 3)
					) {
						schedule->score -= 1000;
					}
				}
				
				/* Tjekker nattevagter i streg*/
				if (shift == 0) {
					if (current_worker->last_block == block_number - 3 ){
						current_worker->consecutive_night_shifts += 1;
						if (current_worker->consecutive_night_shifts > 2){
							schedule->score -= 1000;
						}
					}
				} 
				else {
					current_worker->consecutive_night_shifts = 0;
				}

				/* Tjekker fridøgn */
				if (block_number - current_worker->last_block > 5){
					current_worker->day_off = 1;
				}
				/* Sætter last shift*/
				current_worker->last_block = block_number;

				/*Tjek preferred day*/
				if (current_worker->desired_day_off == day) {
					schedule->score -= 2;
				}
			}
		}
	}
	
	/* Tjekker om der har været fridøgn*/
	for (worker_i = 0; worker_i < amount_of_workers; worker_i++) {
		if (worker[worker_i]->day_off == -1) {
			printf("dayoff");
			schedule->score -= 1000;
		}
	}

	return schedule->score;
}



int compare_schedule(const void* a, const void* b) {
	const  Schedule* sa = a;
	const  Schedule* sb = b;

	return sb->score - sa->score;
}

void combine_schedule(Worker* workers[], size_t worker_count, RequiredWorkers required_workers, const  Schedule* a, const  Schedule* b,  Schedule* out) {
	int crossover_start = random_number(0, 20);
	int crossover_end = random_number(crossover_start + 1, 21);
	const BlockSchedule* a_block = (const  BlockSchedule*) a->blocks;
	const BlockSchedule* b_block = (const  BlockSchedule*) b->blocks;
	BlockSchedule* out_block = ( BlockSchedule*) out->blocks;

	int j;

	for (j = 0; j < 21; j++) {
		if (j >= crossover_start && j < crossover_end) {
			memcpy(out_block[j].workers, b_block[j].workers, get_required_for_shift(required_workers, j % 3));
		} else {
			memcpy(out_block[j].workers, a_block[j].workers, get_required_for_shift(required_workers, j % 3));
		}
	}
	if (rand() % 3 == 0) {
		/*Det her skal renskrives*/
		int random_block_index = random_number(0, 21);
		int i;
		int top = worker_count;
		int needed_workers = get_required_for_shift(required_workers, random_block_index % 3);
		for (i = 0; i < needed_workers; i++) {
			int random_index = random_number(0, top);
			Worker* tmp = NULL;
			if (j <= 0) {
				fatal_error("Not enough workers to fulfill a single day");
			}
			out_block[random_block_index].workers[i] = workers[random_index];
			tmp = workers[random_index];
			workers[random_index] = workers[top - 1];
			workers[top - 1] = tmp; 
			top--; 
		}
	}
}

Schedule read_schedule(FILE* file, RequiredWorkers* out, Worker** workers, size_t worker_count) {
	Schedule rv;
	unsigned int block_id;
	int res = 0;
	char shift_string[33];
	char day_string[33];
	out->night_workers = 0;
	out->day_workers = 0;
	out->evening_workers = 0;
	for (block_id = 0; block_id < 21; block_id++) {
		size_t allocated_workers = 10;
		size_t workers_read = 0;
		unsigned int amount_required = get_required_for_shift(*out, block_id % 3);
		if (feof(file)) {
			fatal_error("Forkert formateret vagtplan");
		}
		
		res = fscanf(file,
			" %32[^;,] %*1[;,] %32[^;,] %*1[;,]", 
			day_string, shift_string
		);

		rv.blocks[block_id].workers = malloc(allocated_workers * sizeof(Worker*));

		if (rv.blocks[block_id].workers == NULL) {
			fatal_error("kunne ikke allokere hukommelse");
		}

		while (true) {
			char name_buffer[MAX_NAME_LENGTH + 1];
			unsigned int uuid_read;
			int end_char = 0;
			if (feof(file)) {
				fatal_error("Forkert formateret vagtplan");
			}
			if (workers_read + 1 > allocated_workers) {
				allocated_workers += 10;
				rv.blocks[block_id].workers = realloc(rv.blocks[block_id].workers, allocated_workers * sizeof(Worker*));
				if (rv.blocks[block_id].workers == NULL) {
					fatal_error("kunne ikke allokere hukommelse");
				}
			}

			res = fscanf(file, " %50[^.].%u %*1[;,] ", name_buffer,  &uuid_read);
			if (res != 2) {
				fatal_error("Forkert formateret vagtplan");
			}
			name_buffer[MAX_NAME_LENGTH] = 0;

			rv.blocks[block_id].workers[workers_read] = find_worker_from_uuid(workers, worker_count, uuid_read);
			if (rv.blocks[block_id].workers[workers_read] == NULL) {
				fatal_error("Forkert uuid");
			}
			workers_read++;
			end_char = fgetc(file);
			if (end_char == '$') {
				break;
			}
			ungetc(end_char, file);
		}
		fscanf(file, "%*[^\n]\n");
		if (amount_required == 0) {
			set_required_for_shift(out, block_id % 3, workers_read);
		} else {
			if (amount_required != workers_read) {
				printf("%u, %u\n", amount_required, workers_read);
				fatal_error("Forkert mængde medarbejdere sat");
			}
		}
	}
	return rv;
}

void write_schedule(FILE* file, const  Schedule* schedule, RequiredWorkers required_workers) {
	unsigned int block_id;
	for (block_id = 0; block_id < 21; block_id++) {
		unsigned int amount_of_workers = get_required_for_shift(required_workers, block_id % 3);
		unsigned int i;
		fprintf(file, "%s,%s", get_day_as_string(block_id / 3), get_shift_as_string(block_id % 3));
		for (i = 0; i < amount_of_workers; i++) {
			fprintf(file, ",%s.%u", schedule->blocks[block_id].workers[i]->name, schedule->blocks[block_id].workers[i]->uuid);
		}
		fprintf(file, ",$\n");
	}
}

Worker* find_worker_from_uuid(Worker** workers, size_t worker_count, unsigned int uuid) {
	size_t i = 0;
	for (i = 0; i < worker_count; i++) {
		if (workers[i]->uuid == uuid) {
			return workers[i];
		}
	}
	return NULL;
}

const char* get_day_as_string(enum Day day){
	switch (day) {
	case DAY_MONDAY:
		return "mandag";
		break;
	case DAY_TUESDAY:
		return "tirsdag";
	case DAY_WEDNESDAY:
		return "onsdag";
	case DAY_THURSDAY:
		return "torsdag";
	case DAY_FRIDAY:
		return "fredag";
	case DAY_SATURDAY:
		return "lørdag";
	case DAY_SUNDAY:
		return "søndag";
	case DAY_INVALID:
		return "invalid";
	default:
		break;
	}
	fatal_error("Program fejl2");
	return "";
}

const char* get_shift_as_string(enum Shift shift) {
	switch (shift) {
	case SHIFT_NIGHT:
		return "nat";
	case SHIFT_DAY:
		return "dag";
	case SHIFT_EVENING: 
		return "nat";
	case SHIFT_INVALID:
		return "ingen";
	}
	fatal_error("Program fejl1");
	return "";
}

void print_worker_schedule(FILE* file, const Worker* worker, const Schedule* schedule, RequiredWorkers required_workers) {
	unsigned int day;
	unsigned int shift;
	fprintf(file,
		"+---------------+--------+--------+--------+--------+--------+--------+--------+\n"
		"|               | Mandag | Tirsdag| Onsdag | Torsdag| Fredag | Lørdag | Søndag |\n"
		"+---------------+--------+--------+--------+--------+--------+--------+--------+\n"
	);
	for (shift = 0; shift < 3; shift++) {
		fprintf(file, "| %s |", get_time_slot(shift));
		for (day = 0; day < 7; day++) {
			unsigned int worker_i;
			unsigned int amount_of_workers = get_required_for_shift(required_workers, shift);
			int found = 0;
			for (worker_i = 0; worker_i < amount_of_workers; worker_i++) {
				if (worker->uuid == schedule->blocks[shift + day * 3].workers[worker_i]->uuid) {
					found = 1;
					break;
				}
			}
			if (found)  {
				fprintf(file, " ###### |");
			} else {
				fprintf(file, "        |");
			}
		}
		fprintf(file, "\n");
	}
	fprintf(file, "+---------------+--------+--------+--------+--------+--------+--------+--------+\n");
}

const char* get_time_slot(enum Shift shift) {
	switch (shift) {
		case SHIFT_NIGHT:
			return "00:00 - 08:00";
		case SHIFT_DAY:
			return "08:00 - 16:00";
		case SHIFT_EVENING:
			return "16:00 - 24:00";
		default:
			fatal_error("Program fejl3");
			return NULL;
	}
}