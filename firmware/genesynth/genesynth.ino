#include <util/delay.h>

#include "buttons.h"
#include "midi_interface.h"
#include "patch_loader.h"
#include "synth.h"
#include "ui.h"
#include "vgm.h"
#include "ym2612.h"

#define STATUS_LED 13
#define YM_CLOCK 4

static void setup_ym_clock() {
  /* Uses PWM to generate an 7.67Mhz clock for the YM2612 */
  pinMode(YM_CLOCK, OUTPUT);
  /*
    Ideal clock is 7.67, but it seems the teensy can't do that accurately.
    Use 7.5, which is accurate & stable (verified by external tuner) & more
    than close enough.
  */
  analogWriteFrequency(YM_CLOCK, 7500000);
  analogWrite(YM_CLOCK, 128);
}

// the setup routine runs once when you press reset:
void setup() {
  // Initialize the display & buttons.
  // thea::display::init();
  thea::buttons::init();

  // Wait for serial monitoring if the down button is pressed on boot.
  Serial.begin(9600);

  // Loop a few times to let the buttons settle, and if the down button is
  // pressed, wait for the serial debug monitor.
  for (auto i = 0; i < 100; i++) {
    thea::buttons::loop();
  }
  bool wait_for_serial = thea::buttons::is_pressed(2);

  // Initialize the user interface
  thea::ui::init(wait_for_serial);

  Serial.println("Early initialization done.");

  // Setup clocks
  setup_ym_clock();
  delay(50); // wait a moment for the clock.

  // Setup sound chips.
  thea::ym2612::setup();
  thea::ym2612::reset();
  thea::synth::init();

  // Setup patch loading. The MIDI interface will load the
  // initial patch.
  thea::patch_loader::init();

  // Setup MIDI
  thea::midi_interface::setup();

  Serial.println("Sound chips and MIDI interface up.");
}

void loop() {
  thea::buttons::loop();
  thea::midi_interface::loop();
  thea::ui::loop();
}