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

// FUNCOES PARA VERIFICAR SE O ARQUIVO ESTA FORMATADO CORRETAMENTE
int verifyCustomerFile(char *customerFile);
int verifyCommandsFile(char *commandsFile);

int countCustomerResources(char *customerFile);
int countCommandsResources(char *commandsFile);

// FUNCOES PARA CONTAR QUANTIDADE DE CUSTOMERS E COMANDOS
int countCustomer(char *customerFile);
int countCommands(char *commandsFile);

void readCustomerFile(int number_of_customers, int number_of_resources);
void readCommandFile(int number_of_resources, int number_of_customers);
void executeCommand(char *command, int number_of_resources, int number_of_customers);
void allocateResources(int customer_num, int request[], int number_of_resources, int number_of_customers);
void printState(int number_of_customers, int number_of_resources);
void releaseResources(int customer_num, int release[], int number_of_resources);
void calculateNeed(int number_of_customers, int number_of_resources);
int checkSafety(int number_of_resources, int number_of_customers);

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
  int numTokens = 0; // Variável para armazenar a quantidade de elementos na primeira linha
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

    // Ignorar linhas que começam com '*'
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

  if (strncmp(command, "*", 1) == 0)
  {
    printState(number_of_customers, number_of_resources);
    free(resources);
    return;
  }

  mode = strtok(command, " ");
  printf("%s\n", mode);

  if (strcmp(mode, "RQ") == 0)
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
      printf("%d\n", resources[i]);
    }
    releaseResources(customer, resources, number_of_resources);
  }

  free(resources);
}

void allocateResources(int customer_num, int request[], int number_of_resources, int number_of_customers)
{
  FILE *outputFile = fopen("results.txt", "a");

  if (outputFile == NULL)
  {
    perror("Error opening results.txt");
    return;
  }

  for (int i = 0; i < number_of_resources; i++)
  {
    if (request[i] > customers[customer_num].need[i])
    {
      fprintf(outputFile, "The customer %d request ", customer_num);
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
      fprintf(outputFile, " are not enough to customer %d the request", customer_num);
      for (int j = 0; j < number_of_resources; j++)
      {
        fprintf(outputFile, " %d", request[j]);
      }
      fprintf(outputFile, "\n");

      return;
    }
  }

  for (int i = 0; i < number_of_resources; i++)
  {
    available[i] -= request[i];
    customers[customer_num].allocation[i] += request[i];
    customers[customer_num].need[i] -= request[i];
  }

  if (!checkSafety(number_of_resources, number_of_customers))
  {
    for (int i = 0; i < number_of_resources; i++)
    {
      available[i] += request[i];
      customers[customer_num].allocation[i] -= request[i];
      customers[customer_num].need[i] += request[i];
    }
    fprintf(outputFile, "The customer %d request", customer_num);
    for (int i = 0; i < number_of_resources; i++)
    {
      fprintf(outputFile, " %d", request[i]);
    }
    fprintf(outputFile, " was denied because result in an unsafe state\n");
  }
  else
  {
    fprintf(outputFile, "Allocate to customer %d the resources", customer_num);
    for (int i = 0; i < number_of_resources; i++)
    {
      fprintf(outputFile, " %d", request[i]);
    }
    fprintf(outputFile, "\n");
  }

  fclose(outputFile);
}

int checkSafety(int number_of_resources, int number_of_customers)
{
  int work[number_of_resources];
  int finish[number_of_customers];

  for (int i = 0; i < number_of_resources; i++)
  {
    work[i] = available[i];
  }

  for (int i = 0; i < number_of_customers; i++)
  {
    finish[i] = 0;
  }

  for (int i = 0; i < number_of_customers; i++)
  {
    if (finish[i] == 0)
    {
      int canAllocate = 1;

      for (int j = 0; j < number_of_resources; j++)
      {
        if (customers[i].need[j] > work[j])
        {
          canAllocate = 0;
          break;
        }
      }

      if (canAllocate)
      {
        for (int j = 0; j < number_of_resources; j++)
        {
          work[j] += customers[i].allocation[j];
        }
        finish[i] = 1;

        i = -1;
      }
    }
  }

  for (int i = 0; i < number_of_customers; i++)
  {
    if (finish[i] == 0)
    {
      return 0;
    }
  }

  return 1;
}

void calculateNeed(int number_of_customers, int number_of_resources)
{
  for (int i = 0; i < number_of_customers; i++)
  {
    for (int j = 0; j < number_of_resources; j++)
    {
      customers[i].need[j] = customers[i].max[j] - customers[i].allocation[j];
    }
  }
}

void releaseResources(int customer_num, int release[], int number_of_resources)
{
  FILE *outputFile = fopen("results.txt", "a");

  if (outputFile == NULL)
  {
    perror("Error opening results.txt");
    return;
  }
  printf("NUMERO DE RECURSOS AQUI-> %d\n", number_of_resources);

  for (int i = 0; i < number_of_resources; i++)
  {
    if (release[i] > customers[customer_num].allocation[i])
    {
      fprintf(outputFile, "The customer %d release", customer_num);
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
    customers[customer_num].allocation[i] -= release[i];
    customers[customer_num].need[i] += release[i];
  }

  fprintf(outputFile, "Release from customer %d the resources", customer_num);

  for (int i = 0; i < number_of_resources; i++)
  {
    fprintf(outputFile, " %d", release[i]);
  }

  fprintf(outputFile, "\n");
  fclose(outputFile);
}

void printState(int number_of_customers, int number_of_resources)
{
  FILE *outputFile = fopen("results.txt", "a");

  if (outputFile == NULL)
  {
    perror("Error opening results.txt");
    return;
  }

  fprintf(outputFile, "MAXIMUM | ALLOCATION | NEED\n");

  for (int i = 0; i < number_of_customers; i++)
  {
    for (int j = 0; j < number_of_resources; j++)
    {
      fprintf(outputFile, "%d ", customers[i].max[j]);
    }
    fprintf(outputFile, "| ");

    for (int j = 0; j < number_of_resources; j++)
    {
      fprintf(outputFile, "%d ", customers[i].allocation[j]);
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
