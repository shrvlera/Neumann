#include <stdio.h>
/*
  принципы архитектуры фон Неймана:
 1. Единая память (массив memory): в одном адресном пространстве хранятся
     и команды программы, и обычные числа (данные), с которыми она работает.
 2. Последовательное управление: процессор считывает команды по очереди
     по счётчику команд (PC), если порядок не меняют команды перехода или прерывания.
 3. Наличие АЛУ и регистров: все вычисления выполняются над аккумулятором (ACC),
     а код текущей команды сохраняется в регистре команд (IR)
*/

// размер массивов
#define MEM_SIZE 256
#define REG_COUNT 8

// индексы регистров cpu_state
#define PC 0     // счетчик команд
#define ACC 1    // аккумулятор
#define IR 2     // регистр команд
#define IVEC 3   // вектор прерываний
#define HALT 4   // флаг остановки
#define ERR 5    // код последней ошибки

// индексы первый ячеек memory
#define IDX_IVEC 0  // вектор прерываний
#define IDX_START 1 // адрес превой команды

int main() {
	int memory[MEM_SIZE] = {0};
	int cpu_state[REG_COUNT] = {0};

// 1 считываем програму и данные
	int count = 0;
	while (count < MEM_SIZE && scanf("%d", &memory[count]) == 1) {
		count++;
	}

	if (count < 2) {
		printf("недостаточно данных на вводе\n");
		return 1;
	}
// 2 - подготовка регистров процессора перед запуском цикла
	cpu_state[IVEC] = memory[IDX_IVEC];
	cpu_state[PC]   = memory[IDX_START];
	cpu_state[ACC]  = 0;
	cpu_state[HALT] = 0;
	cpu_state[ERR]  = 0;

// 3 основной цикл: работает, пока флаг остановки = 0
	while (cpu_state[HALT] == 0) {
		int pc = cpu_state[PC];

		// поверка - не вышел ли счетчик за границы массива
		if (pc < 0 || pc >= MEM_SIZE) {
			printf("ошибка: счётчик команд вышел за границы памяти\n");
			cpu_state[ERR] = 1;
			if (pc == cpu_state[IVEC]) {
				cpu_state[HALT] = 1;
			} else {
				cpu_state[PC] = cpu_state[IVEC];
			}
			continue;
		}
		// берем код текущей команды из памяти в регист команд
		cpu_state[IR] = memory[pc];

		switch (cpu_state[IR]) {
			case 0: // остановка программы (команда 0)
				printf("pc=%d: команда 0, operand=нет, acc=%d\n", pc, cpu_state[ACC]);
				cpu_state[HALT] = 1;
				break;

			case 1: // аккумулятор(ACC) + 1 (команда 1)
				cpu_state[ACC] = cpu_state[ACC] + 1;
				printf("pc=%d: команда 1, operand=нет, acc=%d\n", pc, cpu_state[ACC]);
				cpu_state[PC] = pc + 1;
				break;

			case 2: // аккумулятор(ACC) - 1 (команда 2)
				cpu_state[ACC] = cpu_state[ACC] - 1;
				printf("pc=%d: команда 2, operand=нет, acc=%d\n", pc, cpu_state[ACC]);
				cpu_state[PC] = pc + 1;
				break;

			case 3: { // аккумулятор(ACC) + число из ячейки памяти
				int addr = memory[pc + 1];
				if (addr < 0 || addr >= MEM_SIZE) {
					printf("pc=%d: команда 3, operand=%d, выход за границы памяти\n", pc, addr);
					cpu_state[ERR] = 1;
					if (pc == cpu_state[IVEC]) {
						cpu_state[HALT] = 1;
					} else {
						cpu_state[PC] = cpu_state[IVEC];
					}
				} else {
					cpu_state[ACC] = cpu_state[ACC] + memory[addr];
					printf("pc=%d: команда 3, operand=%d, acc=%d\n", pc, addr, cpu_state[ACC]);
					cpu_state[PC] = pc + 2;
				}
				break;
			}

			case 4: { // безусловный переход (JUMP) (команда 4)
				int addr = memory[pc + 1];
				if (addr < 0 || addr >= MEM_SIZE) {
					printf("pc=%d: команда 4, operand=%d, выход за границы памяти\n", pc, addr);
					cpu_state[ERR] = 1;
					if (pc == cpu_state[IVEC]) {
						cpu_state[HALT] = 1;
					} else {
						cpu_state[PC] = cpu_state[IVEC];
					}
				} else {
					printf("pc=%d: команда 4, operand=%d, acc=%d\n", pc, addr, cpu_state[ACC]);
					cpu_state[PC] = addr; // переход на новый адрес
				}
				break;
			}

			case 5: { //условный переход при ACC(аккумулятор) = 0
				int addr = memory[pc + 1];
				if (addr < 0 || addr >= MEM_SIZE) {
					printf("pc=%d: команда 5, operand=%d,выход за границы памяти\n", pc, addr);
					cpu_state[ERR] = 1;
					if (pc == cpu_state[IVEC]) {
						cpu_state[HALT] = 1;
					} else {
						cpu_state[PC] = cpu_state[IVEC];
					}
				} else {
					printf("pc=%d: команда 5, operand=%d, acc=%d\n", pc, addr, cpu_state[ACC]);
					if (cpu_state[ACC] == 0) {
						cpu_state[PC] = addr;
					} else {
						cpu_state[PC] = pc + 2;
					}
				}
				break;
			}

			default: // Если встретилось неизвестное число вместо команды
				printf("pc=%d: команда %d, ошибка: неизвестная команда\n", pc, cpu_state[IR]);
				cpu_state[ERR] = 1;
				if (pc == cpu_state[IVEC]) {
					cpu_state[HALT] = 1;
				} else {
					cpu_state[PC] = cpu_state[IVEC];
				}
				break;
		}
	}

	if (cpu_state[ERR] == 0) {
		printf("Программа успешно завершена.\n");
	} else {
		printf("Программа завершена с ошибкой.\n");
	}
	return 0;
}













































