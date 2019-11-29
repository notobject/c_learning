//
// Created by longduping on 2019/11/28.
//
#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "nids.h"

void on_packet_callback(u_char *user_arg, const struct pcap_pkthdr *pkt_hdr, const u_char *packet)
{
    printf("[%s] ...\n", user_arg);
    nids_pcap_handler(user_arg, pkt_hdr, packet);
}

void tcp_callback(struct tcp_stream *ssn, void **args)
{
    printf("TCP [%s] %d: client=%u, server=%u\n", ssn->user, ssn->nids_state, ssn->client.count_new,
           ssn->server.count_new);
}

void udp_callback(struct tuple4 *addr, char *buf, int len, struct ip *iph)
{
    printf("UDP \n");
}

int main(int argc, char *argv[])
{

    /*** init pcap ***/
    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_if_t *devices = (pcap_if_t *) malloc(sizeof(pcap_if_t));
    if (pcap_findalldevs(&devices, err_buf) < 0) {
        printf("[ERROR] %s\n", err_buf);
        return EXIT_FAILURE;
    }
    char *device = "ens160";
    pcap_if_t *work_if = devices;
    while (work_if) {
        if (!strcmp(device, work_if->name)) {
            break;
        }
        work_if = work_if->next;
    }
    if (work_if == NULL) {
        printf("[ERROR] no available devices!\n");
        return EXIT_FAILURE;
    }
    pcap_t *pcap_fp;
    pcap_fp = pcap_open_live(work_if->name, 1512/*snap len 2^16*/, 1/*promisc mode*/, 0/*always wait*/, err_buf);

    /*** init nids ***/
    struct nids_chksum_ctl ctl = {0};
    ctl.action = NIDS_DONT_CHKSUM;
    nids_params.n_tcp_streams = 16384;
    nids_params.tcp_workarounds = 1;
    nids_params.scan_num_hosts = 0;
    nids_params.pcap_desc = pcap_fp;

    if (!nids_init()) {
        printf("[ERROR] nids init failed!\n");
        return EXIT_FAILURE;
    }
    nids_register_chksum_ctl(&ctl, 0);
    nids_register_tcp(tcp_callback);
    nids_register_udp(udp_callback);

    /*** pcap loop ***/
    char *myname = "ldp";
    printf("working on %s\n", work_if->name);
    pcap_loop(pcap_fp, -1/*cap util error*/, on_packet_callback, (u_char *) myname);

    pcap_close(pcap_fp);
    return 0;
}