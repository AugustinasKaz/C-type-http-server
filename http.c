#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

char *decode_request(char buffer[], long valread);
char *read_file(char *file);
int main()
{
    int sock, new_sock;
    long valread;
    struct sockaddr_in address;
    char buffer[30000] = {0}, *result;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(sock, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_sock = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        valread = read(new_sock, buffer, 30000);
        result = decode_request(buffer, valread);
        write(new_sock, result, strlen(result));
        free(result);
        close(new_sock);
    }
    return 0;
}

char *decode_request(char buffer[], long valread)
{
    int j = 0, tmp = 0;
    char *request_method=calloc(10, sizeof(char)),request_resource[20]="",file_name[80]="";
    for (int i = 0; i < valread; i++)
        printf("%c",buffer[i]);

    for (int i = 0; i < valread; i++)
    {
        if (buffer[i] == ' ')
        {
            tmp = i + 2;
            break;
        }
        else
            request_method[i] = buffer[i];
    }
    if(strcmp(request_method, "GET") == 0){
        for (int i = tmp; i < valread; i++){
            if (buffer[i] == ' ')
                break;
            else{
                request_resource[j] = buffer[i];
                j++;
            }
        }
    }
    else if(strcmp(request_method, "POST") == 0){
        for (int i = tmp; i < valread; i++){
            if (buffer[i] == ' ')
                break;
            else{
                request_resource[j] = buffer[i];
                j++;
            }
        }
    }
    else{
        perror("error: ");
        return "failed";
    }
    sprintf(file_name, "templates/%s", request_resource);
    char *test = read_file(file_name);
    return test;
}

char *read_file(char *file)
{
    char *duom;
    long duom_size;
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        perror("error in file open: ");
        FILE *fd = fopen("templates/error.html", "r");
        if(fd == NULL)
            return "failed to open";
        fseek(fd, 0, SEEK_END);
        duom_size = ftell(fd) - 1;
        rewind(fd);
        duom = malloc(sizeof(char) * duom_size);
        char *header = malloc(sizeof(char) * (duom_size + 100));
        fread(duom, 1, duom_size, fd);
        fclose(fd);
        sprintf(header, "HTTP/1.1 404 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: %ld\n\n%s", duom_size, duom);
        free(duom);
        return header;
    }
    fseek(fp, 0, SEEK_END);
    duom_size = ftell(fp) - 1;
    rewind(fp);
    duom = (char *)malloc(sizeof(char) * duom_size);
    char *header = malloc(sizeof(char) * (duom_size + 100));
    fread(duom, 1, duom_size, fp);
    fclose(fp);
    sprintf(header, "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: %ld\n\n%s", duom_size, duom);
    free(duom);
    return header;
}

