/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_ADC_0.c
 * Purpose: USB Device Audio Device Class (ADC) User module
 * Rev.:    V6.2
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_adcFunctions
 *
 */
 
 
//! [code_USBD_User_ADC]
 
#include <stdint.h>
#include <stdbool.h>
#include "rl_usb.h"
 
extern unsigned int sound_label;
extern unsigned int mute_req;
 
// Called during USBD_Initialize to initialize the USB ADC class Device.
void USBD_ADC0_Initialize (void) {
  USBD_ADC_SpeakerSetVolumeRange(0,0,0,256,4,128);
  sound_label=128;
	mute_req=false;
}
 
// Called during USBD_Uninitialize to un-initialize the USB ADC class Device.
void USBD_ADC0_Uninitialize (void) {
  // Add code for de-initialization
}
 
// Callback function called when speaker activity (interface) setting changed event.
// \param[in]     active        activity status.
void USBD_ADC0_SpeakerStatusEvent (bool active) {
  // Add code for speaker activity setting change
}
 
// Callback function called when speaker mute setting changed event.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left speaker (in stereo mode)
//                                - value 2: right speaker (in stereo mode)
// \param[in]     cur           current setting.
void USBD_ADC0_SpeakerMuteEvent (uint8_t ch, bool cur) {
  // Add code for speaker mute setting change
  mute_req=cur;
}
 
// Callback function called when speaker volume setting changed event.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left speaker (in stereo mode)
//                                - value 2: right speaker (in stereo mode)
// \param[in]     cur           current setting.
void USBD_ADC0_SpeakerVolumeEvent (uint8_t ch, uint16_t cur) {
  sound_label=cur;
	// Add code for speaker volume setting change
}
 
// Callback function called when microphone activity (interface) setting changed event.
// \param[in]     active        activity status.
void USBD_ADC0_MicrophoneStatusEvent (bool active) {
  // Add code for microphone activity setting change
}
 
// Callback function called when microphone mute setting changed event.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left microphone (in stereo mode)
//                                - value 2: right microphone (in stereo mode)
// \param[in]     cur           current setting.
void USBD_ADC0_MicrophoneMuteEvent (uint8_t ch, bool cur) {
  // Add code for microphone mute setting change
}
 
// Callback function called when microphone volume setting changed event.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left microphone (in stereo mode)
//                                - value 2: right microphone (in stereo mode)
// \param[in]     cur           current setting.
void USBD_ADC0_MicrophoneVolumeEvent (uint8_t ch, uint16_t cur) {
  // Add code for microphone volume setting change
}
 
//! [code_USBD_User_ADC]
