
#ifndef PROJETO_SO_2_MANAGER_H
#define PROJETO_SO_2_MANAGER_H

#define MODE_BUFFER_SIZE 15

#define MODE_CREATE 0
#define MODE_REMOVE 1
#define MODE_LIST 2

/* Reads User input and returns the mode */
int read_input(char *register_pipe_name, client_pipe_path_t* client_pipe_name, char *mode, char *box_name);


#endif //PROJETO_SO_2_MANAGER_H
