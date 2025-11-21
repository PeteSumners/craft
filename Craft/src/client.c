#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #define close closesocket
    #define sleep Sleep
#else
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "tinycthread.h"

#define QUEUE_SIZE 1048576
#define RECV_SIZE 4096

typedef struct {
    int enabled;
    int running;
    int sd;
    int bytes_sent;
    int bytes_received;
    char *queue;
    int qsize;
    thrd_t recv_thread;
    mtx_t mutex;
} Client;

static Client client = {0};

void client_enable() {
    client.enabled = 1;
}

void client_disable() {
    client.enabled = 0;
}

int get_client_enabled() {
    return client.enabled;
}

int client_sendall(int sd, char *data, int length) {
    if (!client.enabled) {
        return 0;
    }
    int count = 0;
    while (count < length) {
        int n = send(sd, data + count, length, 0);
        if (n == -1) {
            return -1;
        }
        count += n;
        length -= n;
        client.bytes_sent += n;
    }
    return 0;
}

void client_send(char *data) {
    if (!client.enabled) {
        return;
    }
    if (client_sendall(client.sd, data, strlen(data)) == -1) {
        perror("client_send failed");
        fprintf(stderr, "Network error - disabling client\n");
        client_disable();
    }
}

void client_version(int version) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "V,%d\n", version);
    client_send(buffer);
}

void client_login(const char *username, const char *identity_token) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "A,%s,%s\n", username, identity_token);
    client_send(buffer);
}

void client_position(float x, float y, float z, float rx, float ry) {
    if (!client.enabled) {
        return;
    }
    static float px, py, pz, prx, pry = 0;
    float distance =
        (px - x) * (px - x) +
        (py - y) * (py - y) +
        (pz - z) * (pz - z) +
        (prx - rx) * (prx - rx) +
        (pry - ry) * (pry - ry);
    if (distance < 0.0001) {
        return;
    }
    px = x; py = y; pz = z; prx = rx; pry = ry;
    char buffer[1024];
    snprintf(buffer, 1024, "P,%.2f,%.2f,%.2f,%.2f,%.2f\n", x, y, z, rx, ry);
    client_send(buffer);
}

void client_chunk(int p, int q, int key) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "C,%d,%d,%d\n", p, q, key);
    client_send(buffer);
}

void client_block(int x, int y, int z, int w) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "B,%d,%d,%d,%d\n", x, y, z, w);
    client_send(buffer);
}

void client_light(int x, int y, int z, int w) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "L,%d,%d,%d,%d\n", x, y, z, w);
    client_send(buffer);
}

void client_sign(int x, int y, int z, int face, const char *text) {
    if (!client.enabled) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "S,%d,%d,%d,%d,%s\n", x, y, z, face, text);
    client_send(buffer);
}

void client_talk(const char *text) {
    if (!client.enabled) {
        return;
    }
    if (strlen(text) == 0) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "T,%s\n", text);
    client_send(buffer);
}

char *client_recv() {
    if (!client.enabled) {
        return 0;
    }
    char *result = 0;
    mtx_lock(&client.mutex);
    char *p = client.queue + client.qsize - 1;
    while (p >= client.queue && *p != '\n') {
        p--;
    }
    if (p >= client.queue) {
        int length = p - client.queue + 1;
        result = malloc(sizeof(char) * (length + 1));
        memcpy(result, client.queue, sizeof(char) * length);
        result[length] = '\0';
        int remaining = client.qsize - length;
        memmove(client.queue, p + 1, remaining);
        client.qsize -= length;
        client.bytes_received += length;
    }
    mtx_unlock(&client.mutex);
    return result;
}

int recv_worker(void *arg) {
    char *data = malloc(sizeof(char) * RECV_SIZE);
    while (1) {
        int length;
        if ((length = recv(client.sd, data, RECV_SIZE - 1, 0)) <= 0) {
            if (client.running) {
                perror("recv failed");
                fprintf(stderr, "Connection lost - disabling client\n");
                client_disable();
                break;
            }
            else {
                break;
            }
        }
        data[length] = '\0';
        while (1) {
            int done = 0;
            mtx_lock(&client.mutex);
            if (client.qsize + length < QUEUE_SIZE) {
                memcpy(client.queue + client.qsize, data, sizeof(char) * (length + 1));
                client.qsize += length;
                done = 1;
            }
            mtx_unlock(&client.mutex);
            if (done) {
                break;
            }
            sleep(0);
        }
    }
    free(data);
    return 0;
}

int client_connect(char *hostname, int port) {
    if (!client.enabled) {
        return -1;
    }
    struct hostent *host;
    struct sockaddr_in address;
    if ((host = gethostbyname(hostname)) == 0) {
        perror("gethostbyname");
        client_disable();
        return -1;
    }
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = ((struct in_addr *)(host->h_addr_list[0]))->s_addr;
    address.sin_port = htons(port);
    if ((client.sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        client_disable();
        return -1;
    }
    if (connect(client.sd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("connect");
        client_disable();
        return -1;
    }
    return 0;
}

int client_start() {
    if (!client.enabled) {
        return -1;
    }
    client.running = 1;
    client.queue = (char *)calloc(QUEUE_SIZE, sizeof(char));
    if (!client.queue) {
        perror("calloc failed");
        client_disable();
        return -1;
    }
    client.qsize = 0;
    mtx_init(&client.mutex, mtx_plain);
    if (thrd_create(&client.recv_thread, recv_worker, NULL) != thrd_success) {
        perror("thrd_create");
        free(client.queue);
        client.queue = NULL;
        mtx_destroy(&client.mutex);
        client_disable();
        return -1;
    }
    return 0;
}

void client_stop() {
    if (!client.enabled) {
        return;
    }
    client.running = 0;
    close(client.sd);
    if (thrd_join(client.recv_thread, NULL) != thrd_success) {
        perror("thrd_join");
    }
    mtx_destroy(&client.mutex);
    client.qsize = 0;
    free(client.queue);
    client.queue = NULL;
}
