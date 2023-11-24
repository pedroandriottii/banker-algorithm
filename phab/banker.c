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
void printCustomers(int number_of_customers, int number_of_resources);
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
  printCustomers(number_of_customers, number_of_resources);
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

// -- OK --
void executeCommand(char *command, int number_of_resources, int number_of_customers)
{
  char mode[3];
  int customer;
  int resources[number_of_resources];

  sscanf(command, "%s", mode);

  if (strcmp(mode, "RQ") == 0)
  {
    sscanf(command, "%s %d", mode, &customer);
    // Lê os recursos solicitados e realiza a requisição
    for (int i = 0; i < number_of_resources; i++)
    {
      sscanf(command, "%d", &resources[i]);
    }
    allocateResources(customer, resources, number_of_resources, number_of_customers);
  }
  else if (strcmp(mode, "RL") == 0)
  {
    sscanf(command, "%s %d", mode, &customer);
    // Lê os recursos a serem liberados e realiza a liberação
    for (int i = 0; i < number_of_resources; i++)
    {
      sscanf(command, "%d", &resources[i]);
    }
    releaseResources(customer, resources, number_of_customers);
  }
  else if (strcmp(mode, "*") == 0)
  {
    printState(number_of_resources, number_of_customers);
  }
}

void allocateResources(int customer_num, int request[], int number_of_resources, int number_of_customers)
{
  // Verifica se o pedido excede a necessidade máxima
  for (int i = 0; i < number_of_resources; i++)
  {
    if (request[i] > customers[customer_num].need[i])
    {
      printf("The customer %d request exceeds its max need\n", customer_num);
      return;
    }
  }

  // Verifica se os recursos estão disponíveis
  for (int i = 0; i < number_of_resources; i++)
  {
    if (request[i] > available[i])
    {
      printf("The resources are not available for customer %d\n", customer_num);
      return;
    }
  }

  // Aloca os recursos e verifica se o estado continua seguro
  for (int i = 0; i < number_of_resources; i++)
  {
    available[i] -= request[i];
    customers[customer_num].allocation[i] += request[i];
    customers[customer_num].need[i] -= request[i];
  }

  if (!checkSafety(number_of_resources, number_of_customers))
  {
    // Se não estiver seguro, reverte a alocação
    for (int i = 0; i < number_of_resources; i++)
    {
      available[i] += request[i];
      customers[customer_num].allocation[i] -= request[i];
      customers[customer_num].need[i] += request[i];
    }
    printf("The customer %d request results in an unsafe state\n", customer_num);
  }
  else
  {
    printf("Allocate to customer %d the resources", customer_num);
    for (int i = 0; i < number_of_resources; i++)
    {
      printf(" %d", request[i]);
    }
    printf("\n");
  }
}

int checkSafety(int number_of_resources, int number_of_customers)
{
  int work[number_of_resources];
  int finish[number_of_customers];

  // Inicializa o vetor de trabalho com os recursos disponíveis
  for (int i = 0; i < number_of_resources; i++)
  {
    work[i] = available[i];
  }

  // Inicializa o vetor de finalização (finish) como falso para todos os clientes
  for (int i = 0; i < number_of_customers; i++)
  {
    finish[i] = 0;
  }

  // Algoritmo principal de verificação de segurança
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

        // Reinicia a busca
        i = -1;
      }
    }
  }

  // Verifica se todos os clientes podem terminar
  for (int i = 0; i < number_of_customers; i++)
  {
    if (finish[i] == 0)
    {
      return 0; // O sistema não está em um estado seguro
    }
  }

  return 1; // O sistema está em um estado seguro
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
  // Verifica se o cliente está tentando liberar mais recursos do que foi alocado
  for (int i = 0; i < number_of_resources; i++)
  {
    if (release[i] > customers[customer_num].allocation[i])
    {
      printf("The customer %d release exceeds its allocation\n", customer_num);
      return;
    }
  }

  // Libera os recursos
  for (int i = 0; i < number_of_resources; i++)
  {
    available[i] += release[i];
    customers[customer_num].allocation[i] -= release[i];
    customers[customer_num].need[i] += release[i];
  }

  printf("Release from customer %d the resources", customer_num);
  for (int i = 0; i < number_of_resources; i++)
  {
    printf(" %d", release[i]);
  }
  printf("\n");
}

void printState(int number_of_customers, int number_of_resources)
{
  printf("Current State:\n");
  printf("MAXIMUM | ALLOCATION | NEED\n");

  for (int i = 0; i < number_of_customers; i++)
  {
    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].max[j]);
    }
    printf("| ");

    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].allocation[j]);
    }
    printf("| ");

    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].need[j]);
    }
    printf("\n");
  }

  printf("AVAILABLE ");
  for (int i = 0; i < number_of_resources; i++)
  {
    printf("%d ", available[i]);
  }
  printf("\n");
}

void printCustomers(int number_of_customers, int number_of_resources)
{
  for (int i = 0; i < number_of_customers; i++)
  {
    printf("Customer %d:\n", i + 1);

    printf("Maximum: ");
    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].max[j]);
    }
    printf("\n");

    printf("Allocation: ");
    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].allocation[j]);
    }
    printf("\n");

    printf("Need: ");
    for (int j = 0; j < number_of_resources; j++)
    {
      printf("%d ", customers[i].need[j]);
    }
    printf("\n\n");
  }
}
