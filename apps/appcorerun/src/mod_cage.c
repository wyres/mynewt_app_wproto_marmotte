/**
 * Copyright 2019 Wyres
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, 
 * software distributed under the License is distributed on 
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
 * either express or implied. See the License for the specific 
 * language governing permissions and limitations under the License.
*/
/**
 * Cage door and test button handling app module
 */

#include "os/os.h"
#include "bsp/bsp.h"

#include "wyres-generic/wutils.h"
#include "wyres-generic/timemgr.h"
#include "wyres-generic/rebootmgr.h"
#include "wyres-generic/movementmgr.h"
#include "wyres-generic/sensormgr.h"

#include "app-core/app_core.h"
#include "app-core/app_msg.h"

#define USER_BUTTON  ((int8_t)MYNEWT_VAL(BUTTON_IO))
#define DOOR_CONTACT  ((int8_t)MYNEWT_VAL(DOOR_IO))

// COntext data
static struct appctx {
    uint32_t lastButtonReleaseTS;
    uint32_t lastDoorOpenedTS;
    uint32_t lastDoorClosedTS;
} _ctx;
static void buttonChangeCB(void* ctx, SR_BUTTON_STATE_t currentState, SR_BUTTON_PRESS_TYPE_t currentPressType);
static void doorChangeCB(void* ctx, SR_BUTTON_STATE_t currentState, SR_BUTTON_PRESS_TYPE_t currentPressType);

// My api functions
static uint32_t start() {
    log_debug("MP:start cage check");
    log_debug("MP:for 1s");
    return 1*1000;
}

static void stop() {
    log_debug("MP:done");
}
static void off() {
    // ensure sensors are low power mode
}
static void deepsleep() {
    // ensure sensors are off
}
static bool getData(APP_CORE_UL_t* ul) {
    log_info("MP: UL cage last button @%d, opened @%d, closed@%d", _ctx.lastButtonReleaseTS, _ctx.lastDoorOpenedTS, _ctx.lastDoorClosedTS);
    // TODO write to UL TS and current states

    return false;       // nothing vital here
}

static APP_CORE_API_t _api = {
    .startCB = &start,
    .stopCB = &stop,
    .offCB = &off,
    .deepsleepCB = &deepsleep,
    .getULDataCB = &getData,
    .ticCB = NULL,    
};
// Initialise module
void mod_cage_init(void) {
    // hook app-core for env data
    AppCore_registerModule("CAGE", APP_MOD_PTI, &_api, EXEC_PARALLEL);

    if (USER_BUTTON>=0) {
        // add cb for button press, no context required
        SRMgr_registerButtonCB(USER_BUTTON, buttonChangeCB, NULL);
    }
    if (DOOR_CONTACT>=0) {
        // using extio as input irq
        SRMgr_registerButtonCB(DOOR_CONTACT, doorChangeCB, NULL);
    }
//    log_debug("MP:initialised");
}

// internals
// callback each time button changes state
static void buttonChangeCB(void* ctx, SR_BUTTON_STATE_t currentState, SR_BUTTON_PRESS_TYPE_t currentPressType) {
    if (currentState==SR_BUTTON_RELEASED) {
        // note using log_noout as button shares GPIO with debug log uart...
        log_noout("MP:button released, duration %d ms, press type:%d", 
            (SRMgr_getLastButtonReleaseTS(USER_BUTTON)-SRMgr_getLastButtonPressTS(USER_BUTTON)),
            SRMgr_getLastButtonPressType(USER_BUTTON));
        _ctx.lastButtonReleaseTS = SRMgr_getLastButtonReleaseTS(USER_BUTTON);
        // ask for immediate UL with only us consulted
        AppCore_forceUL(APP_MOD_PTI);
    } else {
        log_noout("MP:button pressed");
    }
}
// callback each time extio changes state
static void doorChangeCB(void* ctx, SR_BUTTON_STATE_t currentState, SR_BUTTON_PRESS_TYPE_t currentPressType) {
    if (currentState==SR_BUTTON_RELEASED) {
        // note using log_noout as button shares GPIO with debug log uart...
        log_noout("MP:door opened");
        _ctx.lastDoorOpenedTS = TMMgr_getRelTimeMS();
        // ask for immediate UL with only us consulted
        AppCore_forceUL(APP_MOD_PTI);
    } else {
        log_noout("MP:door closeded");
        _ctx.lastDoorClosedTS = TMMgr_getRelTimeMS();
        // ask for immediate UL with only us consulted
        AppCore_forceUL(APP_MOD_PTI);
    }
}