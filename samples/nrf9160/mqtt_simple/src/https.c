
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
#include <data/json.h>

#include <drivers/flash.h>
#include <fs/nvs.h>

/*
#include "first_blood.h"

#include "network_interface_def.h"
#include "network_interface_config.h"

#include "TCPSocket.h"
#include "TLSSocket.h"


#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/tlsio_mbedtls.h"
#include "azure_c_shared_utility/xio.h"

#include "parson.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/urlencode.h"

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "internal_flash_rw.h"
#include "user_nrf_fstorage.h"
#include "serial_api.h"

#include "user_system_restart.h"
*/

//////////////////////////////////////////////////////////////////////////////////////
/* Constants that aren't configurable in menuconfig */ 
#define   WEB_SERVER               "sandbox.api.iot.noa.app"
#define   WEB_PORT                  443
#define   FIRST_BLOOD_URL          "/v1/certs/first-blood?has_mobile_app=true&hardware_version=1.0.0"
#define   RENEW_URL                 "https://sandbox.api.iot.noa.app/v1/certs/renew"

/*static const*/ char *FIRST_BLOOD_REQUEST = "POST "FIRST_BLOOD_URL" HTTP/1.1\n"
    "Host: "WEB_SERVER"\n"
    "Content-Type: application/x-www-form-urlencoded\n"
    "Content-Length: 67\n"
    "Connection: close\n\n"
    "token=pl2-onebagtag-luggagetag:5fbc29b5-f070-4014-8931-04518eb72a4f\0";



/*static const*/ char *REQUEST_RENEW = "POST " RENEW_URL " HTTP/1.1\r\n"
    "Host: "WEB_SERVER"\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n\r\n"
    "public_key=%s";


typedef struct Cer_file {
    bool success;
    char* comman_name;
    char* public_key;
    char* private_key; 
    char* secret;
    char* security_key;
    int32_t  factory_reset_id;
};

#define COMMAN_NAME_ID 		1
#define PUBLIIC_KEY_ID 		2
#define PRIVATE_KEY_ID 		3
#define SECRET_ID 			4
#define SECURITY_KEY_ID 	5
#define FACTORY_RESET_ID 	6
//////////////////////////////////////////////////////////////////////////////////////




#define HTTPS_PORT 443

#define HTTP_HEAD                                                              \
	"HEAD / HTTP/1.1\r\n"                                                  \
	"Host: www.baidu.com:443\r\n"                                         \
	"Connection: close\r\n\r\n"

#define HTTP_HEAD_LEN (sizeof(HTTP_HEAD) - 1)

#define HTTP_HDR_END "\r\n\r\n"

#define RECV_BUF_SIZE 2048
#define TLS_SEC_TAG 42

static const char send_buf[] = HTTP_HEAD;
static char recv_buf[RECV_BUF_SIZE];

/* Certificate for `google.com` */
static const char cert[] = {
//DST Root CA X3
"-----BEGIN CERTIFICATE-----\n"
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"
"-----END CERTIFICATE-----\n"
	//#include "../cert/GlobalSign-Root-CA-R2"
};

BUILD_ASSERT_MSG(sizeof(cert) < KB(4), "Certificate too large");


#define FIELD(struct_, member_, type_) { \
	.field_name = #member_, \
	.field_name_len = sizeof(#member_) - 1, \
	.offset = offsetof(struct_, member_), \
	.type = type_ \
        }


char StrongDevInfo(struct Cer_file *Cer_data )
{
  int err;	
  static struct nvs_fs fs;
  ssize_t bytes_written;

  struct flash_pages_info info;

  printk("Flash test \n");
  fs.offset = DT_FLASH_AREA_STORAGE_OFFSET-4096*8;;
  err = flash_get_page_info_by_offs(device_get_binding(DT_FLASH_DEV_NAME),
      fs.offset, &info);
  if (err) {
      printk("Unable to get page info");
  }
  fs.sector_size = info.size;
  fs.sector_count = 8U;
  err = nvs_init(&fs, DT_FLASH_DEV_NAME);
  if (err) {
      printk("Flash Init failed\n");
  }else printk("Flash Init success\n");
/*
#define COMMAN_NAME_ID 		1
#define PUBLIIC_KEY_ID 		2
#define PRIVATE_KEY_ID 		3
#define SECRET_ID 			4
#define SECURITY_KEY_ID 	5
#define FACTORY_RESET_ID 	6
*/
  bytes_written = nvs_write(&fs, COMMAN_NAME_ID, Cer_data->comman_name, strlen(Cer_data->comman_name));	
  bytes_written = nvs_write(&fs, PUBLIIC_KEY_ID, Cer_data->public_key, strlen(Cer_data->public_key));	
  bytes_written = nvs_write(&fs, PRIVATE_KEY_ID, Cer_data->private_key, strlen(Cer_data->private_key));	
  bytes_written = nvs_write(&fs, SECRET_ID , Cer_data->secret, strlen(Cer_data->secret));	
  bytes_written = nvs_write(&fs, SECURITY_KEY_ID, Cer_data->security_key, strlen(Cer_data->security_key));	
  bytes_written = nvs_write(&fs, FACTORY_RESET_ID, Cer_data->factory_reset_id,sizeof(Cer_data->factory_reset_id));	

#if 1  	//Test
  char rbuf[2048];
  for (int i = COMMAN_NAME_ID; i <= FACTORY_RESET_ID; ++i)
  {
  	bytes_written = nvs_read(&fs, i, &rbuf, sizeof(rbuf));
  	printk("Bytes Read to nvs: %d:%s\n", bytes_written,rbuf);
  	memset(rbuf,0,bytes_written);
  }
#endif
  return 0;
}


void parseDataFromJson(const char* json)
{
	static const struct json_obj_descr descr[] = {
		FIELD(struct Cer_file, success, JSON_TOK_STRING),
		FIELD(struct Cer_file, comman_name, JSON_TOK_STRING),
		FIELD(struct Cer_file, public_key, JSON_TOK_STRING),
		FIELD(struct Cer_file, private_key, JSON_TOK_STRING),
		FIELD(struct Cer_file, secret, JSON_TOK_STRING),
		FIELD(struct Cer_file, security_key, JSON_TOK_STRING),
		FIELD(struct Cer_file, factory_reset_id, JSON_TOK_STRING),
	};

	struct Cer_file info = {};
	//char info_str[1024] = {};
	int ret;
	//int size;
    ret = json_obj_parse(json, strlen(json), descr, ARRAY_SIZE(descr), &info);

    if (ret < 0) {
	printk("Could not retrieve: %d\n", ret);
        return;
	}
	StrongDevInfo(&info);
	printk("success: %d\n", info.success);
	printk("comman_name: %s\n", info.comman_name);
	printk("public_key: %s\n", info.public_key);
	printk("private_key: %s\n", info.private_key);
	printk("secret: %s\n", info.secret);
	printk("security_key: %s\n", info.security_key);
	printk("factory_reset_id: %d\n", info.factory_reset_id);

     return;
}





/* Initialize AT communications */
int at_comms_init(void)
{
	int err;

	err = at_cmd_init();
	if (err) {
		printk("Failed to initialize AT commands, err %d\n", err);
		return err;
	}

	err = at_notif_init();
	if (err) {
		printk("Failed to initialize AT notifications, err %d\n", err);
		return err;
	}

	return 0;
}

/* Provision certificate to modem */
int cert_provision(void)
{
	int err;
	bool exists;
	u8_t unused;

	err = modem_key_mgmt_exists(TLS_SEC_TAG,
				    MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN,
				    &exists, &unused);
	if (err) {
		printk("Failed to check for certificates err %d\n", err);
		return err;
	}

	if (exists) {
		/* For the sake of simplicity we delete what is provisioned
		 * with our security tag and reprovision our certificate.
		 */
		err = modem_key_mgmt_delete(TLS_SEC_TAG,
					    MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN);
		if (err) {
			printk("Failed to delete existing certificate, err %d\n",
			       err);
		}
	}

	printk("Provisioning certificate\n");

	/*  Provision certificate to the modem */
	err = modem_key_mgmt_write(TLS_SEC_TAG,
				   MODEM_KEY_MGMT_CRED_TYPE_CA_CHAIN,
				   cert, sizeof(cert) - 1);
	if (err) {
		printk("Failed to provision certificate, err %d\n", err);
		return err;
	}

	return 0;
}

/* Setup TLS options on a given socket */
int tls_setup(int fd)
{
	int err;
	int verify;

	/* Security tag that we have provisioned the certificate with */
	const sec_tag_t tls_sec_tag[] = {
		TLS_SEC_TAG,
	};

	/* Set up TLS peer verification */
	enum {
		NONE = 0,
		OPTIONAL = 1,
		REQUIRED = 2,
	};

	verify = REQUIRED;

	err = setsockopt(fd, SOL_TLS, TLS_PEER_VERIFY, &verify, sizeof(verify));
	if (err) {
		printk("Failed to setup peer verification, err %d\n", errno);
		return err;
	}

	/* Associate the socket with the security tag
	 * we have provisioned the certificate with.
	 */
	err = setsockopt(fd, SOL_TLS, TLS_SEC_TAG_LIST, tls_sec_tag,
			 sizeof(tls_sec_tag));
	if (err) {
		printk("Failed to setup TLS sec tag, err %d\n", errno);
		return err;
	}

	return 0;
}


void https_main(void)
{
	int err;
	int fd;
	char *p;
	int bytes;
	size_t off;
	struct addrinfo *res;
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};

	printk("HTTPS client sample started\n\r");
#if 0
	err = bsdlib_init();
	if (err) {
		printk("Failed to initialize bsdlib!");
		return;
	}

	/* Initialize AT comms in order to provision the certificate */
	err = at_comms_init();
	if (err) {
		return;
	}

	/* Provision certificates before connecting to the LTE network */
	err = cert_provision();
	if (err) {
		return;
	}

	printk("Waiting for network.. ");
	err = lte_lc_init_and_connect();
	if (err) {
		printk("Failed to connect to the LTE network, err %d\n", err);
		return;
	}
	printk("OK\n");
#endif
        err = getaddrinfo(WEB_SERVER, NULL, &hints, &res);
	//err = getaddrinfo("baidu.com", NULL, &hints, &res);
	if (err) {
		printk("getaddrinfo() failed, err %d\n", errno);
		return;
	}

	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(HTTPS_PORT);

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
	if (fd == -1) {
		printk("Failed to open socket!\n");
		goto clean_up;
	}

	/* Setup TLS socket options */
	err = tls_setup(fd);
	if (err) {
		goto clean_up;
	}

	printk("Connecting to %s\n", WEB_SERVER);
	err = connect(fd, res->ai_addr, sizeof(struct sockaddr_in));
	if (err) {
		printk("connect() failed, err: %d\n", errno);
		goto clean_up;
	}

	off = 0;
	do {
		//bytes = send(fd, &send_buf[off], HTTP_HEAD_LEN - off, 0);
		bytes = send(fd, FIRST_BLOOD_REQUEST, strlen(FIRST_BLOOD_REQUEST), 0);
		if (bytes < 0) {
			printk("send() failed, err %d\n", errno);
			goto clean_up;
		}
		off += bytes;
	} while (off < HTTP_HEAD_LEN);

	printk("Sent %d bytes:\n%s\n", off,FIRST_BLOOD_REQUEST);

	off = 0;
	do {
		bytes = recv(fd, &recv_buf[off], RECV_BUF_SIZE - off, 0);
		if (bytes < 0) {
			printk("recv() failed, err %d\n", errno);
			goto clean_up;
		}
		off += bytes;

	} while (bytes != 0 /* peer closed connection */);

	printk("Received %d bytes\n", off);

	/* Print HTTP response */
	p = strstr(recv_buf, "\r\n");
	if (p) {
		off = p - recv_buf;
		//recv_buf[off + 1] = '\0';
		printk("\n>\t %s\n\n", recv_buf);
	}

        parseDataFromJson(recv_buf);

	printk("Finished, closing socket.\n");

clean_up:
	freeaddrinfo(res);
	(void)close(fd);
}


//////////////////////////////////////////////////////////////////////////////////////

#if 0
//  Converts the desired properties of the Device Twin JSON blob received from IoT Hub into a Car object.
void parseCertFromJson(const char* json)
{
    Cer_file* cerFile = malloc(sizeof(Cer_file));
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;
    bool flag = false;

    uint16_t public_key_len = 0;
    uint16_t private_key_len = 0;

    if (NULL == cerFile){
        //(void)printf("ERROR: Failed to allocate memory\r\n");
        debug_uart_print1("ERROR: Failed to allocate memory\r\n");
    }
    else {
        //printf("%s\r\n",json);
        debug_uart_print0(json, strlen(json));

        (void)memset(cerFile, 0, sizeof(Cer_file));

        root_value = json_parse_string(json);
        root_object = json_value_get_object(root_value);

        JSON_Value* success;
        JSON_Value* comman_name;
        JSON_Value* public_key;
        JSON_Value* private_key;
        JSON_Value* secret;
        JSON_Value* security_key;
        JSON_Value* factory_reset_id;

        success = json_object_dotget_value(root_object, "success");
        if (success != NULL)
        {
            cerFile->success = json_value_get_boolean(success);
            if(cerFile->success)
                flag = true;
        }
        if(flag) {
            comman_name = json_object_dotget_value(root_object, "data.cert.common_name");
            public_key = json_object_dotget_value(root_object, "data.cert.public_key");
            private_key = json_object_dotget_value(root_object, "data.cert.private_key");
            secret = json_object_dotget_value(root_object, "data.secret");
            security_key = json_object_dotget_value(root_object, "data.security_key");
            factory_reset_id = json_object_dotget_value(root_object, "data.factory_reset_id");
            if(comman_name != NULL) {
                const char* data = json_value_get_string(comman_name);
                if(data != NULL) {
                    cerFile->comman_name = malloc(strlen(data) + 1);
                    if(NULL != cerFile->comman_name) {
                        memset(cerFile->comman_name, 0x00, strlen(data) + 1 );
                        (void)strcpy(cerFile->comman_name, data);
                    }
                }
            }

            if(public_key != NULL) {
                const char* data = json_value_get_string(public_key);
                if(data != NULL) {
                    public_key_len = strlen(data);
                    //cerFile->public_key = malloc(strlen(data) + 1);
                    cerFile->public_key = malloc(2048);
                    if(NULL != cerFile->public_key) {
                        memset(cerFile->public_key, 0x00, 2048 );
                        (void)strcpy(cerFile->public_key, data);
                    }
                }
            }

            if(private_key != NULL) {
                const char* data = json_value_get_string(private_key);
                if(data != NULL) {
                    private_key_len = strlen(data);
                    //cerFile->private_key = malloc(strlen(data) + 1);
                    cerFile->private_key = malloc(2048);
                    if(NULL != cerFile->private_key) {
                        memset(cerFile->private_key, 0x00, 2048 );
                        (void)strcpy(cerFile->private_key, data);
                    }
                }
            }        

            if(secret != NULL) {
                const char* data = json_value_get_string(secret);
                if(data != NULL) {
                    cerFile->secret = malloc(strlen(data) + 1);
                    if(NULL != cerFile->secret) {
                        memset(cerFile->secret, 0x00, strlen(data) + 1 );
                        (void)strcpy(cerFile->secret, data);
                    }
                }
            }

            if(security_key != NULL) {
                const char* data = json_value_get_string(security_key);
                if(data != NULL) {
                    cerFile->security_key = malloc(strlen(data) + 1);
                    if(NULL != cerFile->security_key) {
                        memset(cerFile->security_key, 0x00, strlen(data) + 1 );
                        (void)strcpy(cerFile->security_key, data);
                    }
                }
            }

            if(factory_reset_id != NULL) {
                cerFile->factory_reset_id = (int)json_value_get_number(factory_reset_id);

                //save_i32(NVS_FACTORY_ID,cerFile->factory_reset_id);
                internfal_flash_rw_set_factory_reset_id(cerFile->factory_reset_id);
            } else {
                #if 0
                if(ESP_OK != read_i32(NVS_FACTORY_ID,(&cerFile->factory_reset_id))) {
                    cerFile->factory_reset_id = 1;
                    printf("Here is not factory_reset_id,factory_reset_id = %d\r\n", cerFile->factory_reset_id);
                    
                } else {
                    cerFile->factory_reset_id++;
                    printf("Here is not factory_reset_id,factory_reset_id = %d\r\n", cerFile->factory_reset_id);
                }

                save_i32(NVS_FACTORY_ID,cerFile->factory_reset_id);
                #else
                cerFile->factory_reset_id = internfal_flash_rw_get_factory_reset_id();
                cerFile->factory_reset_id ++;
                internfal_flash_rw_set_factory_reset_id(cerFile->factory_reset_id);
                #endif
            }

            if((comman_name != NULL) && (public_key != NULL) && (private_key != NULL) ){
                //printf("comman_name = %s\r\n", cerFile->comman_name);
                debug_uart_print1("comman_name = %s\r\n", cerFile->comman_name);
                

                //printf("public_key = %s\r\n", cerFile->public_key);
                debug_uart_print1("public_key = ");
                debug_uart_print0(cerFile->public_key, strlen(cerFile->public_key));
                debug_uart_print1("\r\n");
       

                //printf("private_key = %s\r\n", cerFile->private_key);
                debug_uart_print1("private_key = ");
                debug_uart_print0(cerFile->private_key, strlen(cerFile->private_key));
                debug_uart_print1("\r\n");


                //save_data(NVS_CM_ID,cerFile->comman_name);
                //save_data(NVS_CERT_ID,cerFile->public_key);
                //save_data(NVS_KEY_ID,cerFile->private_key);

                internfal_flash_rw_set_public_key(cerFile->public_key, public_key_len);
                vTaskDelay(300);
                internfal_flash_rw_set_private_key(cerFile->private_key, private_key_len);
                vTaskDelay(300);

                internfal_flash_rw_set_common_name(cerFile->comman_name, strlen(cerFile->comman_name));
            } else {
                //printf("Something is Empty\r\n");
                debug_uart_print1("Something is Empty\r\n");
            }

            if((security_key != NULL) && (secret != NULL)){
                //printf("secret = %s\r\n", cerFile->secret);
                debug_uart_print1("secret = %s\r\n", cerFile->secret);
               
                //printf("security_key = %s\r\n", cerFile->security_key);
                debug_uart_print1("security_key = %s\r\n", cerFile->security_key);

                //save_data(NVS_SECRET_ID,cerFile->secret);
                //save_data(NVS_SECURITYKEY_ID,cerFile->security_key);

                internfal_flash_rw_set_secret(cerFile->secret, strlen(cerFile->secret));
                internfal_flash_rw_set_security_key(cerFile->security_key, strlen(cerFile->security_key));

            } else {
                //printf("Something is Empty\r\n");
                debug_uart_print1("Something is Empty\r\n");
            }
            //printf("factory_reset_id = %d\r\n", cerFile->factory_reset_id);
            debug_uart_print1("factory_reset_id = %d\r\n", cerFile->factory_reset_id);
                              
            // esp_restart();
            
        }
        //printf("success = %d\r\n", (int)cerFile->success);
        debug_uart_print1("success = %d\r\n", (int)cerFile->success);


        if(cerFile->comman_name){
          free(cerFile->comman_name);
        }
        if(cerFile->public_key){
          free(cerFile->public_key);
        }
        if(cerFile->private_key){
          free(cerFile->private_key);
        }
        if(cerFile->secret ){
          free(cerFile->secret);
        }
        if(cerFile->security_key){
          free(cerFile->security_key);
        }

    }

    free(cerFile);
  
    internfal_flash_rw_set_dev_startup_mode(E_DEV_STARTUP_MODE_AZURE_DPS);
    internal_flash_rw_dev_info_save();
    vTaskDelay(300);
    user_system_reset();
}


void first_blood_task(void * pvParameter){

   uint8_t buffer[1024];
   char* pStr;
  // Set up a receive buffer (on the heap)
   uint8_t* recv_buffer = (uint8_t*)malloc(1024*5);
   uint16_t recv_buffer_index;
   // Socket::recv is called until we don't have any data anymore
   nsapi_size_or_error_t recv_ret;
   TickType_t tickstart;

    uint8_t try_init;
    try_init = 0;
    while(true){
        if (platform_init() != 0){
            //LogError("Platform initialization failed");
            debug_uart_print1("Platform initialization failed");

            try_init++;
            if( try_init >= 3){
                //restart system
                user_system_reset();
            }
           
        }else{
            break;
        }
    }

    struct TLSSocket *tls_socket = (struct TLSSocket *)malloc(sizeof(struct TLSSocket));
    if(tls_socket == NULL){
        //restart the system
        user_system_reset();
    }
    memset(tls_socket, 0x00, sizeof(struct TLSSocket) );

    TLSSocket_Init(tls_socket);

    tls_socket->open(tls_socket,network_interface);

    nsapi_error_t ret =  tls_socket->connect(tls_socket,WEB_SERVER,WEB_PORT);

    ret = tls_socket->tls_socket_wrapper.send(&tls_socket->tls_socket_wrapper,FIRST_BLOOD_REQUEST, strlen(FIRST_BLOOD_REQUEST));

    memset(recv_buffer, 0x00, 1024*5);
    recv_buffer_index = 0;
    tickstart = xTaskGetTickCount();
    while(true){

        memset(buffer, 0x00, sizeof(buffer) );
        recv_ret = tls_socket->tls_socket_wrapper.recv(&tls_socket->tls_socket_wrapper, buffer, (sizeof(buffer)-1) );
        if( recv_ret  < 0){
            break;
        }else if(recv_ret == 0 ){
            break;
        }else{
            memcpy(recv_buffer + recv_buffer_index, buffer, recv_ret);
            recv_buffer_index += recv_ret;
        }

        if(xTaskGetTickCount() - tickstart >  5000 ){    
           break;
        } 
    }
  
    //printf("%s",recv_buffer);
    debug_uart_print0(recv_buffer, strlen(recv_buffer));

#if 1
//looking the http body content
    pStr = strstr((char*)recv_buffer, "\r\n\r\n");
    if(pStr != NULL ){
        //printf("%s",pStr);
       parseCertFromJson( pStr+strlen("\r\n\r\n") );
    }

#endif 


    TLSSocket_DeInit(tls_socket);
    if(tls_socket){
        free(tls_socket);
    }

    if(recv_buffer){
        free(recv_buffer);
    }

    debug_uart_print1("first blood failed\r\n");

    //should not be here
    //restart the system
    user_system_reset();
}






void factory_reset_and_renew_task(void * pvParameter){

   uint8_t buffer[1024];
   char* pStr;
  // Set up a receive buffer (on the heap)
   uint8_t* recv_buffer = (uint8_t*)malloc(1024*5);
   uint16_t recv_buffer_index;
   // Socket::recv is called until we don't have any data anymore
   nsapi_size_or_error_t recv_ret;
   TickType_t tickstart;

    uint8_t try_init;
    try_init = 0;
    while(true){
        if (platform_init() != 0) {
            //LogError("Platform initialization failed");
          debug_uart_print1("Platform initialization failed");

           try_init++;
           if( try_init >= 3){
              //restart system
              user_system_reset();
           }
           
        }else{
          break;
        }
    }


    STRING_HANDLE encodedURL = URL_EncodeString((const char*)INTERNAL_FLASH_RW_PUBLIC_KEY_ADDR);
    const char * certUrl = STRING_c_str(encodedURL);
 
    int lenReq = strlen(REQUEST_RENEW) + strlen(certUrl) + 1 ;//+ strlen(certFile)
    char *reqStr = malloc(lenReq);
    sprintf_s(  reqStr, lenReq, REQUEST_RENEW, strlen(certUrl) + 11, certUrl);//,certFile

    //printf("%s\r\n%d\r\n",reqStr,strlen(reqStr));
    debug_uart_print1("renew http string:\r\n");
    debug_uart_print0(reqStr,strlen(reqStr));

    struct TLSSocket *tls_socket = (struct TLSSocket *)malloc(sizeof(struct TLSSocket));
    if(tls_socket == NULL){
        debug_uart_print1("TLSSocket initialization failed\r\n");
        //restart the system
        user_system_reset();
    }
    memset(tls_socket, 0x00, sizeof(struct TLSSocket) );

    TLSSocket_Init(tls_socket);

    tls_socket->open(tls_socket,network_interface);

    tls_socket->connect(tls_socket,WEB_SERVER,WEB_PORT);

    tls_socket->tls_socket_wrapper.send(&tls_socket->tls_socket_wrapper,reqStr, strlen(reqStr));

    memset(recv_buffer, 0x00, 1024*5);
    recv_buffer_index = 0;
    tickstart = xTaskGetTickCount();
    while(true) {
      
        memset(buffer, 0x00, sizeof(buffer) );
        recv_ret = tls_socket->tls_socket_wrapper.recv(&tls_socket->tls_socket_wrapper, buffer, (sizeof(buffer)-1) );
        if( recv_ret  < 0) {
            break;
        }else if(recv_ret == 0 ) {
            break;
        }else {
            memcpy(recv_buffer + recv_buffer_index, buffer, recv_ret);
            recv_buffer_index += recv_ret;
        }

        if(xTaskGetTickCount() - tickstart >  5000 ) {    
            debug_uart_print1("TLSSocket recv timeout\r\n");
            break;
        } 

    }
  
    //printf("%s",recv_buffer);
    debug_uart_print0(recv_buffer, strlen(recv_buffer));

//looking the http body content
    pStr = strstr((char*)recv_buffer, "\r\n\r\n");
    if(pStr != NULL ){
        //printf("%s",pStr);
        parseCertFromJson(pStr+strlen("\r\n\r\n"));
    }

    STRING_delete(encodedURL);

    if(reqStr){
      free(reqStr);
    }


    TLSSocket_DeInit(tls_socket);
    if(tls_socket){
      free(tls_socket);
    }

    if(recv_buffer){
      free(recv_buffer);
    }

    debug_uart_print1("renew cert failed\r\n");

    //should not be here
    //restart the system
    user_system_reset();
}


//////////////////////////////////////////////////////////////////////////////////////

#endif
