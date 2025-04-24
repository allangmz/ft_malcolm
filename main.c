#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit, malloc, free
#include <string.h>         // memset, memcpy, strcmp
#include <unistd.h>         // close, getuid, sleep
#include <sys/socket.h>     // socket, sendto, recvfrom, AF_*, SOCK_*
#include <netinet/in.h>     // htons, ntohs, IPPROTO_*, struct sockaddr_in
#include <net/ethernet.h>   // ETH_P_ARP, ETH_ALEN
#include <netpacket/packet.h> // struct sockaddr_ll, AF_PACKET
#include <net/if.h>         // if_nametoindex, struct ifreq
#include <arpa/inet.h>      // inet_pton, inet_ntop, inet_addr
#include <netdb.h>          // gethostbyname, getaddrinfo, freeaddrinfo, gai_strerror
#include <ifaddrs.h>        // getifaddrs, freeifaddrs
#include <signal.h>         // signal, sigaction
#include <net/if.h>
#include <sys/types.h>

int ft_strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int verify_ip_format(const char *ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if (result == 0) {
        write(STDERR_FILENO, "Invalid IP address format\n", 26);
        return 0;
    } else if (result == -1) {
        write(STDERR_FILENO, "Error in inet_pton\n", 20);
        return 0;
    }
    return 1;
}

int verify_mac_format(const char *mac) {
    int i;

    i = 0;
    if (mac == NULL) {
        write(STDERR_FILENO, "Invalid MAC address format\n", 28);
        return 0;
    }
    if (ft_strlen(mac) != 17) {
        write(STDERR_FILENO, "Invalid MAC address format\n", 28);
        return 0;
    }

    while (mac[i]) {
        if (i % 3 == 2) {
            if (mac[i] != ':') {
                write(STDERR_FILENO, "Invalid MAC address format\n", 28);
                return 0;
            }
        } else {
            if (!((mac[i] >= '0' && mac[i] <= '9') || (mac[i] >= 'a' && mac[i] <= 'f') || (mac[i] >= 'A' && mac[i] <= 'F'))) {
                write(STDERR_FILENO, "Invalid MAC address format\n", 28);
                return 0;
            }
        }
        i++;
    }
    return 1;
}

char *get_interface()
{
    struct ifaddrs *ifaddr, *current_ifaddr;
    char ip[INET_ADDRSTRLEN];
    char *interface_name = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    current_ifaddr = ifaddr;
    while (current_ifaddr != NULL) {
        int flags = current_ifaddr->ifa_flags;
        // Affichez les informations de l'adresse si disponibles
        if (current_ifaddr->ifa_addr != NULL)
        {
            if ((flags & IFF_UP) && (flags & IFF_RUNNING) && !(flags & IFF_LOOPBACK)) {
                if (current_ifaddr->ifa_addr->sa_family == AF_INET) {
                    inet_ntop(AF_INET, &(((struct sockaddr_in *)current_ifaddr->ifa_addr)->sin_addr), ip, INET_ADDRSTRLEN);
                    if (strcmp(ip, "0.0.0.0") == 0 || strcmp(ip, "2.0.0.0") == 0) {
                        current_ifaddr = current_ifaddr->ifa_next;
                        continue;
                    }
                    interface_name = current_ifaddr->ifa_name;
                    freeifaddrs(ifaddr);
                    printf("IP trouvé: %s\n", strduo(ip));
                    return interface_name;
                }   
            }
        }
        current_ifaddr = current_ifaddr->ifa_next;
    }

    freeifaddrs(ifaddr);
    return NULL;
}



int ft_malcolm(char sourceIp, char sourceMacAddr, char targetIp, char targetMacAddr) {

    // if (verify_ip_format(sourceIp) != 1 && verify_mac_format(sourceMacAddr) != 1) {
    //     return;
    // }
    // if (verify_ip_format(targetIp) != 1 && verify_mac_format(targetMacAddr) != 1) {
    //     return;
    // }
    char *interface_name = get_interface();
    if (interface_name == NULL) {
        write(STDERR_FILENO, "No valid interface found\n", 25);
        return 0;
    }
    int ifindex = if_nametoindex(interface_name);
    if (ifindex == 0) {
        perror("if_nametoindex");
        free(interface_name);
        return 0;
    }
    
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
        printf("IP: %s\n", interface_name);
    return 0;
}


int main() {
    ft_malcolm(0, 0, 0, 0);
    return 0;
}