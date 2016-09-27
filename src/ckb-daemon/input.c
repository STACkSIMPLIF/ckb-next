#include "usb.h"
#include "input.h"

int uinputopen(int index, const struct libusb_device_descriptor* descriptor){
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd <= 0){
        // If that didn't work, try /dev/input/uinput instead
        fd = open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);
        if(fd <= 0){
            printf("Error: Failed to open uinput: %s\n", strerror(errno));
            return fd;
        }
    }
    printf("Setting up uinput device ckb%d\n", index);
    // Set up as a keyboard device and enable all keys as well as all LEDs
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_LED);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);
    for(int i = 0; i < 256; i++)
        ioctl(fd, UI_SET_KEYBIT, i);
    for(int i = 0; i < LED_CNT; i++)
        ioctl(fd, UI_SET_LEDBIT, i);
    // Create the new input device
    struct uinput_user_dev indev;
    memset(&indev, 0, sizeof(indev));
    snprintf(indev.name, UINPUT_MAX_NAME_SIZE, "ckb%d", index);
    indev.id.bustype = BUS_USB;
    indev.id.vendor = descriptor->idVendor;
    indev.id.product = descriptor->idProduct;
    indev.id.version = (UINPUT_VERSION > 4 ? 4 : UINPUT_VERSION);
    if(write(fd, &indev, sizeof(indev)) <= 0)
        printf("Write error: %s\n", strerror(errno));
    if(ioctl(fd, UI_DEV_CREATE)){
        printf("Error: Failed to create uinput device: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    // Get event device. Needed to listen to indicator LEDs (caps lock, etc)
    char uiname[256], uipath[FILENAME_MAX];
    const char* uidevbase = "/sys/devices/virtual/input";
#if UINPUT_VERSION >= 4
    if(ioctl(fd, UI_GET_SYSNAME(256), uiname) >= 0)
        snprintf(uipath, FILENAME_MAX, "%s/%s", uidevbase, uiname);
    else {
        printf("Warning: Couldn't get uinput path (trying workaround): %s\n", strerror(errno));
#endif
        // If the system's version of uinput doesn't support getting the device name, or if it failed, scan the directory for this device
        DIR* uidir = opendir(uidevbase);
        if(!uidir){
            printf("Warning: Couldn't open virtual device path: %s\n", strerror(errno));
            return fd;
        }
        struct dirent* uifile;
        while((uifile = readdir(uidir))){
            int uilength;
            snprintf(uipath, FILENAME_MAX, "%s/%s%n/name", uidevbase, uifile->d_name, &uilength);
            int namefd = open(uipath, O_RDONLY);
            if(namefd > 0){
                char name[10] = { 0 }, trimmedname[10] = { 0 };
                ssize_t len = read(namefd, name, 9);
                sscanf(name, "%9s", trimmedname);
                if(len >= 0 && !strcmp(trimmedname, indev.name)){
                    uipath[uilength] = 0;
                    strcpy(uiname, uifile->d_name);
                    break;
                }
                close(namefd);
            }
            uipath[0] = 0;
        }
        closedir(uidir);
#if UINPUT_VERSION >= 4
    }
#endif
    if(strlen(uipath) > 0){
        printf("%s/%s set up\n", uidevbase, uiname);
        // Look in the uinput directory for a file named event*
        DIR* dir = opendir(uipath);
        if(!dir){
            printf("Warning: Couldn't open uinput path: %s\n", strerror(errno));
            return fd;
        }
        int eid = -1;
        struct dirent* file;
        while((file = readdir(dir))){
            if(sscanf(file->d_name, "event%d", &eid) == 1)
                break;
        }
        closedir(dir);
        if(eid < 0){
            printf("Warning: Couldn't find event at uinput path\n");
            return fd;
        }
        // Open the corresponding device in /dev/input
        snprintf(uipath, FILENAME_MAX, "/dev/input/event%d", eid);
        int fd2 = open(uipath, O_RDONLY | O_NONBLOCK);
        if(fd2 <= 0){
            printf("Warning: Couldn't open event device: %s\n", strerror(errno));
            return fd;
        }
        keyboard[index].event = fd2;
        printf("Opened /dev/input/event%d\n", eid);
    }
    return fd;
}

void uinputclose(int index){
    usbdevice* kb = keyboard + index;
    if(kb->uinput <= 0)
        return;
    printf("Closing uinput device %d\n", index);
    close(kb->event);
    kb->event = 0;
    // Set all keys released
    struct input_event event;
    memset(&event, 0, sizeof(event));
    event.type = EV_KEY;
    for(int key = 0; key < 256; key++){
        event.code = key;
        if(write(kb->uinput, &event, sizeof(event)) <= 0)
            printf("Write error: %s\n", strerror(errno));
    }
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    if(write(kb->uinput, &event, sizeof(event)) <= 0)
        printf("Write error: %s\n", strerror(errno));
    // Close the device
    ioctl(kb->uinput, UI_DEV_DESTROY);
    close(kb->uinput);
    kb->uinput = 0;
}

int macromask(const char* key1, const char* key2){
    // Scan a macro against key input. Return 0 if any of them don't match
    for(int i = 0; i < N_KEYS / 8; i++){
        if((key1[i] & key2[i]) != key2[i])
            return 0;
    }
    return 1;
}

void uinputupdate(usbdevice* kb){
    keybind* bind = &kb->bind;
    // Don't do anything if the state hasn't changed
    if(!memcmp(kb->previntinput, kb->intinput, N_KEYS / 8))
        return;
    // Look for macros matching the current state
    int macrotrigger = 0;
    for(int i = 0; i < bind->macrocount; i++){
        keymacro* macro = &bind->macros[i];
        if(macromask(kb->intinput, macro->combo)){
            if(!macro->triggered){
                macrotrigger = 1;
                macro->triggered = 1;
                // Send events for each keypress in the macro
                struct input_event event;
                for(int a = 0; a < macro->actioncount; a++){
                    memset(&event, 0, sizeof(event));
                    event.type = EV_KEY;
                    event.code = macro->actions[a].scan;
                    event.value = macro->actions[a].down;
                    if(write(kb->uinput, &event, sizeof(event)) <= 0)
                        printf("Write error: %s\n", strerror(errno));
                }
                event.type = EV_SYN;
                event.code = SYN_REPORT;
                event.value = 0;
                if(write(kb->uinput, &event, sizeof(event)) <= 0)
                    printf("Write error: %s\n", strerror(errno));
            }
        } else {
            macro->triggered = 0;
        }
    }
    // Don't do anything else if a macro was already triggered
    if(macrotrigger){
        memcpy(kb->previntinput, kb->intinput, N_KEYS / 8);
        return;
    }
    for(int byte = 0; byte < N_KEYS / 8; byte++){
        char oldb = kb->previntinput[byte], newb = kb->intinput[byte];
        if(oldb == newb)
            continue;
        for(int bit = 0; bit < 8; bit++){
            int keyindex = byte * 8 + bit;
            key* map = keymap + keyindex;
            int scancode = bind->base[keyindex];
            char mask = 1 << bit;
            char old = oldb & mask, new = newb & mask;
            // If the key state changed, send it to the uinput device
            if(old != new && scancode != 0){
                struct input_event event;
                memset(&event, 0, sizeof(event));
                event.type = EV_KEY;
                event.code = scancode;
                event.value = !!new;
                if(write(kb->uinput, &event, sizeof(event)) <= 0)
                    printf("Write error: %s\n", strerror(errno));
                // The volume wheel doesn't generate keyups, so create them automatically
                if(map->scan == KEY_VOLUMEUP || map->scan == KEY_VOLUMEDOWN){
                    kb->intinput[byte] &= ~mask;
                    event.type = EV_KEY;
                    event.code = scancode;
                    event.value = 0;
                    if(write(kb->uinput, &event, sizeof(event)) <= 0)
                        printf("Write error: %s\n", strerror(errno));
                }
                event.type = EV_SYN;
                event.code = SYN_REPORT;
                event.value = 0;
                if(write(kb->uinput, &event, sizeof(event)) <= 0)
                    printf("Write error: %s\n", strerror(errno));
            }
        }
    }
    memcpy(kb->previntinput, kb->intinput, N_KEYS / 8);
}

void updateindicators(usbdevice* kb, int force){
    // Read the indicator LEDs for this device and update them if necessary.
    if(!kb->handle)
        return;
    if(kb->event <= 0){
        if(force){
            kb->ileds = 0;
            libusb_control_transfer(kb->handle, 0x21, 0x09, 0x0200, 0, &kb->ileds, 1, 0);
        }
        return;
    }
    char leds[LED_CNT / 8] = { 0 };
    if(ioctl(kb->event, EVIOCGLED(sizeof(leds)), &leds)){
        char ileds = leds[0];
        if(ileds != kb->ileds || force){
            kb->ileds = ileds;
            libusb_control_transfer(kb->handle, 0x21, 0x09, 0x0200, 0, &ileds, 1, 0);
        }
    }
}

void initbind(keybind* bind){
    for(int i = 0; i < N_KEYS; i++)
        bind->base[i] = keymap[i].scan;
    bind->macros = malloc(32 * sizeof(keymacro));
    bind->macrocap = 32;
}

void closebind(keybind* bind){
    for(int i = 0; i < bind->macrocount; i++)
        free(bind->macros[i].actions);
    free(bind->macros);
    memset(bind, 0, sizeof(*bind));
}

void cmd_bind(usbdevice* kb, int keyindex, const char* to){
    // Find the key to bind to
    int tocode = 0;
    if(sscanf(to, "#x%ux", &tocode) != 1 && sscanf(to, "#%u", &tocode) == 1){
        kb->bind.base[keyindex] = tocode;
        return;
    }
    // If not numeric, look it up
    for(int i = 0; i < N_KEYS; i++){
        if(keymap[i].name && !strcmp(to, keymap[i].name)){
            kb->bind.base[keyindex] = keymap[i].scan;
            return;
        }
    }
}

void cmd_unbind(usbdevice* kb, int keyindex, const char* to){
    kb->bind.base[keyindex] = 0;
}

void cmd_rebind(usbdevice* kb, int keyindex, const char* to){
    kb->bind.base[keyindex] = keymap[keyindex].scan;
}

void cmd_macro(usbdevice* kb, const char* keys, const char* assignment){
    keybind* bind = &kb->bind;
    if(bind->macrocount >= MACRO_MAX)
        return;
    // Create a key macro
    keymacro macro;
    memset(&macro, 0, sizeof(macro));
    // Scan the left side for key names, separated by +
    int empty = 1;
    int left = strlen(keys), right = strlen(assignment);
    int position = 0, field = 0;
    char keyname[12];
    while(position < left && sscanf(keys + position, "%10[^+]%n", keyname, &field) == 1){
        int keycode;
        if((sscanf(keyname, "#%d", &keycode) && keycode >= 0 && keycode < N_KEYS)
                  || (sscanf(keyname, "#x%x", &keycode) && keycode >= 0 && keycode < N_KEYS)){
            // Set a key numerically
            macro.combo[keycode / 8] |= 1 << (keycode % 8);
            empty = 0;
        } else {
            // Find this key in the keymap
            for(unsigned i = 0; i < N_KEYS; i++){
                if(keymap[i].name && !strcmp(keyname, keymap[i].name)){
                    macro.combo[i / 8] |= 1 << (i % 8);
                    empty = 0;
                    break;
                }
            }
        }
        if(keys[position += field] == '+')
            position++;
    }
    if(empty)
        return;
    // Count the number of actions (comma separated)
    int count = 0;
    for(const char* c = assignment; *c != 0; c++){
        if(*c == ',')
            count++;
    }
    // Allocate a buffer for them
    macro.actions = malloc(sizeof(macroaction) * count);
    macro.actioncount = 0;
    // Scan the actions
    position = 0;
    field = 0;
    while(position < right && sscanf(assignment + position, "%11[^,]%n", keyname, &field) == 1){
        if(!strcmp(keyname, "clear"))
            break;
        int down = (keyname[0] == '+');
        if(down || keyname[0] == '-'){
            int keycode;
            if((sscanf(keyname + 1, "#%d", &keycode) && keycode >= 0 && keycode < N_KEYS)
                      || (sscanf(keyname + 1, "#x%x", &keycode) && keycode >= 0 && keycode < N_KEYS)){
                // Set a key numerically
                macro.actions[macro.actioncount].scan = keymap[keycode].scan;
                macro.actions[macro.actioncount].down = down;
                macro.actioncount++;
            } else {
                // Find this key in the keymap
                for(unsigned i = 0; i < N_KEYS; i++){
                    if(keymap[i].name && !strcmp(keyname + 1, keymap[i].name)){
                        macro.actions[macro.actioncount].scan = keymap[i].scan;
                        macro.actions[macro.actioncount].down = down;
                        macro.actioncount++;
                        break;
                    }
                }
            }
        }
        if(assignment[position += field] == ',')
            position++;
    }

    // See if there's already a macro with this trigger
    keymacro* macros = bind->macros;
    for(int i = 0; i < bind->macrocount; i++){
        if(!memcmp(macros[i].combo, macro.combo, N_KEYS / 8)){
            free(macros[i].actions);
            // If the new macro has no actions, erase the existing one
            if(!macro.actioncount){
                for(int j = i + 1; j < bind->macrocount; j++)
                    memcpy(macros + j - 1, macros + j, sizeof(keymacro));
                bind->macrocount--;
            } else
                // If there are actions, replace the existing with the new
                memcpy(macros + i, &macro, sizeof(keymacro));
            return;
        }
    }

    // Add the macro to the device settings if not empty
    if(macro.actioncount < 1)
        return;
    memcpy(bind->macros + (bind->macrocount++), &macro, sizeof(keymacro));
    if(bind->macrocount >= bind->macrocap)
        bind->macros = realloc(bind->macros, (bind->macrocap += 16) * sizeof(keymacro));
}

void cmd_macroclear(usbdevice* kb){
    keybind* bind = &kb->bind;
    for(int i = 0; i < bind->macrocount; i++)
        free(bind->macros[i].actions);
    bind->macrocount = 0;
}
