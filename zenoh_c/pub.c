#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "zenoh.h"

int main(int argc, char **argv)
{
  char *keyexpr = "key/expression";
  char buf[256];
  int i = 0;

  z_owned_config_t config = z_config_default();
  z_owned_session_t s = z_open(z_move(config));
  z_owned_publisher_t pub = z_declare_publisher(z_loan(s), z_keyexpr(keyexpr), NULL);
  while (1)
  {
    sprintf(buf, "Hello from C!! %d", i);
    printf("[pub.c ] %s\n", buf);
    z_publisher_put_options_t options = z_publisher_put_options_default();
    options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
    z_publisher_put(z_loan(pub), (const uint8_t *)buf, strlen(buf), &options);
    sleep(1);
    i++;
  };

  z_undeclare_publisher(z_move(pub));
  z_close(z_move(s));
  return 0;
}