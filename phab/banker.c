#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Customer
{
  int *max;
  int *allocation;
  int *need;
} Customer;

// FUNCOES PARA VERIFICAR SE ESTA FORMATADO CORRETAMENTE
int verifyCustomerFile(char *customerFile);
int verifyCommandsFile(char *commandsFile);

// FUNCOES PARA CONTAR QUANTIDADE DE CUSTOMERS E COMANDOS
int countCustomer(char *customerFile);
int countCommands(char *commandsFile);

int main(int argc, char *argv[])
{
  int NUMBER_OF_CUSTOMERS;
  int NUMBER_OF_RESOURCES;
  int *avaible;
  Customer *customers;

  NUMBER_OF_RESOURCES = argc - 1;
  avaible = (int *)malloc(NUMBER_OF_RESOURCES * sizeof(int));

  int i;
  for (i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    avaible[i] = atoi(argv[i + 1]);
    printf("Resource %d -> %d\n", i, avaible[i]);
  }

  char *customerFile = "customer.txt";
  char *commandsFile = "commands.txt";

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
