#include "communication.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"

static err_t tcp_client_connected(void *arg, struct altcp_pcb *pcb, err_t err)
{
    if (err != ERR_OK) 
    {
        printf("[-] Connection failed: %d . . .\n", err);
        return err;
    }

    printf("[+] Connected to ThingSpeak . . .\n");
    return ERR_OK;
}

static err_t tcp_client_sent(void *arg, struct altcp_pcb *pcb, u16_t len) 
{
    printf("[+] Data sent: %u bytes . . .\n", len);
    return ERR_OK;
}

static err_t tcp_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) 
{
    http_request_t *req = (http_request_t *)arg;
    
    if (!p) 
    {
        printf("[/] Connection closed . . .\n");
        req->complete = true;
        altcp_close(pcb);
        return ERR_OK;
    }

    if (p->tot_len > 0) 
    {
        printf("[+] Received %d bytes: ", p->tot_len);
        for (struct pbuf *q = p; q != NULL; q = q->next) 
	{
            printf("%.*s", q->len, (char *)q->payload);
        }
        printf("\n");
    }

    altcp_recved(pcb, p->tot_len);
    pbuf_free(p);
    
    return ERR_OK;
}

bool send_to_thingspeak(int reading) 
{
    printf("Sending reading to ThingSpeak: %d . . .\n", reading);
    
    http_request_t request = {0};
    
    struct altcp_pcb *pcb = altcp_tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) 
    {
        printf("[-] Failed to create PCB . . .\n");
        return false;
    }
    
    request.pcb = pcb;
    altcp_arg(pcb, &request);
    altcp_recv(pcb, tcp_client_recv);
    altcp_sent(pcb, tcp_client_sent);
    
    ip_addr_t server_ip;
    err_t err = dns_gethostbyname("api.thingspeak.com", &server_ip, NULL, NULL);
    
    int timeout = 100;
    if (err == ERR_INPROGRESS) 
    {
        printf("[/] DNS lookup in progress . . .\n");

        while (err == ERR_INPROGRESS && timeout-- > 0) 
	{
            sleep_ms(100);
            err = dns_gethostbyname("api.thingspeak.com", &server_ip, NULL, NULL);
        }
    }
    
    if (err != ERR_OK) 
    {
        printf("[-] DNS lookup failed . . .\n");
        altcp_close(pcb);
        return false;
    }
    
    printf("[+] Connecting to ThingSpeak . . .\n");
    
    err = altcp_connect(pcb, &server_ip, 80, tcp_client_connected);
    if (err != ERR_OK) 
    {
        printf("[-] Connection failed: %d . . .\n", err);
        altcp_close(pcb);
        return false;
    }
    
    char request_buffer[256];
    snprintf(request_buffer, sizeof(request_buffer),
             "GET /update?api_key=%s&field1=%.2f HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             THINGSPEAK_API_KEY, reading, "api.thingspeak.com");
    
    err = altcp_write(pcb, request_buffer, strlen(request_buffer), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) 
    {
        printf("[-] Write failed: %d . . .\n", err);
        altcp_close(pcb);
        return false;
    }
    
    altcp_output(pcb);
    
    timeout = 100;
    while (!request.complete && timeout-- > 0) 
    {
        sleep_ms(100);
    }
    
    if (request.complete) 
    {
        printf("[+] Request completed successfully . . .\n");
        return true;
    } 
    else
    {
        printf("[/] Request timed out . . .\n");
        altcp_close(pcb);
        return false;
    }
}
