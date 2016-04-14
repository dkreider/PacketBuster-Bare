#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "windivert.h"

#define MAXBUF 0xFFFF
#define MAXURL 4096

int __cdecl main()
{
    HANDLE handle;
    WINDIVERT_ADDRESS addr;
    UINT8 packet[MAXBUF];
    UINT packet_len;
    PWINDIVERT_IPHDR ip_header;
    PWINDIVERT_TCPHDR tcp_header;
    PVOID payload;
    UINT payload_len;
    PACKET reset0;
    PPACKET reset = &reset0;
    PACKET finish0;
    PPACKET finish = &finish0;
    PDATAPACKET blockpage;
    UINT16 blockpage_len;
    PBLACKLIST blacklist;
    unsigned i;
    INT16 priority = 404;       // Arbitrary.

    // Open the Divert device:
    handle = WinDivertOpen(
		"outbound && "
		"(tcp.DstPort == 80 or tcp.DstPort == 443)",
		0, 0, 0
	);

    if (handle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "error: failed to open the WinDivert device (%d)\n",
            GetLastError());
        exit(EXIT_FAILURE);
    }
    printf("OPENED WinDivert\n");

    // Main loop:
    while (TRUE)
    {
		if (!WinDivertRecv(handle, packet, sizeof(packet), &addr, &packet_len))
		{
			fprintf(stderr, "warning: failed to read packet (%d)\n",
				GetLastError());
			continue;
		}
		else
			printf("Read a packet\n");

            // Packet does not match the blacklist; simply reinject it.
		if (!WinDivertSend(handle, packet, packet_len, &addr, NULL))
		{
			fprintf(stderr, "warning: failed to reinject packet (%d)\n",
				GetLastError());
		}
		else
			printf("Packet re-injected successfully.\n");
        }
    
}

