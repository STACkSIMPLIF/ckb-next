#include "led.h"
#include "notify.h"
#include "profile.h"
#include "usb.h"
#include "nxp_proto.h"
#include "dpi.h"
#include <stdbool.h>

// Compare two light structures, ignore keys
static inline int rgbcmp(const lighting* lhs, const lighting* rhs){
    return memcmp(lhs->r + LED_MOUSE, rhs->r + LED_MOUSE, N_MOUSE_ZONES) || memcmp(lhs->g + LED_MOUSE, rhs->g + LED_MOUSE, N_MOUSE_ZONES) || memcmp(lhs->b + LED_MOUSE, rhs->b + LED_MOUSE, N_MOUSE_ZONES);
}

static inline int dpirgbcmp(const lighting* lhs, const lighting* rhs){
    return memcmp(lhs->r + DPI_RGB_START, rhs->r + DPI_RGB_START, DPI_COUNT) || memcmp(lhs->g + DPI_RGB_START, rhs->g + DPI_RGB_START, DPI_COUNT) || memcmp(lhs->b + DPI_RGB_START, rhs->b + DPI_RGB_START, DPI_COUNT);
}

int updatergb_mouse(usbdevice* kb, int force){
    if(!kb->active)
        return 0;
    lighting* lastlight = &kb->profile->lastlight;
    lighting* newlight = &kb->profile->currentmode->light;
    bool fupdate = force || lastlight->forceupdate || newlight->forceupdate;

    // Force a DPI update if the rgb0-rgb5 zones have changed
    // This needs to be above the main rgbcmp check as it doesn't return
    if(NXP_RGB_IN_DPI_PKT(kb) && (fupdate || dpirgbcmp(lastlight, newlight)))
        updatedpi(kb, 1);

    // Don't do anything if the lighting hasn't changed
    if(!(fupdate || rgbcmp(lastlight, newlight)))
        return 0;
    lastlight->forceupdate = newlight->forceupdate = 0;

    // The Dark Core has its own lighting protocol.
    if(IS_DARK_CORE_NXP(kb))
        return updatergb_wireless(kb, lastlight, newlight);

    // Prevent writing to DPI LEDs or non-existent LED zones for the Glaive.
    int num_zones = IS_GLAIVE(kb) ? 3 : N_MOUSE_ZONES;
    // Send the RGB values for each zone to the mouse
    uchar data_pkt[MSG_SIZE] = { CMD_SET, FIELD_M_COLOR, num_zones, 0x01, 0 }; // RGB colors
    uchar* rgb_data = data_pkt + 4;
    for(int i = 0; i < N_MOUSE_ZONES; i++){
        if (IS_GLAIVE(kb) && i != 0 && i != 1 && i != 5)
            continue;
        *rgb_data++ = i + 1;
        *rgb_data++ = newlight->r[LED_MOUSE + i];
        *rgb_data++ = newlight->g[LED_MOUSE + i];
        *rgb_data++ = newlight->b[LED_MOUSE + i];
    }
    // Send RGB data
    if(!usbsend(kb, data_pkt, sizeof(data_pkt), 1))
        return -1;

    memcpy(lastlight, newlight, sizeof(lighting));
    return 0;
}

int savergb_mouse(usbdevice* kb, lighting* light, int mode){
    (void)mode;

    uchar data_pkt[MSG_SIZE] = { CMD_SET, FIELD_MOUSE, MOUSE_HWCOLOR, 1, 0 };
    // Save each RGB zone, minus the DPI light which is sent in the DPI packets
    int zonecount = IS_SCIMITAR(kb) ? 4 : IS_SABRE(kb) ? 3 : 2;
    for(int i = 0; i < zonecount; i++){
        int led = LED_MOUSE + i;
        if(led >= LED_DPI)
            led++;          // Skip DPI light
        data_pkt[4] = light->r[led];
        data_pkt[5] = light->g[led];
        data_pkt[6] = light->b[led];
        if(!usbsend(kb, data_pkt, sizeof(data_pkt), 1))
            return -1;
        // Set packet for next zone
        data_pkt[2]++;
    }
    return 0;
}

int loadrgb_mouse(usbdevice* kb, lighting* light, int mode){
    (void)mode;

    uchar data_pkt[MSG_SIZE] = { CMD_GET, FIELD_MOUSE, MOUSE_HWCOLOR, 1, 0 };
    uchar in_pkt[MSG_SIZE] = { 0 };
    // Load each RGB zone
    int zonecount = IS_SCIMITAR(kb) ? 4 : IS_SABRE(kb) ? 3 : 2;
    for(int i = 0; i < zonecount; i++){
        if(!usbrecv(kb, data_pkt, sizeof(data_pkt), in_pkt))
            return -1;
        if(memcmp(in_pkt, data_pkt, 4)){
            ckb_err("Bad input header");
            return -2;
        }
        // Copy data
        int led = LED_MOUSE + i;
        if(led >= LED_DPI)
            led++;          // Skip DPI light
        light->r[led] = in_pkt[4];
        light->g[led] = in_pkt[5];
        light->b[led] = in_pkt[6];
        // Set packet for next zone
        data_pkt[2]++;
    }
    return 0;
}

#define MOUSE_BACK_LED              LED_MOUSE + 1

int updatergb_mouse_legacy(usbdevice* kb, int force){
    lighting* lastlight = &kb->profile->lastlight;
    lighting* newlight = &kb->profile->currentmode->light;
    // Don't do anything if the lighting hasn't changed
    // We only use "back" on the legacy M95 for consistency
    // 0x00 on R, G and B channels is off
    // Anything else is on
    ushort lastwValue = !!(lastlight->r[MOUSE_BACK_LED] + lastlight->g[MOUSE_BACK_LED] + lastlight->b[MOUSE_BACK_LED]);
    ushort newwValue = !!(newlight->r[MOUSE_BACK_LED] + newlight->g[MOUSE_BACK_LED] + newlight->b[MOUSE_BACK_LED]);
    //ckb_info("last w %u, neww %u", lastwValue, newwValue);
    if(!force && !lastlight->forceupdate && !newlight->forceupdate && lastwValue == newwValue)
        return 0;
    lastlight->forceupdate = newlight->forceupdate = 0;

    lastlight->r[MOUSE_BACK_LED] = newlight->r[MOUSE_BACK_LED];
    lastlight->g[MOUSE_BACK_LED] = newlight->g[MOUSE_BACK_LED];
    lastlight->b[MOUSE_BACK_LED] = newlight->b[MOUSE_BACK_LED];

    usbsend(kb, (&(ctrltransfer) { .bRequestType = 0x40, .bRequest = 49, .wValue = newwValue, .wIndex = 0, .wLength = 0, .timeout = 5000, .data = NULL }), 0, 1);
    return 0;
}
