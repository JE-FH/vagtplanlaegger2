#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 50

enum Day {
	DAY_MONDAY,
	DAY_TUESDAY,
	DAY_WEDNESDAY,
	DAY_THURSDAY,
	DAY_FRIDAY,
	DAY_SATURDAY,
	DAY_SUNDAY
};

enum Shift {
	SHIFT_NIGHT,
	SHIFT_DAY,
	SHIFT_EVENING
};

struct Block {
	enum Day day;
	enum Shift shift;
};

struct Worker {
	char name[MAX_NAME_LENGTH + 1];
	enum Day desired_day_off;
	struct Block last_block;
	enum Shift desired_shift;
	unsigned int night_combo;
	int combo_start;
	unsigned int index;
	unsigned int uuid;
};

struct BlockSchedule {
	struct Worker* workers;
	unsigned int tries;
};

struct DaySchedule {
	struct BlockSchedule blocks[3];
};

struct Schedule {
	struct DaySchedule days[7];
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
struct Schedule make_schedule(const struct Worker workers[], const size_t worker_count, const struct RequiredWorkers required_workers);

/**
 * Finder alle de medarbejder som kan arbejde på det specificerede tidspunkt
 * @param[in] workers alle workers
 * @param[in] worker_count antallet af workers i workers arrayen
 * @param[in] block det tidspunkt blokken ligger på
 * @param[out] valid_workers der hvor alle de valide workers skal puttes i, den skal være stor nok til at rumme worker_count workers
 * @returns mængden af medarbejdere der blev fundet
 */
size_t find_valid_workers(const struct Worker workers[], const size_t worker_count, const struct Block block, struct Worker valid_workers[]);

/**
 * Tjekker om en medarbejder kan arbejde i den bestemte block
 * @param[in] worker en pointer til den worker som skal tjekkes
 * @param[in] block den block som skal tjekkes
 * @returns true hvis worker er valid ellers false
 */
bool is_worker_valid(const struct Worker* worker, const struct Block block);

/**
 * Sortere listen af workers sådan at dem der helst skal have vagten kommer først
 * @param[in, out] workers listen af medarbejdere som skal sorteres
 * @param[in] worker_count antallet af workers som er i workers listen
 */
void sort_workers(struct Worker workers[], const size_t worker_count);

/**
 * Sammenligner to medarbejdere, den skal bruges til qsort
 * @param[in] a den første worker
 * @param[in] b den anden worker
 */
int compare_worker(const void* a, const void* b);

/**
 * Giver en score for hvor godt en worker passer til en vagt, blocken skal på en eller anden måde komme her ind
 * @param[in] worker den medarbejder som skal evalueres
 * @returns en score der bestemer hvor godt denne medarbejder kan tage vagten
 */
int eval_worker(const struct Worker* worker);

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

int main(int argc, char** argv) {
	return 0;
}