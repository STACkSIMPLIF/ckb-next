#include "command.h"
#include "device.h"
#include "input.h"
#include "led.h"
#include "profile.h"

iconv_t utf8to16 = 0, utf16to8 = 0;

void urldecode2(char *dst, const char *src){
    char a, b;
    char s;
    while((s = *src)){
        if((s == '%') &&
                ((a = src[1]) && (b = src[2])) &&
                (isxdigit(a) && isxdigit(b))){
            if(a >= 'a')
                a -= 'a'-'A';
            if(a >= 'A')
                a -= 'A' - 10;
            else
                a -= '0';
            if(b >= 'a')
                b -= 'a'-'A';
            if(b >= 'A')
                b -= 'A' - 10;
            else
                b -= '0';
            *dst++ = 16*a + b;
            src += 3;
        } else {
            *dst++ = s;
            src++;
        }
    }
    *dst = '\0';
}

void urlencode2(char* dst, const char* src){
    char s;
    while((s = *src++)){
        if(s <= ',' || s == '/' ||
                (s >= ':' && s <= '@') ||
                s == '[' || s == ']' ||
                s >= 0x7F){
            char a = s >> 4, b = s & 0xF;
            if(a >= 10)
                a += 'A' - 10;
            else
                a += '0';
            if(b >= 10)
                b += 'A' - 10;
            else
                b += '0';
            dst[0] = '%';
            dst[1] = a;
            dst[2] = b;
            dst += 3;
        } else
            *dst++ = s;
    }
    *dst = '\0';
}

void cmd_name(usbdevice* kb, usbmode* mode, int dummy1, int dummy2, const char* name){
    if(!utf8to16)
        utf8to16 = iconv_open("UTF-16LE", "UTF-8");
    memset(mode->name, 0, sizeof(mode->name));
    char decoded[strlen(name) + 1];
    urldecode2(decoded, name);
    size_t srclen = strlen(decoded), dstlen = MD_NAME_LEN * 2 - 2;
    char* in = decoded, *out = (char*)mode->name;
    iconv(utf8to16, &in, &srclen, &out, &dstlen);
}

void cmd_profilename(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* name){
    usbprofile* profile = kb->profile;
    if(!utf8to16)
        utf8to16 = iconv_open("UTF-16LE", "UTF-8");
    memset(profile->name, 0, sizeof(profile->name));
    char decoded[strlen(name) + 1];
    urldecode2(decoded, name);
    size_t srclen = strlen(decoded), dstlen = PR_NAME_LEN * 2 - 2;
    char* in = decoded, *out = (char*)profile->name;
    iconv(utf8to16, &in, &srclen, &out, &dstlen);
}

void nameconv(ushort* in, char* out, size_t* srclen, size_t* dstlen){
    if(!utf16to8)
        utf16to8 = iconv_open("UTF-8", "UTF-16LE");
    size_t srclen2 = 0;
    for(; srclen2 < MD_NAME_LEN; srclen2++){
        if(!in[srclen2])
            break;
    }
    *srclen = srclen2 * 2;
    iconv(utf16to8, (char**)&in, srclen, &out, dstlen);
}

char* printname(ushort* name, int length){
    // Convert the name to UTF-8
    char* buffer = calloc(1, length * 4 - 3);
    size_t srclen, dstlen = length * 4 - 4;
    nameconv(name, buffer, &srclen, &dstlen);
    // URL-encode it
    char* buffer2 = malloc(strlen(buffer) * 3 + 1);
    urlencode2(buffer2, buffer);
    free(buffer);
    return buffer2;
}

char* getmodename(usbmode* mode){
    return printname(mode->name, MD_NAME_LEN);
}

char* getprofilename(usbprofile* profile){
    return printname(profile->name, PR_NAME_LEN);
}

char* gethwmodename(hwprofile* profile, int index){
    return printname(profile->name[index + 1], MD_NAME_LEN);
}

char* gethwprofilename(hwprofile* profile){
    return printname(profile->name[0], MD_NAME_LEN);
}

static void initmode(usbmode* mode){
    memset(mode, 0, sizeof(*mode));
    mode->light.forceupdate = 1;
    initbind(&mode->bind);
}

void allocprofile(usbdevice* kb){
    if(kb->profile)
        return;
    usbprofile* profile = kb->profile = calloc(1, sizeof(usbprofile));
    for(int i = 0; i < MODE_COUNT; i++)
        initmode(profile->mode + i);
    profile->currentmode = profile->mode;
    profile->lastlight.forceupdate = 1;
}

int loadprofile(usbdevice* kb){
    if(hwloadprofile(kb, 1))
        return -1;
    return 0;
}

static void freemode(usbmode* mode){
    freebind(&mode->bind);
    memset(mode, 0, sizeof(*mode));
}

void cmd_erase(usbdevice* kb, usbmode* mode, int dummy1, int dummy2, const char* dummy3){
    pthread_mutex_lock(imutex(kb));
    freemode(mode);
    initmode(mode);
    pthread_mutex_unlock(imutex(kb));
}

static void _freeprofile(usbdevice* kb){
    usbprofile* profile = kb->profile;
    if(!profile)
        return;
    // Clear all mode data
    for(int i = 0; i < MODE_COUNT; i++)
        freemode(profile->mode + i);
    free(profile);
    kb->profile = 0;
}

void cmd_eraseprofile(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    pthread_mutex_lock(imutex(kb));
    _freeprofile(kb);
    allocprofile(kb);
    pthread_mutex_unlock(imutex(kb));
}

void freeprofile(usbdevice* kb){
    _freeprofile(kb);
    // Also free HW profile
    free(kb->hw);
    kb->hw = 0;
}

void cmd_id(usbdevice* kb, usbmode* mode, int dummy1, int dummy2, const char* id){
    // ID is either a GUID or an 8-digit hex number
    int newmodified;
    if(!setid(&mode->id, id) && sscanf(id, "%08x", &newmodified) == 1)
        memcpy(mode->id.modified, &newmodified, sizeof(newmodified));
}

void cmd_profileid(usbdevice* kb, usbmode* mode, int dummy1, int dummy2, const char* id){
    usbprofile* profile = kb->profile;
    int newmodified;
    if(!setid(&profile->id, id) && sscanf(id, "%08x", &newmodified) == 1)
        memcpy(profile->id.modified, &newmodified, sizeof(newmodified));

}

int setid(usbid* id, const char* guid){
    int32_t data1;
    int16_t data2, data3, data4a;
    char data4b[6];
    if(sscanf(guid, "{%08X-%04hX-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
              &data1, &data2, &data3, &data4a, data4b, data4b + 1, data4b + 2, data4b + 3, data4b + 4, data4b + 5) != 10)
        return 0;
    memcpy(id->guid + 0x0, &data1, 4);
    memcpy(id->guid + 0x4, &data2, 2);
    memcpy(id->guid + 0x6, &data3, 2);
    memcpy(id->guid + 0x8, &data4a, 2);
    memcpy(id->guid + 0xA, data4b, 6);
    return 1;
}

char* getid(usbid* id){
    int32_t data1;
    int16_t data2, data3, data4a;
    char data4b[6];
    memcpy(&data1, id->guid + 0x0, 4);
    memcpy(&data2, id->guid + 0x4, 2);
    memcpy(&data3, id->guid + 0x6, 2);
    memcpy(&data4a, id->guid + 0x8, 2);
    memcpy(data4b, id->guid + 0xA, 6);
    char* guid = malloc(39);
    snprintf(guid, 39, "{%08X-%04hX-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
             data1, data2, data3, data4a, data4b[0], data4b[1], data4b[2], data4b[3], data4b[4], data4b[5]);
    return guid;
}

// Converts a hardware profile to a native profile
void hwtonative(usbprofile* profile, hwprofile* hw, int modes){
    // Copy the profile and mode names
    memcpy(profile->name, hw->name[0], PR_NAME_LEN * 2);
    for(int i = 0; i < modes; i++)
        memcpy(profile->mode[i].name, hw->name[i + 1], MD_NAME_LEN * 2);
    // Copy the profile and mode IDs
    memcpy(&profile->id, hw->id, sizeof(usbid));
    for(int i = 0; i < modes; i++)
        memcpy(&profile->mode[i].id, hw->id + i + 1, sizeof(usbid));
    // Copy the key lighting
    for(int i = 0; i < modes; i++)
        memcpy(&profile->mode[i].light, hw->light + i, sizeof(lighting));
}

// Converts a native profile to a hardware profile
void nativetohw(usbprofile* profile, hwprofile* hw, int modes){
    // Copy the profile and mode names
    memcpy(hw->name[0], profile->name, PR_NAME_LEN * 2);
    for(int i = 0; i < modes; i++)
        memcpy(hw->name[i + 1], profile->mode[i].name, MD_NAME_LEN * 2);
    // Copy the profile and mode IDs
    memcpy(hw->id, &profile->id, sizeof(usbid));
    for(int i = 0; i < modes; i++)
        memcpy(hw->id + i + 1, &profile->mode[i].id, sizeof(usbid));
    // Copy the key lighting
    for(int i = 0; i < modes; i++)
        memcpy(hw->light + i, &profile->mode[i].light, sizeof(lighting));
}

int hwloadmode(usbdevice* kb, hwprofile* hw, int mode){
    // Ask for mode's name
    uchar data_pkt[MSG_SIZE] = { 0x0e, 0x16, 0x01, mode + 1, 0 };
    if(!usbsend(kb, data_pkt, 1))
        return -1;
    // Wait for the response
    if(!usbrecv(kb, data_pkt))
        return -1;
    memcpy(hw->name[mode + 1], data_pkt + 4, MD_NAME_LEN * 2);
    // Load the RGB setting
    return loadrgb_kb(kb, hw->light + mode, mode);
}

int cmd_hwload(usbdevice* kb, usbmode* dummy1, int dummy2, int apply, const char* dummy3){
    DELAY_LONG(kb);
    hwprofile* hw = calloc(1, sizeof(hwprofile));
    // Ask for profile and mode IDs
    uchar data_pkt[2][MSG_SIZE] = {
        { 0x0e, 0x15, 0x01, 0 },
        { 0x0e, 0x16, 0x01, 0 }
    };
    uchar in_pkt[MSG_SIZE];
    int modes = (IS_K95(kb) ? HWMODE_K95 : HWMODE_K70);
    for(int i = 0; i <= modes; i++){
        data_pkt[0][3] = i;
        if(!usbsend(kb, data_pkt[0], 1)){
            free(hw);
            return -1;
        }
        // Wait for the response
        if(!usbrecv(kb, in_pkt)){
            free(hw);
            return -1;
        }
        memcpy(hw->id + i, in_pkt + 4, sizeof(usbid));
    }
    // Ask for profile name
    if(!usbsend(kb, data_pkt[1], 1)){
        free(hw);
        return -1;
    }
    // Wait for the response
    if(!usbrecv(kb, in_pkt)){
        free(hw);
        return -1;
    }
    memcpy(hw->name[0], in_pkt + 4, PR_NAME_LEN * 2);
    // Load modes
    for(int i = 0; i < modes; i++){
        if(hwloadmode(kb, hw, i)){
            free(hw);
            return -1;
        }
    }
    // Make the profile active (if requested)
    if(apply)
        hwtonative(kb->profile, hw, modes);
    // Free the existing profile (if any)
    free(kb->hw);
    kb->hw = hw;
    kb->profile->lastlight.forceupdate = 1;
    DELAY_LONG(kb);
    return 0;
}

int cmd_hwsave(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    DELAY_LONG(kb);
    hwprofile* hw = kb->hw;
    if(!hw)
        hw = kb->hw = calloc(1, sizeof(hwprofile));
    int modes = (IS_K95(kb) ? HWMODE_K95 : HWMODE_K70);
    nativetohw(kb->profile, hw, modes);
    kb->profile->lastlight.forceupdate = 1;
    // Save the profile and mode names
    uchar data_pkt[2][MSG_SIZE] = {
        {0x07, 0x16, 0x01, 0 },
        {0x07, 0x15, 0x01, 0 },
    };
    // Save the mode names
    for(int i = 0; i <= modes; i++){
        data_pkt[0][3] = i;
        memcpy(data_pkt[0] + 4, hw->name[i], MD_NAME_LEN * 2);
        if(!usbsend(kb, data_pkt[0], 1))
            return -1;
    }
    // Save the IDs
    for(int i = 0; i <= modes; i++){
        data_pkt[1][3] = i;
        memcpy(data_pkt[1] + 4, hw->id + i, sizeof(usbid));
        if(!usbsend(kb, data_pkt[1], 1))
            return -1;
    }
    // Save the RGB data
    for(int i = 0; i < modes; i++)
        savergb_kb(kb, i);
    DELAY_LONG(kb);
    return 0;
}
