#ifndef PROJETO_SO_2_PARSER_H
#define PROJETO_SO_2_PARSER_H

void encode_register_publisher_request();
void encode_register_subscriber_request();
void encode_create_mbox_request();
void encode_answer_create_mbox_request(); // returns 0 if successful, -1
                                          // otherwise
//  Em caso de erro a mensagem de erro é enviada (caso contrário, fica
//  simplesmente inicializada com \0.
void encode_remove_mbox_request();
void encode_answer_remove_mbox_request();
void encode_list_mbox_request();
void encode_answer_list_mbox_request();
void encode_publisher_send_msg();
void encode_server_send_msg_to_subscriber();

void parse_subscriber_receive_msg();

#endif // PROJETO_SO_2_PARSER_H
