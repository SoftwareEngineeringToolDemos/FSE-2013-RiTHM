/*
 ============================================================================
 Name        : QIDS.c
 Author      : Shay Berkovich
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <unistd.h>
#include "sniffex.h"
#include <GooMFAnalyzer.h>
#include <time.h>

//global declarations
struct timespec ts1, ts2;
_GOOMF_context context = NULL;
_GOOMF_verdict_type v;
bool _GLOBAL_monitoring = false;
int _GLOBAL_iterations = 0;
void* _GLOBAL_ps;


/*
 * Measures difference between 2 timestamps
 */
struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}


void printResults()
{
	clock_gettime(CLOCK_REALTIME, &ts2);
	struct timespec ts3 = diff(ts1, ts2);

	if (_GLOBAL_monitoring)
	{
		char str[100];
		_GOOMF_get_current_status(context, &v);
		_GOOMF_type_to_string(&v, str);
		printf("%ld sec, %ld microsec, %ld nanosec, property is %s\n",
				ts3.tv_sec,
				ts3.tv_nsec / 1000,
				ts3.tv_nsec,
				str);
	}
	else
		printf("%ld sec, %ld microsec, %ld nanosec\n", ts3.tv_sec, ts3.tv_nsec / 1000,	ts3.tv_nsec);
}


void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	const struct sniff_ethernet* ethernet;  //ethernet header
	const struct sniff_ip *ip;              //IP header
	const struct sniff_tcp *tcp;            //TCP header
	const char *payload;                    //packet payload
	int size_ip = 0;
	int size_tcp = 0;
	int size_payload = 0;

	ethernet = (struct sniff_ethernet*)(packet);

	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;

	//we are interested only in TCP packets
	if ((size_ip < 20) || (ip->ip_p != IPPROTO_TCP))
		return;

	//handling tcp header
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20)
		return;

	payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
	size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

	/*
	if (_GLOBAL_monitoring)
	{
		_GLOBAL_ps = _GOOMF_get_next_space(context);
		_GOOMF_fill_program_state(_GLOBAL_ps, left, right, tempLeft, tempRight);
		_GOOMF_next(context);
		if (_GOOMF_all_properties_converged(context) == true)
			break;
	}
	*/

	fprintf(stdout, "Received packet %s;\n", packet);
	return;
}


int main(int argc, char *argv[])
{
	int c;
	pcap_t* session_handler = NULL;
	char* device = NULL;			//device to sniff on
	char* port = NULL;				//port to sniff on (optional)
	char errbuf[PCAP_ERRBUF_SIZE];	//error string
	bpf_u_int32 mask;				//netmask
	bpf_u_int32 net;				//IP


	//parsing the arguments
	while ((c = getopt(argc, argv, "d:p:m")) != -1)
	{
		switch (c) {
		case 'd':
			device = optarg;
			break;
		case 'p':
			port = optarg;
			break;
		case 'm':
			_GLOBAL_monitoring = true;
			break;
		default:
			fprintf(stderr, "Wrong arguments.\n");
			return -1;
		}
	}

	if (device == NULL)
	{
		device = pcap_lookupdev(errbuf);
		if (device == NULL)
		{
			fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
			return -1;
		}
	}

	//find the properties for the device
	if (pcap_lookupnet(device, &net, &mask, errbuf) == -1)
	{
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n", device, errbuf);
		net = 0;
		mask = 0;
		return -1;
	}

	//open the session in promiscuous mode
	session_handler = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
	if (session_handler == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", device, errbuf);
		return -1;
	}

	//compile and apply the filter only if the specific port is specified
	if (port != NULL)
	{
		struct bpf_program fp;			//compiled filter
		char filter_exp[100] = "port ";	//filter expression

		strcat(filter_exp, port);
		if (pcap_compile(session_handler, &fp, filter_exp, 0, net) == -1)
		{
			fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(session_handler));
			return -1;
		}
		if (pcap_setfilter(session_handler, &fp) == -1)
		{
			fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(session_handler));
			return -1;
		}
	}

	//actual sniffing
	//packet = pcap_next(handle, &header);
	//print the packet
	//printf("Jacked a packet with length of [%d]: %s\n", header.len, packet);
	//struct pcap_pkthdr header;		//header that pcap gives us
	//const u_char* packet;			//actual packet

	if (_GLOBAL_monitoring)
		_GOOMF_init_context(&context, _GOOMF_enum_buffer_trigger, _GOOMF_enum_CPU, 1, NULL);

	pcap_loop(session_handler, 10, got_packet, NULL);

	if (_GLOBAL_monitoring)
		_GOOMF_destroy_context(&context);

	//closing the session
	pcap_close(session_handler);

	return 0;
}
