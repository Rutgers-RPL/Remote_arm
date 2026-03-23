#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const ble_uuid128_t service_uuid = BLE_UUID128_INIT(0x4b,0x91,0x31,0xc9,0x33,0xc5,0xcc,0x8f,0x9e,0x45,0xb5,0x1f,0x01,0xc2,0xaf,0x4f);

static const ble_uuid128_t write_uuid = BLE_UUID128_INIT(0xa8,0x26,0x1b,0x36,0x07,0xea,0xf5,0xb7,0x88,0x46,0xe1,0x36,0x3e,0x48,0xb5,0xbe);

static const ble_uuid128_t notify_uuid = BLE_UUID128_INIT(0x47,0x47,0x85,0x32,0xc3,0x73,0x6a,0x8a,0x48,0x41,0x01,0xad,0x9b,0x59,0x8c,0x49);

#define CHANNEL_1 14
#define CHANNEL_2 25
#define CHANNEL_3 26
#define CHANNEL_4 27

static uint16_t notify_handle;
static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;

void init_pins() {
    gpio_reset_pin(CHANNEL_1);
    gpio_reset_pin(CHANNEL_2);
    gpio_reset_pin(CHANNEL_3);
    gpio_reset_pin(CHANNEL_4);

    gpio_set_direction(CHANNEL_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(CHANNEL_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(CHANNEL_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(CHANNEL_4, GPIO_MODE_OUTPUT);

    gpio_set_level(CHANNEL_1, 0);
    gpio_set_level(CHANNEL_2, 0);
    gpio_set_level(CHANNEL_3, 0);
    gpio_set_level(CHANNEL_4, 0);
}

void send_notification(const char *msg) {
    if (conn_handle == BLE_HS_CONN_HANDLE_NONE) return;

    struct os_mbuf *om = ble_hs_mbuf_from_flat(msg, strlen(msg));
    ble_gatts_notify_custom(conn_handle, notify_handle, om);
}

void handle_pin_toggle(const char *val) {
    if (strlen(val) < 6) return;

    char pin = val[3];
    const char *action = &val[5];

    int level;
    if (strcmp(action, "ARM") == 0) {
        level = 1;
    } else if (strcmp(action, "DISARM") == 0) {
        level = 0;
    } else {
        return;
    }

    switch (pin) {
        case '1': gpio_set_level(CHANNEL_1, level); break;
        case '2': gpio_set_level(CHANNEL_2, level); break;
        case '3': gpio_set_level(CHANNEL_3, level); break;
        case '4': gpio_set_level(CHANNEL_4, level); break;
        default: return;
    }

    char msg[32];
    snprintf(msg, sizeof(msg), "B0_%c_%sED", pin, action);
    send_notification(msg);
}

static int write_cb(uint16_t conn_handle_in, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    char buf[64] = {0};
    int len = ctxt->om->om_len;
    memcpy(buf, ctxt->om->om_data, len);

    printf("Received: %s\n", buf);

    if (len > 1 && buf[1] == '0') {
        handle_pin_toggle(buf);
    }

    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &service_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &write_uuid.u,
                .access_cb = write_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {
                .uuid = &notify_uuid.u,
                .val_handle = &notify_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
            },
            {0}
        },
    },
    {0}
};

static int gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                conn_handle = event->connect.conn_handle;
                send_notification("DISARMED");
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            conn_handle = BLE_HS_CONN_HANDLE_NONE;
            ble_gap_adv_start(0, NULL, BLE_HS_FOREVER, NULL, gap_event, NULL);
            break;

        default:
            break;
    }
    return 0;
}

void start_advertising() {
    struct ble_gap_adv_params adv_params = {0};

    struct ble_hs_adv_fields fields = {0};
    fields.name = (uint8_t *)"RRPL Rocket";
    fields.name_len = strlen("RRPL Rocket");
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(0, NULL, BLE_HS_FOREVER, &adv_params, gap_event, NULL);
}

void ble_app_on_sync(void) {
    start_advertising();
}

void host_task(void *param) {
    nimble_port_run();
}

void app_main(void) {
    nvs_flash_init();
    init_pins();

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = ble_app_on_sync;

    nimble_port_freertos_init(host_task);
}