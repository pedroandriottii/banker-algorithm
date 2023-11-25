#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct Customer
{
  int *max;
  int *allocation;
  int *need;
} Customer;

Customer *customers;
int *available;

int verifyCustomerFile(char *customerFile);
int verifyCommandsFile(char *commandsFile);

int countCustomerResources(char *customerFile);
int countCommandsResources(char *commandsFile);

int countCustomer(char *customerFile);
int countCommands(char *commandsFile);

void readCustomerFile(int number_of_customers, int number_of_resources);
void readCommandFile(int number_of_resources, int number_of_customers);
void executeCommand(char *command, int number_of_resources, int number_of_customers);
void allocateResources(int qtyCustomers, int request[], int number_of_resources, int number_of_customers);
void printTable(int number_of_customers, int number_of_resources);
void releaseResources(int qtyCustomers, int release[], int number_of_resources);
void calculateNeed(int number_of_customers, int number_of_resources);
int isSafety(int number_of_resources, int number_of_customers);
void printTable(int number_of_customers, int number_of_resources);
int countDigits(int num);
int findMaxLength(int number_of_customers, int number_of_resources);
int findMaxAllocLength(int number_of_customers, int number_of_resources);

int main(int argc, char *argv[])
{
  char *customerFile = "customer.txt";
  char *commandsFile = "commands.txt";
  int number_of_customers;
  int number_of_resources;
  number_of_resources = argc - 1;
  available = (int *)malloc(number_of_resources * sizeof(int));

  if (remove("results.txt") != 0)
  {
  }

  // Armazenando os valores dos recursos disponiveis
  int i;
  for (i = 0; i < number_of_resources; i++)
  {
    available[i] = atoi(argv[i + 1]);
  }

  // Verificando se os dois arquivos estão na formatação correta
  if (verifyCustomerFile(customerFile) == 0)
  {
    printf("Fail to read customer.txt\n");
    return 0;
  }

  if (verifyCommandsFile(commandsFile) == 0)
  {
    printf("Fail to read commands.txt\n");
    return 0;
  }

  // Verificando se a quantidade de recursos é a mesma nos dois arquivos e na linha de comando.
  if (number_of_resources != countCustomerResources(customerFile))
  {
    printf("Incompatibility between customer.txt and command line\n");
    return 0;
  }

  if (number_of_resources != countCommandsResources(commandsFile) - 2)
  {
    printf("Incompatibility between commands.txt and command line\n");
    return 0;
  }

  number_of_customers = countCustomer(customerFile);

  readCustomerFile(number_of_customers, number_of_resources);
  readCommandFile(number_of_resources, number_of_customers);

  return 0;
}

int countCustomer(char *customerFile)
{
  FILE *arquivo;
  char linha[100];
  int contador = 0;

  arquivo = fopen(customerFile, "r");

  if (arquivo == NULL)
  {
    perror("Fail to read customer.txt");
    return -1;
  }

  while (fgets(linha, sizeof(linha), arquivo) != NULL)
  {
    contador++;
  }

  fclose(arquivo);

  return contador;
}

int countCommands(char *commandsFile)
{
  FILE *arquivo;
  char linha[100];
  int contador = 0;

  arquivo = fopen(commandsFile, "r");

  if (arquivo == NULL)
  {
    perror("Fail to read commands.txt");
    return -1;
  }

  while (fgets(linha, sizeof(linha), arquivo) != NULL)
  {
    contador++;
  }

  fclose(arquivo);

  return contador;
}

int verifyCustomerFile(char *customerFile)
{
  FILE *file = fopen(customerFile, "r");

  if (file == NULL)
  {
    return 0;
  }

  char line[100];
  int numTokens = 0;
  int isFirstLine = 1;

  while (fgets(line, sizeof(line), file) != NULL)
  {
    size_t len = strlen(line);

    if (len > 0 && line[len - 1] == '\n')
    {
      line[len - 1] = '\0';
      len--;
    }

    if (line[0] == ',' || (len > 1 && line[len - 1] == ','))
    {
      fclose(file);
      return 0;
    }

    char *token = strtok(line, ",");
    int numTokensInLine = 0;

    while (token != NULL)
    {
      for (int i = 0; i < strlen(token); i++)
      {
        if (!isdigit(token[i]))
        {
          fclose(file);
          return 0;
        }
      }

      token = strtok(NULL, ",");
      numTokensInLine++;
    }

    if (isFirstLine)
    {
      numTokens = numTokensInLine;
      isFirstLine = 0;
    }
    else
    {
      if (numTokensInLine != numTokens)
      {
        fclose(file);
        return 0;
      }
    }
  }

  fclose(file);
  return 1;
}

int verifyCommandsFile(char *commandsFile)
{
  FILE *file = fopen(commandsFile, "r");

  if (file == NULL)
  {
    return 0;
  }

  char line[100];
  int numTokens = 0;
  int isFirstLine = 1;

  while (fgets(line, sizeof(line), file) != NULL)
  {
    size_t len = strlen(line);

    if (len > 0 && line[len - 1] == '\n')
    {
      line[len - 1] = '\0';
      len--;
    }

    if (strcmp(line, "*") == 0)
    {
      continue;
    }

    char *command = strtok(line, " ");

    if (!isalpha(command[0]))
    {
      fclose(file);
      return 0;
    }

    char *token = strtok(NULL, " ");
    int numTokensInLine = 1;

    while (token != NULL)
    {
      if (strcmp(token, "*") == 0)
      {
        token = strtok(NULL, " ");
        continue;
      }

      for (int i = 0; i < strlen(token); i++)
      {
        if (!isdigit(token[i]))
        {
          fclose(file);
          return 0;
        }
      }

      token = strtok(NULL, " ");
      numTokensInLine++;
    }

    if (isFirstLine)
    {
      numTokens = numTokensInLine;
      isFirstLine = 0;
    }
    else
    {
      if (numTokensInLine != numTokens)
      {
        fclose(file);
        return 0;
      }
    }
  }

  fclose(file);
  return 1;
}

int countCustomerResources(char *customerFile)
{
  FILE *file = fopen(customerFile, "r");

  char line[100];
  fgets(line, sizeof(line), file);

  int numColumns = 0;
  char *token = strtok(line, ",");
  while (token != NULL)
  {
    numColumns++;
    token = strtok(NULL, ",");
  }

  fclose(file);
  return numColumns;
}

int countCommandsResources(char *commandsFile)
{
  FILE *file = fopen(commandsFile, "r");

  char line[100];
  fgets(line, sizeof(line), file);

  int numColumns = 0;
  char *token = strtok(line, " ");
  while (token != NULL)
  {
    numColumns++;
    token = strtok(NULL, " ");
  }

  fclose(file);
  return numColumns;
}

void readCustomerFile(int number_of_customers, int number_of_resources)
{
  FILE *file = fopen("customer.txt", "r");

  customers = (Customer *)malloc(number_of_customers * sizeof(Customer));

  for (int i = 0; i < number_of_customers; i++)
  {
    customers[i].max = (int *)malloc(number_of_resources * sizeof(int));
    customers[i].allocation = (int *)calloc(number_of_resources, sizeof(int));
    customers[i].need = (int *)malloc(number_of_resources * sizeof(int));

    for (int j = 0; j < number_of_resources; j++)
    {
      fscanf(file, "%d,", &customers[i].max[j]);
      customers[i].need[j] = customers[i].max[j];
    }
  }

  fclose(file);
}

void readCommandFile(int number_of_resources, int number_of_customers)
{
  FILE *file = fopen("commands.txt", "r");

  char command[256];

  while (fgets(command, sizeof(command), file) != NULL)
  {
    executeCommand(command, number_of_resources, number_of_customers);
  }

  fclose(file);
}

void executeCommand(char *command, int number_of_resources, int number_of_customers)
{
  char *mode;
  int customer;
  int *resources = malloc(number_of_resources * sizeof(int));

  mode = strtok(command, " \n");

  if (strcmp(mode, "*") == 0)
  {
    printTable(number_of_customers, number_of_resources);
  }
  else if (strcmp(mode, "RQ") == 0)
  {
    customer = atoi(strtok(NULL, " "));
    for (int i = 0; i < number_of_resources; i++)
    {
      resources[i] = atoi(strtok(NULL, " "));
    }
    allocateResources(customer, resources, number_of_resources, number_of_customers);
  }
  else if (strcmp(mode, "RL") == 0)
  {
    customer = atoi(strtok(NULL, " "));
    for (int i = 0; i < number_of_resources; i++)
    {
      resources[i] = atoi(strtok(NULL, " "));
    }
    releaseResources(customer, resources, number_of_resources);
  }

  free(resources);
}

void allocateResources(int qtyCustomers, int request[], int number_of_resources, int number_of_customers)
{
  FILE *outputFile = fopen("results.txt", "a");

  for (int i = 0; i < number_of_resources; i++)
  {
    if (request[i] > customers[qtyCustomers].need[i])
    {
      fprintf(outputFile, "The customer %d request ", qtyCustomers);
      for (int j = 0; j < number_of_resources; j++)
      {
        fprintf(outputFile, "%d ", request[j]);
      }
      fprintf(outputFile, "was denied because exceed its maximum need\n");

      fclose(outputFile);
      return;
    }
  }

  for (int i = 0; i < number_of_resources; i++)
  {
    if (request[i] > available[i])
    {
      fprintf(outputFile, "The resources");
      for (int j = 0; j < number_of_resources; j++)
      {
        fprintf(outputFile, " %d", available[j]);
      }
      fprintf(outputFile, " are not enough to customer %d request", qtyCustomers);
      for (int j = 0; j < number_of_resources; j++)
      {
        fprintf(outputFile, " %d", request[j]);
      }
      fprintf(outputFile, "\n");

      fclose(outputFile);
      return;
    }
  }

  for (int i = 0; i < number_of_resources; i++)
  {
    available[i] -= request[i];
    customers[qtyCustomers].allocation[i] += request[i];
    customers[qtyCustomers].need[i] -= request[i];
  }

  if (!isSafety(number_of_resources, number_of_customers))
  {
    for (int i = 0; i < number_of_resources; i++)
    {
      available[i] += request[i];
      customers[qtyCustomers].allocation[i] -= request[i];
      customers[qtyCustomers].need[i] += request[i];
    }
    fprintf(outputFile, "The customer %d request", qtyCustomers);
    for (int i = 0; i < number_of_resources; i++)
    {
      fprintf(outputFile, " %d", request[i]);
    }
    fprintf(outputFile, " was denied because result in an unsafe state\n");
  }
  else
  {
    fprintf(outputFile, "Allocate to customer %d the resources", qtyCustomers);
    for (int i = 0; i < number_of_resources; i++)
    {
      fprintf(outputFile, " %d", request[i]);
    }
    fprintf(outputFile, "\n");
  }

  fclose(outputFile);
}

void calculateNeed(int need[P][R], int maxm[P][R], int allot[P][R])
{
  for (int i = 0; i < P; i++)
    for (int j = 0; j < R; j++)
      need[i][j] = maxm[i][j] - allot[i][j];
}

int isSafe(int processes[], int avail[], int maxm[][R], int allot[][R])
{
  int need[P][R];

  // Função para calcular a matriz de necessidade
  calculateNeed(need, maxm, allot);

  // Marcar todos os processos como incompletos
  int finish[P] = {0};

  // Para armazenar a sequência segura
  int safeSeq[P];

  // Fazer uma cópia dos recursos disponíveis
  int work[R];
  for (int i = 0; i < R; i++)
    work[i] = avail[i];

  // Enquanto todos os processos não estiverem concluídos
  // ou o sistema não estiver em um estado seguro.
  int count = 0;
  while (count < P)
  {
    // Encontrar um processo que não foi concluído e
    // cujas necessidades podem ser satisfeitas com os recursos atuais em work[].
    int found = 0;
    for (int p = 0; p < P; p++)
    {
      // Primeiro, verificar se um processo está concluído,
      // se não, vá para a próxima condição
      if (finish[p] == 0)
      {
        // Verificar se, para todos os recursos de
        // P atual, a necessidade é menor
        // do que o work
        int j;
        for (j = 0; j < R; j++)
          if (need[p][j] > work[j])
            break;

        // Se todas as necessidades de p foram satisfeitas.
        if (j == R)
        {
          // Adicionar os recursos alocados de
          // P atual aos recursos disponíveis/work
          // ou seja, liberar os recursos
          for (int k = 0; k < R; k++)
            work[k] += allot[p][k];

          // Adicionar este processo à sequência segura.
          safeSeq[count++] = p;

          // Marcar este p como concluído
          finish[p] = 1;

          found = 1;
        }
      }
    }

    // Se não conseguirmos encontrar o próximo processo na sequência segura.
    if (found == 0)
    {
      printf("O sistema não está em um estado seguro.\n");
      return 0;
    }
  }

  // Se o sistema estiver em um estado seguro, então
  // a sequência segura será a seguinte
  printf("O sistema está em um estado seguro.\nA sequência segura é: ");
  for (int i = 0; i < P; i++)
    printf("%d ", safeSeq[i]);

  return 1;
}

void releaseResources(int qtyCustomers, int release[], int number_of_resources)
{
  FILE *outputFile = fopen("results.txt", "a");

  for (int i = 0; i < number_of_resources; i++)
  {
    if (release[i] > customers[qtyCustomers].allocation[i])
    {
      fprintf(outputFile, "The customer %d release", qtyCustomers);
      for (int j = 0; j < number_of_resources; j++)
      {
        fprintf(outputFile, " %d", release[j]);
      }
      fprintf(outputFile, " was denied because exceed its maximum allocation\n");

      fclose(outputFile);
      return;
    }
  }

  for (int i = 0; i < number_of_resources; i++)
  {
    available[i] += release[i];
    customers[qtyCustomers].allocation[i] -= release[i];
    customers[qtyCustomers].need[i] += release[i];
  }

  fprintf(outputFile, "Release from customer %d the resources", qtyCustomers);

  for (int i = 0; i < number_of_resources; i++)
  {
    fprintf(outputFile, " %d", release[i]);
  }

  fprintf(outputFile, "\n");
  fclose(outputFile);
}

void printTable(int number_of_customers, int number_of_resources)
{
  FILE *outputFile = fopen("results.txt", "a");
  int max_word_len = 8;
  int alloc_word_len = 11;
  int max_column_len = findMaxLength(number_of_customers, number_of_resources);
  int alloc_column_len = findMaxAllocLength(number_of_customers, number_of_resources);
  int max_blank_spaces = 0;
  int alloc_blank_spaces = 0;

  fprintf(outputFile, "MAXIMUM");

  for (int i = max_word_len; i < max_column_len; i++)
  {
    fprintf(outputFile, " ");
  }
  fprintf(outputFile, " | ");

  fprintf(outputFile, "ALLOCATION");

  for (int i = alloc_word_len; i < alloc_column_len; i++)
  {
    fprintf(outputFile, " ");
  }
  fprintf(outputFile, " | ");

  fprintf(outputFile, "NEED\n");

  max_blank_spaces = max_word_len - max_column_len;
  alloc_blank_spaces = alloc_word_len - alloc_column_len;

  for (int i = 0; i < number_of_customers; i++)
  {
    for (int j = 0; j < number_of_resources; j++)
    {
      fprintf(outputFile, "%d ", customers[i].max[j]);
    }

    for (int k = 0; k < max_blank_spaces; k++)
    {
      fprintf(outputFile, " ");
    }
    fprintf(outputFile, "| ");

    for (int j = 0; j < number_of_resources; j++)
    {
      fprintf(outputFile, "%d ", customers[i].allocation[j]);
    }

    for (int k = 0; k < alloc_blank_spaces; k++)
    {
      fprintf(outputFile, " ");
    }
    fprintf(outputFile, "| ");

    for (int j = 0; j < number_of_resources; j++)
    {
      fprintf(outputFile, "%d ", customers[i].need[j]);
    }

    fprintf(outputFile, "\n");
  }

  fprintf(outputFile, "AVAILABLE ");
  for (int i = 0; i < number_of_resources; i++)
  {
    fprintf(outputFile, "%d ", available[i]);
  }

  fprintf(outputFile, "\n");
  fclose(outputFile);
}

int countDigits(int num)
{
  int count = 0;

  if (num == 0)
  {
    return 1;
  }

  while (num != 0)
  {
    num /= 10;
    count++;
  }

  return count;
}

int findMaxLength(int number_of_customers, int number_of_resources)
{
  int max_length = 0;

  for (int i = 0; i < number_of_customers; i++)
  {
    int current_length = 0;

    for (int j = 0; j < number_of_resources; j++)
    {
      current_length += countDigits(customers[i].max[j]);

      current_length += 1;
    }

    if (current_length > max_length)
    {
      max_length = current_length;
    }
  }

  return max_length;
}

int findMaxAllocLength(int number_of_customers, int number_of_resources)
{
  int max_length = 0;

  for (int i = 0; i < number_of_customers; i++)
  {
    int current_length = 0;

    for (int j = 0; j < number_of_resources; j++)
    {
      current_length += countDigits(customers[i].allocation[j]);

      current_length += 1;
    }
    if (current_length > max_length)
    {
      max_length = current_length;
    }
  }

  return max_length;
}
