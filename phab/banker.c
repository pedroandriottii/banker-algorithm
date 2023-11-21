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

// FUNCOES PARA VERIFICAR SE O ARQUIVO ESTA FORMATADO CORRETAMENTE
int verifyCustomerFile(char *customerFile);
int verifyCommandsFile(char *commandsFile);

int countCustomerResources(char *customerFile);
int countCommandsResources(char *commandsFile);

// FUNCOES PARA CONTAR QUANTIDADE DE CUSTOMERS E COMANDOS
int countCustomer(char *customerFile);
int countCommands(char *commandsFile);

int main(int argc, char *argv[])
{
  char *customerFile = "customer.txt";
  char *commandsFile = "commands.txt";
  int number_of_customers;
  int number_of_resources;
  int *avaible;
  Customer *customers;

  number_of_resources = argc - 1;
  avaible = (int *)malloc(number_of_resources * sizeof(int));

  // Armazenando os valores dos recursos disponiveis
  int i;
  for (i = 0; i < number_of_resources; i++)
  {
    avaible[i] = atoi(argv[i + 1]);
    printf("Resource %c -> %d\n", i + 65, avaible[i]);
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

  printf("Customers -> %d | Commands -> %d", countCustomer(customerFile), countCommands(commandsFile));

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
