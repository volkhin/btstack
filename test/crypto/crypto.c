#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

#include "btstack_run_loop_posix.h"

#include "hci_cmd.h"
#include "btstack_util.h"
#include "btstack_debug.h"
#include "btstack_memory.h"
#include "btstack_crypto.h"
#include "hci.h"
#include "hci_dump.h"

void mock_init(void);
void mock_simulate_hci_event(uint8_t * packet, uint16_t size);
void aes128_report_result(void);
uint8_t * mock_packet_buffer(void);
uint16_t mock_packet_buffer_len(void);
void mock_clear_packet_buffer(void);

void CHECK_EQUAL_ARRAY(uint8_t * expected, uint8_t * actual, int size){
	int i;
	for (i=0; i<size; i++){
		if (expected[i] != actual[i]) {
			printf("offset %u wrong\n", i);
			printf("expected: "); printf_hexdump(expected, size);
			printf("actual:   "); printf_hexdump(actual, size);
		}
		BYTES_EQUAL(expected[i], actual[i]);
	}
}

static int parse_hex(uint8_t * buffer, const char * hex_string){
    int len = 0;
    while (*hex_string){
        if (*hex_string == ' '){
            hex_string++;
            continue;
        }
        int high_nibble = nibble_for_char(*hex_string++);
        int low_nibble = nibble_for_char(*hex_string++);
        *buffer++ = (high_nibble << 4) | low_nibble;
        len++;
    }
    return len;
}

#if 0
static void validate_message(const char * name, const char * message_string, const char * cmac_string){

    btstack_crypto_aes128_cmac_t btstack_crypto_aes128_cmac;
    
    mock_clear_packet_buffer();
    int len = parse_hex(m, message_string);

    // expected result
    sm_key_t cmac;
    parse_hex(cmac, cmac_string);

    printf("-- verify key %s message %s, len %u:\nm:    %s\ncmac: %s\n", key_string, name, len, message_string, cmac_string);

    sm_key_t key;
    parse_hex(key, key_string);
    // printf_hexdump(key, 16);

    cmac_hash_received = 0;
    btstack_crypto_aes128_cmac_message(&btstack_crypto_aes128_cmac, key, len, m, cmac_hash, &cmac_done2, NULL);
    while (!cmac_hash_received){
        aes128_report_result();
    }
    CHECK_EQUAL_ARRAY(cmac, cmac_hash, 16);
}

#define VALIDATE_MESSAGE(NAME) validate_message(#NAME, NAME##_string, cmac_##NAME##_string)
#endif

static uint8_t zero[16] = { 0 };
static btstack_crypto_aes128_t crypto_aes128_request;
static btstack_crypto_ccm_t    crypto_ccm_request;
static uint8_t ciphertext[16];
static uint8_t plaintext[32];

static int crypto_done;
static void crypto_done_callback(void * arg){
    crypto_done = 1;
}
static void perform_crypto_operation(void){
    crypto_done = 0;
    while (!crypto_done){
        aes128_report_result();
    }
}

TEST_GROUP(Crypto){
	void setup(void){
        static int first = 1;
        if (first){
            first = 0;
            btstack_memory_init();
            btstack_run_loop_init(btstack_run_loop_posix_get_instance());            
        }
        btstack_crypto_init();
    }
};

TEST(Crypto, AES128){
    mock_init();
    btstack_crypto_aes128_encrypt(&crypto_aes128_request, zero, zero, ciphertext, &crypto_done_callback, NULL);
    perform_crypto_operation();
    // printf_hexdump(ciphertext, 16);
    uint8_t expected[16];
    parse_hex(expected, "66e94bd4ef8a2c3b884cfa59ca342b2e");
    CHECK_EQUAL_ARRAY(expected, ciphertext, 16);
}

TEST(Crypto, AES_CCM){
    mock_init();
    uint8_t expected_plaintext[25];
    uint8_t key[16];
    uint8_t nonce[16];
    uint8_t enc_data[25];

    parse_hex(enc_data, "d0bd7f4a89a2ff6222af59a90a60ad58acfe3123356f5cec29");
    parse_hex(key,      "c80253af86b33dfa450bbdb2a191fea3");  // session key
    parse_hex(nonce,    "da7ddbe78b5f62b81d6847487e");        // session nonce
    btstack_crypo_ccm_init(&crypto_ccm_request, key, nonce, 25);
    btstack_crypto_ccm_decrypt_block(&crypto_ccm_request, 25, enc_data, plaintext, &crypto_done_callback, NULL);
    perform_crypto_operation();
    // printf_hexdump(plaintext, 25);
    parse_hex(expected_plaintext, "efb2255e6422d330088e09bb015ed707056700010203040b0c");
    CHECK_EQUAL_ARRAY(expected_plaintext, plaintext, 25);
}

int main (int argc, const char * argv[]){
    // hci_dump_open("hci_dump.pklg", HCI_DUMP_PACKETLOGGER);
    return CommandLineTestRunner::RunAllTests(argc, argv);
}