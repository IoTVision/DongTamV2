#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "OnlineStatusEvent.h"
esp_ping_handle_t ping;

#define TIME_TO_PING 3

static void TestOnPingSuccess(esp_ping_handle_t hdl, void *args)
{

    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    printf("Successfully get %lu bytes from %s icmp_seq=%d ttl=%d time=%ld ms\n",recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
    if(seqno >= TIME_TO_PING){
        ESP_LOGI("PingSuccess","Stop ping");
        esp_ping_stop(ping);
        OnlEvt_SetBit(ONL_EVT_PING_SUCCESS);
        OnlEvt_ClearBit(ONL_EVT_PING_TIMEOUT);
    }
}

static void TestOnPingTimeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("Ping timeout from %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);\
    if(seqno >= TIME_TO_PING){
        ESP_LOGE("PingTimeout","Stop ping");
        esp_ping_stop(ping);
        OnlEvt_SetBit(ONL_EVT_PING_TIMEOUT);
        OnlEvt_ClearBit(ONL_EVT_PING_SUCCESS);
    }
}

static void TestOnPingEnd(esp_ping_handle_t hdl, void *args)
{
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    printf("End with %lu packets transmitted, %ld received, time %ldms\n", transmitted, received, total_time_ms);
}

void DNS_Ping(char *host)
{
    ESP_LOGI("Ping","to %s........",host);
    ip_addr_t targetAddress;
    memset(&targetAddress,0,sizeof(targetAddress));
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint,0,sizeof(hint));
    getaddrinfo(host, NULL, &hint, &res);
    /* 
    Sau khi gọi hàm getaddrinfo từ host thì thông tin được lưu vào trong res thuộc kiểu addrinfo
    Trong addrinfo có một member struct sockaddr với tên con trỏ tên là ai_addr(address info_ address)
    ép kiểu con trỏ ai_addr đó thành kiểu sockaddr_in* (nghĩa là ép kiểu từ sockaddr thành sockaddr_in) 
    và trỏ tới member in_addr có tên sin_addr trong sockaddr_in và gán vào addr4
    */
    struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
    ESP_LOGI("DNS_Check","ai_addr:%p,sa_data:%s,addr4:%lu",&res->ai_addr,res->ai_addr->sa_data,addr4.s_addr);
    inet_addr_to_ip4addr(ip_2_ip4(&targetAddress),&addr4);
    freeaddrinfo(res);


    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ping_config.target_addr = targetAddress;          // target IP address
    ping_config.count = ESP_PING_COUNT_INFINITE;    // ping in infinite mode, esp_ping_stop can stop it

    /* set callback functions */
    esp_ping_callbacks_t cbs;
    cbs.on_ping_success = TestOnPingSuccess;
    cbs.on_ping_timeout = TestOnPingTimeout;
    cbs.on_ping_end = TestOnPingEnd;

    
    esp_ping_new_session(&ping_config, &cbs, &ping);
    esp_ping_start(ping);
}