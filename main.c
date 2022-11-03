#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define N 1000

typedef char string[256];

int task_index;

struct Array {
  char *values[N];
  int length;
} first_array, second_array, tasks;


char *to_string(int x) {
  int length = snprintf(NULL, 0, "%d", x);
  char *str = malloc(length + 1);
  snprintf(str, length + 1, "%d", x);
  return str;
}

char* read_string(string prompt) {
  if (prompt) printf("%s ", prompt);
  string value = "";
  scanf("%[^\n]", value);
  return strdup(value);
}

int read_natural(string prompt) {
  if (prompt) printf("%s ", prompt);
  int value;
  int result = scanf("%d", &value);
  if (result != 1 || value <= 0) {
	printf(RED "Некорректный ввод натурального числа!" RESET);
	exit(1);
  }
  return value;
}


void init(string directory_path, struct Array *array) {
  DIR *directory = opendir(directory_path);
  if (!directory) {
	printf(RED "Каталога '%s' не существует!" RESET, directory_path);
	exit(1);
  }
  struct dirent *file;
  while ((file = readdir(directory))) {
	string value = "";
	strcat(value, directory_path);
	strcat(value, "/");
	strcat(value, file->d_name);
	if (file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
	  init(strdup(value), array);
	}
	if (file->d_type != DT_REG) continue;
	array->values[array->length] = strdup(value);
	array->length++;
  }
  closedir(directory);
}

void perform() {
  if (++task_index >= tasks.length) return;
  if (fork() == 0) {
//	printf("%d -> %d | Александр %d " GREEN "рождён" RESET "\n", getppid(), getpid(), task_index);
//	sleep(task_index % 8);
	string task = "";
	strcat(task, tasks.values[task_index]);
	strcat(task, " ");
	strcat(task, to_string(getpid()));
	system(task);
//	printf("%d -> %d | Александр %d " RED "погиб" RESET "\n", getppid(), getpid(), task_index);
	exit(0);
  }
}


int main(int argc, char *argv[]) {
  char* first_directory_path = read_string("Введите первый каталог:");
  char* second_directory_path = read_string("Введите второй каталог:");
  scanf(" %[^\n]", second_directory_path);
  init(first_directory_path, &first_array);
  init(second_directory_path, &second_array);

  if (!first_array.length || !second_array.length) {
	printf(RED "В одном из каталогов отсутствуют какие-либо файлы!" RESET);
	return 1;
  }
  for (int first_index = 0; first_index < first_array.length; first_index++)
	for (int second_index = 0; second_index < second_array.length; second_index++) {
	  char task[255] = "../task.sh '";
	  strcat(task, first_array.values[first_index]);
	  strcat(task, "' '");
	  strcat(task, second_array.values[second_index]);
	  strcat(task, "'");
	  tasks.values[tasks.length] = strdup(task);
	  tasks.length++;
	}

  int limit = read_natural("Введите максимальное число процессов:");

  for (int i = 0; i < limit; i++) perform();
  while (wait(0) > 0) perform();

  return 0;
}
