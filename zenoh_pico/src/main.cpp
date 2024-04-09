/*
 * This code is referred to the follows under EPL-2.0 OR Apache-2.0 license
 * https://github.com/eclipse-zenoh/zenoh-pico/tree/main/examples/arduino
 */

#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <zenoh-pico.h>

// WiFi-specific parameters
#define SSID "SSID" /* need to edit */
#define PASS "PASS" /* need to edit */

#define CLIENT_OR_PEER 0 // 0: Client mode; 1: Peer mode
#if CLIENT_OR_PEER == 0
#define MODE "client"
#define CONNECT "" // If empty, it will scout /* need to edit */
#elif CLIENT_OR_PEER == 1
#define MODE "peer"
#define CONNECT "udp/224.0.0.225:7447#iface=en0"
#else
#error "Unknown Zenoh operation mode. Check CLIENT_OR_PEER value."
#endif

#define KEYEXPR "key/expression"

z_owned_publisher_t pub;
static int idx = 0;
static char display_text[1024];

void data_handler(const z_sample_t *sample, void *arg)
{
  z_owned_str_t keystr = z_keyexpr_to_string(sample->keyexpr);
  std::string val((const char *)sample->payload.start, sample->payload.len);

  Serial.print("[sub.pico] ");
  Serial.println(val.c_str());

  // Print message history on M5 Display
  char display_text_tmp[1024];
  snprintf(display_text_tmp, 1024, "%s", display_text);
  snprintf(display_text, 1024, "%s\n%s", val.c_str(), display_text_tmp);
  M5.Display.clear();
  M5.Display.startWrite();
  M5.Display.setCursor(0, 0);
  M5.Display.print(display_text);
  M5.Display.endWrite();

  z_str_drop(z_str_move(&keystr));
}

void setup()
{
  // Initialize M5 Display
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setTextSize(2);

  // Initialize Serial for debug
  Serial.begin(115200);
  while (!Serial)
  {
    delay(1000);
  }

  // Set WiFi in STA mode and trigger attachment
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  Serial.println("OK");

  // Initialize Zenoh Session and other parameters
  z_owned_config_t config = z_config_default();
  zp_config_insert(z_config_loan(&config), Z_CONFIG_MODE_KEY, z_string_make(MODE));
  if (strcmp(CONNECT, "") != 0)
  {
    zp_config_insert(z_config_loan(&config), Z_CONFIG_CONNECT_KEY, z_string_make(CONNECT));
  }

  // Open Zenoh session
  Serial.print("Opening Zenoh Session...");
  z_owned_session_t s = z_open(z_config_move(&config));
  if (!z_session_check(&s))
  {
    Serial.println("Unable to open session!");
    while (1)
    {
      ;
    }
  }
  Serial.println("OK");

  // Start the receive and the session lease loop for zenoh-pico
  zp_start_read_task(z_session_loan(&s), NULL);
  zp_start_lease_task(z_session_loan(&s), NULL);

  // Declare Zenoh publisher
  Serial.print("Declaring publisher for ");
  Serial.print(KEYEXPR);
  Serial.println("...");
  pub = z_declare_publisher(z_session_loan(&s), z_keyexpr(KEYEXPR), NULL);
  if (!z_publisher_check(&pub))
  {
    Serial.println("Unable to declare publisher for key expression!");
    while (1)
    {
      ;
    }
  }
  Serial.println("OK");

  // Declare Zenoh subscriber
  Serial.print("Declaring Subscriber on ");
  Serial.print(KEYEXPR);
  Serial.println(" ...");
  z_owned_closure_sample_t callback = z_closure_sample(data_handler, NULL, NULL);
  z_owned_subscriber_t sub =
      z_declare_subscriber(z_session_loan(&s), z_keyexpr(KEYEXPR), z_closure_sample_move(&callback), NULL);
  if (!z_subscriber_check(&sub))
  {
    Serial.println("Unable to declare subscriber.");
    while (1)
    {
      ;
    }
  }
  Serial.println("OK");

  Serial.println("Zenoh setup finished!");
  M5.Display.startWrite();
  M5.Display.setCursor(0, 0);
  M5.Display.print("Zenoh setup finished!");
  M5.Display.endWrite();

  delay(1000);
}

void loop()
{
  delay(1000);
  char buf[256];
  sprintf(buf, "Hello from pico!! %d", idx++);
  Serial.print("[pub.pico] ");
  Serial.println(buf);

  z_publisher_put_options_t options = z_publisher_put_options_default();
  options.encoding = z_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN, NULL);
  if (z_publisher_put(z_publisher_loan(&pub), (const uint8_t *)buf, strlen(buf), &options))
  {
    Serial.println("Error while publishing data");
  }
}