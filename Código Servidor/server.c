#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // read(), write(), close()
#include <sys/socket.h>  // API sockets
#include <sys/un.h>      // AF_UNIX (endereços de arquivos)
#include <netinet/in.h>  // AF_INET (endereços de rede)
#include <pthread.h>     // Biblioteca POSIX para uso de threads

#define PORT 8080
#define BUFFER_SIZE 1024
#define AI_SOCKET_PATH "/tmp/ai_socket"

// Função para enviar dados para Python por socket Unix
void consultar_ia(const char *entrada, char *saida) {
    int py_sock;
    struct sockaddr_un py_addr;

    // Criar socket Unix local
    if ((py_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        strcpy(saida, "Erro ao criar socket Unix\n");
        return;
    }

    py_addr.sun_family = AF_UNIX;
    strncpy(py_addr.sun_path, AI_SOCKET_PATH, sizeof(py_addr.sun_path) - 1);

    // Conectar ao socket do Python
    if (connect(py_sock, (struct sockaddr *)&py_addr, sizeof(py_addr)) < 0) {
        strcpy(saida, "Erro ao conectar ao socket Python\n");
        close(py_sock);
        return;
    } 

    // Enviar a entrada para o Python
    send(py_sock, entrada, strlen(entrada), 0);
    
    // Ler a resposta retornada pela IA
    int bytes_recebidos = read(py_sock, saida, BUFFER_SIZE - 1);
    if (bytes_recebidos > 0) {
        saida[bytes_recebidos] = '\0'; // Garante a terminação da string
    }

    close(py_sock);  
}

// Função de tratamento que será executada por cada thread em paralelo
void *tratar_cliente(void *socket_desc) {
    // Recupera o valor do socket do cliente e liberta a memória alocada na main
    int client_socket = *(int*)socket_desc;
    free(socket_desc);

    char buffer[BUFFER_SIZE] = {0};
    char ai_response[BUFFER_SIZE] = {0};

    // Lê a requisição do cliente externo
    int bytes_lidos = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (bytes_lidos > 0) {
        buffer[bytes_lidos] = '\0'; // Garante o fim correto da string
        printf("[Thread ID: %lu] Pacote recebido para análise...\n", pthread_self());

        // Consulta a IA enviando os dados brutos recebidos
        consultar_ia(buffer, ai_response);

        // Devolve a resposta processada de volta ao cliente
        send(client_socket, ai_response, strlen(ai_response), 0);
    }
    
    // Fecha a conexão do cliente atual e encerra a execução da thread
    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Configurar o socket externo (AF_INET)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erro ao criar socket do servidor");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro ao realizar o bind");
        exit(EXIT_FAILURE);
    }

    // Aumentada a fila para suportar múltiplas conexões pendentes
    if (listen(server_fd, 500) < 0) {
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor rodando na porta %d (Modo Multi-Thread)\n", PORT);

    // Loop principal para aceitar conexões simultâneas
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) continue;
        
        pthread_t thread_id;
        
        // Alocação dinâmica de memória para evitar sobreposição de descritores no loop
        int *new_sock = malloc(sizeof(int));
        if (new_sock == NULL) {
            perror("Erro ao alocar memória");
            close(new_socket);
            continue;
        }
        *new_sock = new_socket;
        
        // Cria a thread passando a função de tratamento e o ponteiro do socket
        if (pthread_create(&thread_id, NULL, tratar_cliente, (void*) new_sock) < 0) {
            perror("Erro ao criar thread");
            free(new_sock);
            close(new_socket);
            continue;
        }
        
        // Desanexa a thread para que o SO limpe automaticamente os seus recursos ao terminar
        pthread_detach(thread_id);
    }   

    close(server_fd);
    return 0;
}
