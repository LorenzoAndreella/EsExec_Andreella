#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    /* fork(), execvp() */
#include <sys/wait.h>  /* wait(), waitpid() */

/* Raccoglie i figli gia' terminati SENZA bloccarsi */
void raccogli_figli() {
    int status;
    pid_t terminato;

    /* WNOHANG: se nessun figlio e' terminato, waitpid ritorna 0 subito */
    while ((terminato = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status))
            printf("[Padre] figlio %d terminato con codice %d\n",
                   terminato, WEXITSTATUS(status));
    }
}

int main() {
    char comando[256];

    printf("[Padre] PID: %d -- mini shell avviata (scrivi 'exit' per uscire)\n", getpid());

    while (1) {
        /* Prima di ogni prompt raccoglie i figli finiti nel frattempo */
        raccogli_figli();

        printf("miniShell> ");
        fflush(stdout);

        if (fgets(comando, sizeof(comando), stdin) == NULL)
            break;

        /* Toglie il '\n' finale */
        comando[strcspn(comando, "\n")] = '\0';

        /* Riga vuota: richiede un nuovo comando */
        if (comando[0] == '\0')
            continue;

        /* Comando di uscita */
        if (strcmp(comando, "exit") == 0)
            break;

        fflush(stdout);
        pid_t pid = fork();

        if (pid == -1) {
            /* fork() fallita */
            perror("Errore fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            /* === PROCESSO FIGLIO === */
            /* Comando senza argomenti: args[] contiene solo il nome */
            char *args[] = { comando, NULL };

            execvp(comando, args);

            /* Se arriviamo qui, execvp() ha fallito (comando inesistente) */
            perror("Errore execvp");
            exit(EXIT_FAILURE);
        }

        /* === PROCESSO PADRE === */
        /* NIENTE wait(): torna subito al prompt */
        printf("[Padre] lanciato figlio con PID %d\n", pid);
    }

    /* Prima di uscire aspetta i figli ancora in esecuzione */
    while (wait(NULL) > 0);

    printf("[Padre] mini shell terminata.\n");
    return 0;
}