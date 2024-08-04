#ifdef TARGET_LINUX

#include <iostream>

int main() {
	std::cout << "Hello, world" << std::endl;
}

#else

#include "nvs_flash.h"

extern void app_wifi(void);
extern void app_websocket(void);

extern "C" {
extern void app_main(void);
}

void app_main(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  app_wifi();
  app_websocket();
}
#endif
