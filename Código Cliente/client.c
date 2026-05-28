#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Bibliotecas de socket
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORTA 8080
#define BUFFER_SIZE 1024

int main() {

    int cliente_fd;

    struct sockaddr_in servidor;

    char mensagem[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};

    // 1. Criar socket
    cliente_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (cliente_fd == -1) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket do cliente criado.\n");

    // 2. Configurar endereço do servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORTA);

    // IP do servidor
    servidor.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Conectar ao servidor
    if (connect(cliente_fd,
               (struct sockaddr *)&servidor,
               sizeof(servidor)) < 0) {

        perror("Erro ao conectar");
        close(cliente_fd);
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor.\n");

    // 4. Ler mensagem do usuário
    printf("Digite uma mensagem: ");
    fgets(mensagem, BUFFER_SIZE, stdin);

    // Remover quebra de linha
    mensagem[strcspn(mensagem, "\n")] = '\0';

    // 5. Enviar mensagem
    send(cliente_fd, mensagem, strlen(mensagem), 0);

    printf("Mensagem enviada.\n");

    // 6. Receber resposta
    recv(cliente_fd, buffer, BUFFER_SIZE, 0);

    printf("Resposta do servidor: %s\n", buffer);

    // 7. Fechar conexão
    close(cliente_fd);

    return 0;
}
