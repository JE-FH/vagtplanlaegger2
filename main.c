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

int main(int argc, char** argv) {
	return 0;
}