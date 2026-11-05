#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    /* fork(), execvp() */
#include <sys/wait.h>  /* wait() */

int main() {
    printf("[Padre] PID: %d -- sto per fare fork()\n", getpid());

    char *cmd1[] = {"date", NULL};
    char *cmd2[] = {"echo", "ciao", "dal", "figlio",  NULL};
    char *cmd3[] = {"hostname", NULL};
    char **comandi[] = { cmd1, cmd2, cmd3};


    for (int i = 0; i<3; i++) {
        fflush(stdout);
        pid_t pid = fork();

        if (pid == -1) {
            /* fork() fallita */
            perror("Errore fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            /* === PROCESSO FIGLIO === */
            printf("[Figlio] PID: %d -- chiamo execvp(%s)\n", getpid(), comandi[i][0]);
            fflush(stdout);

            execvp(comandi[i][0], comandi[i]);

            perror("Errore execvp");
            exit(EXIT_FAILURE);
        }

        /* === PROCESSO PADRE === */
        /* pid contiene il PID del figlio */
        printf("[Padre] ho creato il figlio con PID %d\n", pid);

        /* Aspetta che il figlio termini */
        int status;
        wait(&status);

        if (WIFEXITED(status))
            printf("[Padre] figlio terminato con codice %d\n",
                   WEXITSTATUS(status));


    }
    printf("[Padre] continuo la mia esecuzione.\n");

return 0;
}