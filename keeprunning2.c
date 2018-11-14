#include <stdio.h>
#include <unistd.h>
#include <signal.h>



void handle_SIGINT(int sig) {
    printf("lol u loser");
}

void handle2(int sig) {
    printf("wait for a second\n");
    usleep(1000000);
    printf("keep waiting \n");
    usleep(10000000);
    printf("u look like an idiot rn \n");
    usleep(3000000);
}

int main(void) {

    int x = 0;

    /* let's ignore signal 2 */
    signal(2, handle_SIGINT);
    signal(9, SIG_IGN);
    signal(15, SIG_IGN);
    signal(19, SIG_IGN);
    signal(20, handle2);

    while(1) {
        printf("%d\n", x);
        x++;
        usleep(100000);
    }

}