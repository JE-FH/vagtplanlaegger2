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
	unsigned int last_shift; 
	unsigned int consecutive_night_shifts;
	unsigned int uuid;
} Worker;

typedef struct BlockSchedule {
	 Worker** workers;
} BlockSchedule;

typedef struct Schedule {
	BlockSchedule blocks[21];
	int workers[21];
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

/*Expects that every block is preallocated for the amount of workers needed*/
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
double evaluate_schedule(Schedule* schedule, const  RequiredWorkers required_workers, const Worker* worker, unsigned int amount_of_workers);


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
void write_schedule(FILE* file, const  Schedule* schedule);

/**
 * Læser en schedule som er gemt i en fil
 * @param[in, out] file en åben fil hvor schedule skal læses fra
 * @return det indlæste skema
 */
 Schedule read_schedule(FILE* file);

/**
 * skriver et pænt skema for en enkelt person til en fil
 * @param[in, out] file en åben fil hvor skemaet skal skrives til
 * @param[in] worker den medarbejder skemaet skal printes til
 * @param[in] skemaet som er lavet for medarbejderen
 */
void print_worker_schedule(FILE* file, const  Worker* worker, const  Schedule* schedule);

void fatal_error(const char* reason);

void evaluate_schedule_required_workers( Schedule* schedule, const RequiredWorkers required_workers, unsigned int block_number, size_t j);

enum Shift string_to_shift(char* input);

enum Day string_to_day(char* input);

unsigned int get_required_workers( RequiredWorkers required_workers, enum Shift shift);

void evaluate_schedule_preferred_day_off(Worker* worker, size_t m, size_t amount_assigned_workers, Worker* assigned_workers, unsigned int amount_of_workers)

int compare_schedule(const void* a, const void* b);

void combine_schedule(const  Schedule* a, const  Schedule* b,  Schedule* out);

int random_number(int min, int max);

int main(int argc, char** argv) {
	FILE* fil = fopen("input.csv", "r");
	Worker* workers;
	size_t worker_count = 0;
	Schedule schedule;
	RequiredWorkers required_workers;
	srand(time(NULL));

	required_workers.night_workers = 40;
	required_workers.day_workers = 80;
	required_workers.evening_workers = 80;
	
	if (fil == NULL) {
		fatal_error("Kunne ikke åbne input csv filen");
	}

	workers = read_workers(fil, &worker_count);
	//schedule = make_schedule(&workers, worker_count, required_workers);
	getchar();
	
	
	// BlockSchedule hejmeddig;
	// Schedule aSkema;
	// aSkema.blocks[0] = hejmeddig;
	// hejmeddig.workers = workerPointerArray;
	// RequiredWorkers reqwork;
	// Schedule array[2];
	// array[0] = aSkema;
	// printf("%s",(aSkema.blocks[0].workers[0]->name));
	// evaluate_schedule(array,reqwork);
	
	return 0;
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

		// workers[*worker_count].last_block.day = DAY_INVALID;
		// workers[*worker_count].last_block.shift = SHIFT_INVALID;
		// workers[*worker_count].night_combo = 0;
		// workers[*worker_count].combo_start = 0;
		// workers[*worker_count].index = *worker_count;

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


 Schedule make_schedule( Worker* workers[], const size_t worker_count, const  RequiredWorkers required_workers) {
	
}


void generate_random_schedule(
	Worker* workers[], 
	const size_t worker_count, 
	const  RequiredWorkers required_workers,  
	Schedule* schedule
) {
	
}

int random_number(int min, int max) {
	return (int) round(min + (((double)rand()) / RAND_MAX) * (max - min));
}

double evaluate_schedule( Schedule* schedule, const RequiredWorkers required_workers, const Worker* worker, unsigned int amount_of_workers) {
	size_t i, j, k, m;

	for (i = 0; i < 7 ; i++){
		Worker assigned_workers[amount_of_workers];
		unsigned int amount_assigned_workers = 0;
		
		for (j = 0; j < 3; j++){
			unsigned int block_number = i * 3 + j;
			
			Worker** current_worker_array = (schedule->blocks[block_number]).workers;
			for (k = 0; k < schedule->workers[block_number]; k++){
				int unikt_navn = 1;
				Worker current_worker = *(current_worker_array[k]);

				/* Tjekker preferred shift */
				if (current_worker.desired_shift == k) schedule->score += 1;

				/* Tjekker flere vagter på en dag */
				for (m = 0; m < amount_assigned_workers; m++){
					if (current_worker.uuid == assigned_workers[m].uuid){
						schedule->score -= 1000;
						unikt_navn = 0;
						break;
					}
				}
				if (unikt_navn == 1){
					assigned_workers[amount_assigned_workers] = current_worker;
					amount_assigned_workers++;
				} 
				
				/* Tjekker om det er cyklisk*/
				if (i > 0){
					if (current_worker.last_shift % 3 == 2){
						if (!(block_number % 3 == 2 || block_number >= current_worker.last_shift + 3)){
							schedule->score -= 1000;
						}
					}
					else if (!(block_number % 3 == current_worker.last_shift % 3 || block_number == current_worker.last_shift % 3 + 1 || block_number >= current_worker.last_shift + 3)){
						schedule->score -= 1000;
					}
				}
				
				/* Tjekker nattevagter i streg*/
				if ( j == 0 ){
					if (current_worker.last_shift == block_number - 3 ){
						current_worker.consecutive_night_shifts += 1;
						if (current_worker.consecutive_night_shifts > 2){
							schedule->score -= 1000;
						}
					}
				}
				else{
					current_worker.consecutive_night_shifts = 0;
				}
				/* Sætter last shift*/
				current_worker.last_shift = block_number;
			}
			/* Tjekker om der er nok arbejdere*/

		evaluate_schedule_required_workers(schedule, required_workers, block_number, j);
		}
	evaluate_schedule_preferred_day_off(worker, m, amount_assigned_workers, assigned_workers, amount_of_workers);
	}

return schedule->score;
}

void evaluate_schedule_required_workers( Schedule* schedule, const RequiredWorkers required_workers, unsigned int block_number, size_t j){
	switch (j)
			{
			case 0:
				if (required_workers.night_workers > schedule->workers[block_number]){
					schedule->score -= 1000;
				}
				break;
			case 1:
				if (required_workers.day_workers > schedule->workers[block_number]){
					schedule->score -= 1000;
				}
				break;

			case 2:
				if (required_workers.evening_workers > schedule->workers[block_number]){
					schedule->score -= 1000;
				}
				break;
			}
}

void evaluate_schedule_preferred_day_off(Worker* worker, size_t m, size_t amount_assigned_workers, Worker* assigned_workers, unsigned int amount_of_workers){
	int l;
	for (l = 0; l < amount_of_workers; l++){
			if (worker[l].desired_day_off == i){
				int day_off_bool = 1;
				for (m = 0; m < amount_assigned_workers; m++){
					if (worker[l].uuid == assigned_workers[m].uuid){
						day_off_bool = 0;
						break;
					}
			}
			if (day_off_bool == 1){
				 schedule->score += 2;
			}
}




int compare_schedule(const void* a, const void* b) {
	const  Schedule* sa = a;
	const  Schedule* sb = b;

	return sb->score - sa->score;
}

void combine_schedule(const  Schedule* a, const  Schedule* b,  Schedule* out) {
	int crossover_start = random_number(0, 20);
	int crossover_end = random_number(crossover_start + 1, 21);
	
	const  BlockSchedule* a_block = (const  BlockSchedule*) a->blocks;
	const  BlockSchedule* b_block = (const  BlockSchedule*) b->blocks;
	 BlockSchedule* out_block = ( BlockSchedule*) out->blocks;

	int j;

	for (j = 0; j < 21; j++) {
		if (j >= crossover_start && j < crossover_end) {
			out_block[j].workers = b_block[j].workers;
		} else {
			out_block[j].workers = a_block[j].workers;
		}
	}
}