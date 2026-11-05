#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // pipe(), fork(), dup2(), execvp()
#include <sys/wait.h>  // wait()

int main() {
    int fd[2];  // fd[0] = lato lettura, fd[1] = lato scrittura

    printf("[Padre] PID: %d -- creo la pipe\n", getpid());

    // Crea la pipe anonima prima delle fork, cosi' i figli la ereditano
    if (pipe(fd) == -1) {
        perror("Errore pipe");
        exit(EXIT_FAILURE);
    }

    fflush(stdout);

    pid_t pid1 = fork();

    if (pid1 == -1) {
        // fork() fallita
        perror("Errore fork (figlio 1)");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // PROCESSO FIGLIO 1
        close(fd[0]);                // chiude il lato lettura (non serve)
        dup2(fd[1], STDOUT_FILENO);  // stdout ora punta al lato scrittura
        close(fd[1]);                // fd[1] originale non serve piu'

        char *args[] = { "ls", "/tmp", NULL };
        execvp("ls", args);

        // Se arriviamo qui, execvp() ha fallito
        perror("Errore execvp (ls)");
        exit(EXIT_FAILURE);
    }

    // Secondo figlio: wc -l (legge dalla pipe)
    pid_t pid2 = fork();

    if (pid2 == -1) {
        // fork() fallita
        perror("Errore fork (figlio 2)");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // PROCESSO FIGLIO 2
        close(fd[1]);               // chiude il lato scrittura (non serve)
        dup2(fd[0], STDIN_FILENO);  // stdin ora punta al lato lettura
        close(fd[0]);               // fd[0] originale non serve piu'

        char *args[] = { "wc", "-l", NULL };
        execvp("wc", args);

        // Se arriviamo qui, execvp() ha fallito
        perror("Errore execvp (wc)");
        exit(EXIT_FAILURE);
    }

    // PROCESSO PADRE
    printf("[Padre] creati i figli: PID %d (ls) e PID %d (wc)\n", pid1, pid2);
    fflush(stdout);

    // Il padre chiude entrambi i lati della pipe.
    close(fd[0]);
    close(fd[1]);

    // Aspetta entrambi i figli
    int status;
    wait(&status);
    wait(&status);

    printf("[Padre] pipeline terminata.\n");
    return 0;
}
