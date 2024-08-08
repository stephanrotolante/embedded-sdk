#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8765
#define BUFFER_SIZE 409

static const char b64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int mod_table[] = {0, 2, 1};

// Function to encode a byte array to a base64 string
char *base64Encode(const unsigned char *data, size_t input_length, size_t *output_length)
{
  *output_length = 4 * ((input_length + 2) / 3);
  char *encoded_data = (char *)malloc(*output_length + 1);
  if (encoded_data == NULL)
    return NULL;

  for (size_t i = 0, j = 0; i < input_length;)
  {
    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded_data[j++] = b64_chars[(triple >> 3 * 6) & 0x3F];
    encoded_data[j++] = b64_chars[(triple >> 2 * 6) & 0x3F];
    encoded_data[j++] = b64_chars[(triple >> 1 * 6) & 0x3F];
    encoded_data[j++] = b64_chars[(triple >> 0 * 6) & 0x3F];
  }

  for (size_t i = 0; i < mod_table[input_length % 3]; ++i)
    encoded_data[*output_length - 1 - i] = '=';

  encoded_data[*output_length] = '\0';
  return encoded_data;
}

// Function to generate a random 16-byte key and encode it as a base64 string
char *generateChallengeKey()
{
  unsigned char bytes[16];
  size_t output_length;

  // Seed the random number generator
  srand((unsigned int)time(NULL));

  // Generate 16 random bytes
  for (int i = 0; i < 16; ++i)
  {
    bytes[i] = rand() % 256;
  }

  // Encode the random bytes to a base64 string
  return base64Encode(bytes, 16, &output_length);
}
// #ifdef TARGET_LINUX

// #include <stdlib.h>
// #include <time.h>

// int main()
// {

//   char *key = generateChallengeKey();
//   if (key)
//   {
//     printf("Generated key: %s\n", key);
//     // free(key); // Remember to free the allocated memory
//   }
//   else
//   {
//     fprintf(stderr, "Error generating key.\n");
//   }

//   const char *hostname = "127.0.0.1";
//   const char *path = "/";

//   int sockfd;
//   struct sockaddr_in server_addr;
//   struct hostent *server;
//   char request[BUFFER_SIZE];
//   char response[BUFFER_SIZE];

//   // Create socket
//   sockfd = socket(AF_INET, SOCK_STREAM, 0);
//   if (sockfd < 0)
//   {
//     perror("Error opening socket");
//     exit(EXIT_FAILURE);
//   }

//   // Get server IP address
//   server = gethostbyname(hostname);
//   if (server == NULL)
//   {
//     fprintf(stderr, "No such host\n");
//     exit(EXIT_FAILURE);
//   }

//   // Set up server address struct
//   memset(&server_addr, 0, sizeof(server_addr));
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_port = htons(PORT);
//   memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

//   // Connect to the server
//   if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//   {
//     perror("Error connecting");
//     exit(EXIT_FAILURE);
//   }

//   // Create HTTP GET request
//   snprintf(request, sizeof(request),
//            "GET %s HTTP/1.1\r\n"
//            "Host: %s\r\n"
//            "Upgrade: websocket\r\n"
//            "Connection: Upgrade\r\n"
//            "Sec-WebSocket-Key: %s\r\n"
//            "Sec-WebSocket-Version: 13\r\n"
//            "Connection: close\r\n\r\n",
//            path, hostname, key);

//   printf(request, sizeof(request),
//          "GET %s HTTP/1.1\r\n"
//          "Host: %s\r\n"
//          "Upgrade: websocket\r\n"
//          "Connection: Upgrade\r\n"
//          "Sec-WebSocket-Key: %s\r\n"
//          "Sec-WebSocket-Version: 13\r\n"
//          "Connection: close\r\n\r\n",
//          path, hostname, key);

//   // Send the request
//   if (write(sockfd, request, strlen(request)) < 0)
//   {
//     perror("Error writing to socket");
//     exit(EXIT_FAILURE);
//   }

//   // Read the response
//   int n;
//   while ((n = read(sockfd, response, sizeof(response) - 1)) > 0)
//   {
//     response[n] = '\0';
//     printf("%s\n", response);
//   }

//   if (n < 0)
//   {
//     perror("Error reading from socket");
//   }

//   // Close the socket
//   close(sockfd);
// }

// #else
#include "sdkconfig.h"
#include <sys/socket.h>
#include <errno.h>
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "embedded-sdk";

extern void app_wifi(void);
extern void app_websocket(void);

extern "C"
{
  extern void app_main(void);
}

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  app_wifi();




  vTaskDelay(pdMS_TO_TICKS(10000));

  char *key = generateChallengeKey();
  if (key)
  {
    ESP_LOGI(TAG, "Generated key: %s\n", key);
    // free(key); // Remember to free the allocated memory
  }
  else
  {
    ESP_LOGI(TAG, "Error generating key.\n");
  }
  // app_websocket();

  char rx_buffer[128];
  const char *host_ip = "192.168.1.65";
  const char *path = "/";
  int addr_family = 0;
  int ip_protocol = 0;
  char request[BUFFER_SIZE];

  struct sockaddr_in dest_addr;
  inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(PORT);
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;

  int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
  if (sock < 0)
  {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return;
  }
  ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

  int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
    return;
  }
  ESP_LOGI(TAG, "Successfully connected");

  snprintf(request, sizeof(request),
           "GET %s HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Key: %s\r\n"
           "Sec-WebSocket-Version: 13\r\n"
           "Connection: close\r\n\r\n",
           path, host_ip, key);

  err = send(sock, request, strlen(request), 0);
  if (err < 0)
  {
    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    return;
  }

  int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
  // Error occurred during receiving
  if (len < 0)
  {
    ESP_LOGE(TAG, "recv failed: errno %d", errno);
    return;
  }
  // Data received
  else
  {
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
    ESP_LOGI(TAG, "%s", rx_buffer);
  }

  if (sock != -1)
  {
    ESP_LOGE(TAG, "Shutting down socket and restarting...");
    shutdown(sock, 0);
    close(sock);
  }
}
// #endif
