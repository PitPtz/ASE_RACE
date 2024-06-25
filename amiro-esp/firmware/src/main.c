#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include <sys/param.h>
#include "nvs_flash.h"
#include "driver/twai.h"
#include <esp_heap_caps.h>
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define LED_PIN                 32
#define TX_GPIO_NUM             4 //Prototyp: 19
#define RX_GPIO_NUM             2 //Prototyp: 21
#define KEEPALIVE_IDLE              5
#define KEEPALIVE_INTERVAL          10
#define KEEPALIVE_COUNT 10
#define CAN_SEND_MESSAGE_TIMEOUT_IN_MS 500

/*
Open Todos:
 - Cleanup
 - Replace LED functions with ledc hardware pwm
 - Define LED states
 - Use the larger memory on the new esp board
 - Measure energy consumption
*/

static uint8_t s_led_state = 0;
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);

#define PORT 1234
int sock=0;
#define TAG "main"
#define CAN_RX_QUEUE_LEN 8192*8
QueueHandle_t can_rx_queue;
void *can_rx_queue_storage;
void *can_rx_queue_buf;
int num_clients;


static void toggle_led() {
    s_led_state = !s_led_state;
    gpio_set_level(LED_PIN, s_led_state);

}

static void set_led(bool state) {
    s_led_state = state?1:0;
    gpio_set_level(LED_PIN, s_led_state);

}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        set_led(false);
        ESP_LOGI(TAG,"WIFI_EVENT_STA_START");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
        uint8_t reason = event->reason;
        printf("Reason: %u\n", reason);
        esp_wifi_connect();
        set_led(false);
        ESP_LOGI(TAG,"WIFI_EVENT_STA_DISCONNECTED->recon");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        set_led(true);
    } else if (event_base == WIFI_EVENT){
        ESP_LOGI(TAG, "other wifi event: %ld", event_id);
    } else {
        ESP_LOGI(TAG, "other event: %ld", event_id);
    }
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t* netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_netif_set_hostname(netif, DEVICE_NAME));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .ssid = WIFI_SSID,
            .rm_enabled =1,
            .btm_enabled =1,
            .pmf_cfg.capable = 1,
            .mbo_enabled = 1
        },
    };
    
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)WIFI_EAP_USER, strlen(WIFI_EAP_USER)) );

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)WIFI_EAP_USER, strlen(WIFI_EAP_USER)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)WIFI_EAP_PW, strlen(WIFI_EAP_PW)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_MSCHAPV2)); //ESP_EAP_TTLS_PHASE2_PAP
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
    //ESP_ERROR_CHECK( tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA ,DEVICE_NAME));
}

void server_init()
{
     int addr_family = AF_INET;
    struct sockaddr_storage dest_addr;
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORT);
    int ip_protocol = IPPROTO_IP;

    
    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ESP_LOGI(TAG, "Socket created");
    if (setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt))) 
    {
         ESP_LOGE(TAG,"ERROR: setsocketopt(), TCP_NODELAY"); 
    }; 
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    int no_delay = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(int));
    
 
    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }
    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    ESP_LOGI(TAG, "Socket listening");
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t addr_len = sizeof(source_addr);
    while (1) 
    {
        vTaskDelay(10);
        int sock2 = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock2 < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            //break;
            continue;
        }

        char addr_str[128];
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);
        
        if (sock>1)
        {
            shutdown(sock, 2); // shutdown send & recv
            vTaskDelay(10); // delay needed
            close(sock);
        }

        num_clients+=1;
        unsigned char first_msg[20];
        for (int i=0;i<20;i++)
            first_msg[i]='#';
        first_msg[19] = num_clients;
        send(sock2, &first_msg[0], 20, 0);
        sock = sock2;
        ESP_LOGI(TAG, "new sock is global");
    }
    CLEAN_UP:
        close(listen_sock);
        vTaskDelete(NULL);
}

void receive_can_over_wifi()
{
    twai_message_t msg;
    while (true)
    {
         if (sock==0)
        {
            vTaskDelay(10);
            continue;
        }
        int len = recv(sock, &msg, sizeof(twai_message_t), 0);
        ESP_LOGE(TAG, "Got CAN PKG from WLAN: length %d", len);
        if (len < 0) {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        } else if (len == 0) {
            ESP_LOGW(TAG, "Connection closed");
        } else {
            if (len!=sizeof(twai_message_t))
            {
                ESP_LOGE(TAG, "msg over wifi: wrong size, should not happen");
                continue;
            }
            twai_transmit(&msg, pdMS_TO_TICKS(CAN_SEND_MESSAGE_TIMEOUT_IN_MS));
        }
    }
}

void transmit_can_over_wifi()
{
    const int max_msgs = 72; //1452 Bytes MSS => 1452/20=72
    twai_message_t *rx_message = (twai_message_t*)malloc(sizeof(twai_message_t)*max_msgs); 
    //ESP_LOGI(TAG, "after rx_msg (%d)",xPortGetMinimumEverFreeHeapSize());
    int missed=0;
    TickType_t last = 0;
    TickType_t send_pre=0;
    TickType_t send_post=0;
    while (true)
    {
        TickType_t start = xTaskGetTickCount();
        twai_status_info_t status;
        for (int t=0;t<20;t++) 
        {  
            int msgs_to_rx = uxQueueMessagesWaiting(can_rx_queue);
            if (msgs_to_rx==0)
            {
                continue;
            }
            /*if (msgs_to_rx<16) 
            {
                vTaskDelay(10);
                msgs_to_rx = uxQueueMessagesWaiting(can_rx_queue);
            }*/
            if (msgs_to_rx>max_msgs)
            {
                msgs_to_rx = max_msgs;
            }
            for (int i=0;i<msgs_to_rx;i++)
            {
                if (pdTRUE != xQueueReceive(can_rx_queue,&rx_message[i], 0))
                {
                    ESP_LOGI(TAG, "should not happen: can_rx_queue empty");
                }
            }
            if (sock==0)
            {
                continue;
            }
            int len = sizeof(twai_message_t)*msgs_to_rx;
            int to_write = len;
            while (to_write > 0) {
                send_pre = xTaskGetTickCount();
                int written = send(sock, rx_message + (len - to_write), to_write, 0);
                send_post = xTaskGetTickCount();
                if (written < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    shutdown(sock, 2); // shutdown send & recv
                    vTaskDelay(10); // delay needed
                    close(sock);
                    sock=0;
                    break;
                }
                if (written!=len)
                {
                    printf(".\n");
                }
                to_write -= written;
            }
        }
        TickType_t send_done = xTaskGetTickCount();
        //taskYIELD();
        vTaskDelay(10);
        TickType_t yield_done = xTaskGetTickCount();
        //vTaskDelay(100 / portTICK_PERIOD_MS);
        //vTaskDelay(10);
        if (ESP_OK!=twai_get_status_info(&status))
        {
                ESP_LOGI(TAG, "Statuserr");
        }
        if (missed!=status.rx_missed_count)
        {
          printf("l:%lu,%lu,%lu\n",status.rx_overrun_count,status.rx_missed_count,xPortGetMinimumEverFreeHeapSize());  
          missed = status.rx_missed_count;
        }
        TickType_t final_done = xTaskGetTickCount();
        if (final_done-start>20)
        {
            //printf("%3d %3d %3d %3d %3d",send_done-start, yield_done-start,final_done-start,start-last, send_post-send_pre);
            //printf(" | %5d %2d %2d", status.rx_missed_count, status.msgs_to_rx, uxQueueMessagesWaiting(can_rx_queue));
            //printf(" | %d %d\n", xPortGetMinimumEverFreeHeapSize(), heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
            last=start;
        }
        
        //printf("l:%d,%d,%d\n",status.rx_overrun_count,status.rx_missed_count,xPortGetMinimumEverFreeHeapSize());
    }
}



void handle_rx() 
{
    /* TODO check malloc returns! */
    ESP_LOGI(TAG, "CAN RX BUF SIZE: %d avail %d",sizeof(twai_message_t)*CAN_RX_QUEUE_LEN,heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    can_rx_queue_storage = heap_caps_malloc(sizeof(twai_message_t)*CAN_RX_QUEUE_LEN,MALLOC_CAP_SPIRAM);
    can_rx_queue_buf = malloc(sizeof(StaticQueue_t));
    can_rx_queue = xQueueCreateStatic(CAN_RX_QUEUE_LEN,sizeof(twai_message_t),(uint8_t*)can_rx_queue_storage,can_rx_queue_buf);
    g_config.rx_queue_len=1024;
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(TAG, "Driver installed");
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAG, "Driver started: rx queue: %lu (%lu) sleep time %lu",g_config.rx_queue_len, xPortGetMinimumEverFreeHeapSize(),100 / portTICK_PERIOD_MS);
    xTaskCreate(transmit_can_over_wifi, "transmit_can_over_wifi", 4096, NULL, tskIDLE_PRIORITY+1, NULL);
    twai_message_t tmp;
    while (true)
    {
        if (ESP_OK == twai_receive(&tmp, pdMS_TO_TICKS(1000)))
        {
            xQueueSendToBack(can_rx_queue,&tmp,pdMS_TO_TICKS(1000));
        }
    }
}



void app_main() 
{
    printf("mem free:%d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));
    num_clients=0;
    gpio_reset_pin(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init();
    esp_wifi_set_ps (WIFI_PS_NONE);
    
    xTaskCreate(server_init, "tcp_server", 4096, NULL, 5, NULL);
    
    xTaskCreate(handle_rx, "can_rx_handler", 4096, NULL, tskIDLE_PRIORITY+2, NULL);
    xTaskCreate(receive_can_over_wifi, "can_tx_handler", 4096, NULL, tskIDLE_PRIORITY+2, NULL);

    
    printf("%d\n",heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    printf("%d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));
    
    printf("%d\n",heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    printf("%d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));
    
    
    printf("mem internal free:%d\n",heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    printf("mem free: %d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));
    printf("heap free: %lu\n",esp_get_free_heap_size());
    printf("heap free min: %lu\n",esp_get_minimum_free_heap_size());
    printf("mem psram free: %d\n",heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    
    
   // uint32_t alerts_to_enable = TWAI_ALERT_ALL ; //TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_OFF;
   // ESP_ERROR_CHECK(twai_reconfigure_alerts(alerts_to_enable, NULL));

}