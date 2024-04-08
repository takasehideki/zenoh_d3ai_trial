#include <stdio.h>
#include <string.h>
#include "zenoh.h"

void data_handler(const z_sample_t *sample, void *arg)
{
  z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);

  printf("[sub.c ] %.*s\n",
         (int)sample->payload.len,
         sample->payload.start);
  
  z_drop(z_move(keystr));
}

int main(int argc, char **argv)
{
  char *expr = "key/expression";

  z_owned_config_t config = z_config_default();
  z_owned_session_t s = z_open(z_move(config));

  z_owned_closure_sample_t callback = z_closure(data_handler);
  z_owned_subscriber_t sub = z_declare_subscriber(z_loan(s), z_keyexpr(expr), z_move(callback), NULL);

  while (1)
  {
  }

  z_undeclare_subscriber(z_move(sub));
  z_close(z_move(s));
  return 0;
}