#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX 1024
bool esperarTermino = true;

void imprimirDiretorio()
{
    char dir[MAX];
    // pegando o diretorio atual
    getcwd(dir, sizeof(dir));
    // imprimindo diretorio atual
    printf("\n %s $ ", dir);
}

char **getComandos(char *input)
{
    char **comandos = malloc(8 * sizeof(char *));
    char *separador = " ";
    char *comando;
    int index = 0;

    comando = strtok(input, separador);
    while (comando != NULL)
    {
        comandos[index] = comando;
        index++;

        comando = strtok(NULL, separador);
    }
    
    // verificando se informado & não espera o termino da exec
    // do processo filho
    if (strcmp(comandos[index - 1], "&") == 0)
    {
        esperarTermino = false;
    }

    //execvp necessita de null no ultimo argumento
    comandos[index] = NULL;
    return comandos;
}

int main()
{
    char **comandos;
    char input[MAX];
    pid_t child_pid;
    int child_status;

    while (1)
    {
        imprimirDiretorio();

        // realizando leitura da entrada do usuario
        fgets(input, MAX, stdin);

        //realizando a limpeza da String para retirar o \n que captura junto com o fgets
        input[strcspn(input, "\n")] = 0;

        // separando a entrada do usuario em um array
        comandos = getComandos(input);

        if (strcmp(comandos[0], "exit") == 0)
        {
            exit(0);
        }
        else if (strcmp(comandos[0], "cd") == 0)
        {
            // se digitar somente cd vai para o dir home do usuario
            if (comandos[1] == NULL)
            {
                // informando como argumento o caminho para home do usuario
                comandos[1] = getenv("HOME");
            }
            chdir(comandos[1]);
        }
        else
        {
            // realizando o fork
            child_pid = fork();

            if (child_pid == 0)
            {
                // execucao do processo filho
                execvp(comandos[0], comandos);
                printf("Erro ao executar comando\n");
            }
            else if (esperarTermino)
            {
                // aguardando termino do processo filho
                waitpid(child_pid, &child_status, WUNTRACED);
            }
            esperarTermino = true;

            // liberando espaço da memoria
            free(comandos);
        }
    }

    return 0;
}