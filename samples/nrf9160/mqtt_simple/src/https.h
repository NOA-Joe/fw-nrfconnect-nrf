
#include <string.h>
#include <zephyr.h>
#include <stdlib.h>
#include <net/socket.h>
#include <modem/bsdlib.h>
#include <net/tls_credentials.h>
#include <modem/lte_lc.h>
#include <modem/at_cmd.h>
#include <modem/at_notif.h>
#include <modem/modem_key_mgmt.h>


/* Initialize AT communications */
int at_comms_init(void);

/* Provision certificate to modem */
int cert_provision(void);

/* Setup TLS options on a given socket */
int tls_setup(int fd);


void https_main(void);