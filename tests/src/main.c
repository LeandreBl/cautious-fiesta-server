/*
** EPITECH PROJECT, 2018
** test
** File description:
** test
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <stdio.h>
#define _XOPEN_SOURCE
#include <time.h>
#include <string.h>
#include <linux/fb.h>
#include <readline/readline.h>
#include <assert.h>
#include <pthread.h>

#include <lbuffer.h>
#include <lsocket.h>
#include <lserver.h>
#include <lstr.h>

enum pktType_e {
	LOGIN = 0,
	LOGOUT,
	CREATE_GAMESESSION,
	DELETE_GAMESESSION,
	GET_GAMESESSIONS_LIST,
	JOIN_GAMESESSION,
	LEAVE_GAMESESSION,
	GET_GAMESESSION_PLAYERS_LIST,
	SEND_MESSAGE,
	RECEIVE_MESSAGE,
	TOGGLE_READY,
	ASSETS_REQUIREMENT,
	ASSETS_LOADER,
	GAME_STARTED,
	ACK, /* this should stay the last enum type */
};

typedef struct packet_s {
	uint32_t magic;
	uint32_t type;
	uint64_t len;
} packet_t;

typedef struct jumper_s {
	uint32_t type;
	const char *command;
	void (*sender)(lclient_t *client, const char *argument,
		       packet_t *header);
	void (*receiver)(lclient_t *client);
} jumper_t;

void packet_create(packet_t *pkt, uint32_t type, uint32_t len)
{
	pkt->magic = 0x0b3a4;
	pkt->type = type;
	pkt->len = len;
}

void packet_display(packet_t *packet)
{
	printf("PACKET: {magic: \"%x\", pktType: \"%u\", pktLen: \"%lu\"}\n",
	       packet->magic, packet->type, packet->len);
}

void send_header(lclient_t *client, packet_t *header)
{
	write(client->socket.fd, header, sizeof(*header));
}

void send_str(lclient_t *client, const char *str)
{
	uint64_t len = strlen(str);

	write(client->socket.fd, &len, sizeof(len));
	write(client->socket.fd, str, len);
}

void receive_str(lclient_t *client, char **str)
{
	uint64_t len;

	lbuffer_read(&client->buffer, &len, sizeof(len));
	*str = calloc(len + 1, sizeof(char));
	if (*str == NULL)
		return;
	lbuffer_read(&client->buffer, *str, len);
}

void login_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = 8 + strlen(argument);
	send_header(client, header);
	send_str(client, argument);
}

void login_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void logout_sender(lclient_t *client, const char *argument, packet_t *header)
{
	(void)argument;
	send_header(client, header);
}

void logout_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void create_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = strlen(argument) + 8;
	send_header(client, header);
	send_str(client, argument);
}

void create_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void delete_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = strlen(argument) + 8;
	send_header(client, header);
	send_str(client, argument);
}

void delete_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void getgames_sender(lclient_t *client, const char *argument, packet_t *header)
{
	(void)argument;
	send_header(client, header);
}

void getgames_receiver(lclient_t *client)
{
	uint64_t len;
	uint64_t nb;
	char *name;

	lbuffer_read(&client->buffer, &len, sizeof(len));
	printf("%s: [%lu] GameSessions:\n", __FUNCTION__, len);
	while (len > 0) {
		lbuffer_read(&client->buffer, &nb, sizeof(nb));
		receive_str(client, &name);
		printf("\t{%lu - \"%s\"}\n", nb, name);
		free(name);
		--len;
	}
}

void join_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = strlen(argument) + 8;
	send_header(client, header);
	send_str(client, argument);
}

void join_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void leave_sender(lclient_t *client, const char *argument, packet_t *header)
{
	(void)argument;
	send_header(client, header);
}

void leave_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void getplayers_sender(lclient_t *client, const char *argument,
		       packet_t *header)
{
	header->len = strlen(argument) + 8;
	send_header(client, header);
	send_str(client, argument);
}

void getplayers_receiver(lclient_t *client)
{
	uint8_t answer;
	uint64_t size;
	uint8_t rdy;
	char *name;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	if (answer == 0) {
		printf("%s: %u\n", __FUNCTION__, answer);
		return;
	}
	receive_str(client, &name);
	lbuffer_read(&client->buffer, &size, sizeof(size));
	printf("%s: [%lu] %s\n", __FUNCTION__, size, name);
	while (size > 0) {
		lbuffer_read(&client->buffer, &rdy, sizeof(rdy));
		receive_str(client, &name);
		printf("{ <%u> \"%s\"}\n", rdy, name);
		free(name);
		--size;
	}
}

void send_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = strlen(argument) + 8;
	send_header(client, header);
	send_str(client, argument);
}

void send_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %u\n", __FUNCTION__, answer);
}

void receive_sender(lclient_t *client, const char *argument, packet_t *header)
{
	(void)client;
	(void)argument;
	(void)header;
	printf("%s: you can't send a request of receive\n", __FUNCTION__);
}

void receive_receiver(lclient_t *client)
{
	char *name;
	char *msg;

	receive_str(client, &name);
	receive_str(client, &msg);
	printf("%s: <%s> \"%s\"\n", __FUNCTION__, name, msg);
	free(name);
	free(msg);
}

void ready_sender(lclient_t *client, const char *argument, packet_t *header)
{
	(void)argument;
	send_header(client, header);
}

void ready_receiver(lclient_t *client)
{
	uint8_t answer;

	lbuffer_read(&client->buffer, &answer, sizeof(answer));
	printf("%s: %s\n", __FUNCTION__, (answer == 1) ? "READY" : "NOT READY");
}

void require_sender(lclient_t *client, const char *argument, packet_t *packet)
{
	uint8_t answer = 1;

	(void)argument;
	packet->len = 1;
	send_header(client, packet);
	write(client->socket.fd, &answer, sizeof(answer));
}

void require_receiver(lclient_t *client)
{
	uint64_t size;
	char *name;
	uint32_t chksum;

	lbuffer_read(&client->buffer, &size, sizeof(size));
	printf("%s: [%lu]\n", __FUNCTION__, size);
	while (size > 0) {
		receive_str(client, &name);
		lbuffer_read(&client->buffer, &chksum, sizeof(chksum));
		printf("{ %s | %u }\n", name, chksum);
		free(name);
		--size;
	}
}

void loader_sender(lclient_t *client, const char *argument, packet_t *header)
{
	header->len = 16 + strlen(argument);
	uint64_t size = 1;

	send_header(client, header);
	write(client->socket.fd, &size, sizeof(size));
	send_str(client, argument);
}

void loader_receiver(lclient_t *client)
{
	uint64_t nb;
	uint64_t fsize;
	uint32_t chksum;
	char *filename;
	int fd;

	lbuffer_read(&client->buffer, &nb, sizeof(nb));
	printf("%s: [%lu]\n", __FUNCTION__, nb);
	while (nb > 0) {
		lbuffer_read(&client->buffer, &fsize, sizeof(fsize));
		receive_str(client, &filename);
		lbuffer_read(&client->buffer, &chksum, sizeof(chksum));
		fd = open(filename, O_WRONLY | O_CREAT, 0666);
		if (fd == -1)
			return;
		lbuffer_fdread(&client->buffer, fd, fsize);
		printf("{%lu bytes, \"%s\", %u sum}\n", fsize, filename,
		       chksum);
		free(filename);
		close(fd);
		--nb;
	}
}

void start_receiver(lclient_t *client)
{
	uint16_t port;

	lbuffer_read(&client->buffer, &port, sizeof(port));
	printf("%s: %u\n", __FUNCTION__, port);
}

void help(lclient_t *client, const char *argument, packet_t *header);

static const jumper_t actions[] = {
	{LOGIN, "login", &login_sender, &login_receiver},
	{LOGOUT, "logout", &logout_sender, &logout_receiver},
	{CREATE_GAMESESSION, "create", &create_sender, &create_receiver},
	{DELETE_GAMESESSION, "delete", &delete_sender, &delete_receiver},
	{GET_GAMESESSIONS_LIST, "get-games", &getgames_sender,
	 &getgames_receiver},
	{JOIN_GAMESESSION, "join", &join_sender, &join_receiver},
	{LEAVE_GAMESESSION, "leave", &leave_sender, &leave_receiver},
	{GET_GAMESESSION_PLAYERS_LIST, "get-players", &getplayers_sender,
	 &getplayers_receiver},
	{SEND_MESSAGE, "send", &send_sender, &send_receiver},
	{RECEIVE_MESSAGE, "receive", &receive_sender, &receive_receiver},
	{TOGGLE_READY, "ready", &ready_sender, &ready_receiver},
	{ASSETS_REQUIREMENT, "require", &require_sender, &require_receiver},
	{ASSETS_LOADER, "loader", &loader_sender, &loader_receiver},
	{GAME_STARTED, "start", NULL, &start_receiver},
	{ACK, "ack", NULL, NULL},
	{0, "help", help, NULL},
};

void help(lclient_t *client, const char *argument, packet_t *header)
{
	(void)client;
	(void)argument;
	(void)header;
	printf("Help:\n");
	for (size_t i = 0; i < sizeof(actions) / sizeof(*actions); ++i) {
		printf("\t%s\n", actions[i].command);
	}
}

void *receiver_handler(void *data)
{
	lclient_t *client = data;
	packet_t header;

	while (1) {
		lclient_update(client, 100);
		if (lbuffer_size(&client->buffer) >= sizeof(packet_t)) {
			lbuffer_read(&client->buffer, &header, sizeof(header));
			packet_display(&header);
			while (lbuffer_size(&client->buffer) < header.len)
				lclient_update(client, 100);
			rl_clear_visible_line();
			printf("[] ");
			actions[header.type].receiver(client);
			rl_forced_update_display();
		}
	}
}

int main(void)
{
	lclient_t client;
	packet_t header;
	char *line;
	pthread_t thread;

	if (lclient_create32(&client, 409600, 0, 2121) == -1) {
		printf("Can't connect to server\n");
		return (-1);
	}
	if (pthread_create(&thread, NULL, receiver_handler, &client) == -1)
		return -1;
	pthread_detach(thread);
	do {
		line = readline("> ");
		if (line != NULL) {
			for (size_t i = 0;
			     i < sizeof(actions) / sizeof(*actions); ++i) {
				if (strncmp(actions[i].command, line,
					    strlen(actions[i].command))
				    == 0) {
					packet_create(&header, i, 0);
					actions[i].sender(
						&client,
						line + strlen(actions[i].command)
							+ 1,
						&header);
					break;
				}
				if (i + 1 == sizeof(actions) / sizeof(*actions))
					printf("Command not found: %s\n", line);
				else
					add_history(line);
			}
			free(line);
		}
	} while (line != NULL);
	lclient_destroy(&client);
	return (0);
}
/* 1 + (0)*printf calls alloc by default */
