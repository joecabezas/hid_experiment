#include <math.h>
#include <stdio.h>

#include "bsp/board_api.h"
#include "linear_job.h"
#include "message.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "pico/stdlib.h"
#include "schedule.h"
#include "tusb.h"
#include "usb_descriptors.h"


#define RADIUS 150
#define SPEED 0.02
#define OFFSET_X 960
#define OFFSET_Y 540

enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
  BLINK_CDC = 50,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
static Schedule schedule;

void led_blinking_task(void);
void hid_task(void);
void cdc_task(void);

void encode_message(NetworkCommandType type, NetworkPoint2D p0,
                    NetworkPoint2D p1, int32_t duration, uint8_t **buffer_out,
                    size_t *size_out);
void decode_message(uint8_t *buffer, size_t buffer_size, NetworkCommand **cmd,
                    bool *status);

int main(void) {
  board_init();
  stdio_init_all();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (1) {
    tud_task();
    led_blinking_task();

    hid_task();
    cdc_task();
  }
}

void cdc_task(void) {
  if (tud_cdc_connected()) {
    if (tud_cdc_available()) {
      // blink_interval_ms = BLINK_CDC;
      // Echo characters received over CDC
      uint8_t buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
  printf("mounted\n");
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  printf("unmounted\n");
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  printf("suspended\n");
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
  printf("resumed\n");
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// static void send_hid_report(uint8_t report_id, uint32_t btn) {
//   // skip if hid is not ready yet
//   if (!tud_hid_ready()) return;
//   if (report_id != REPORT_ID_MOUSE) return;
//   if (!btn) return;

//   uint32_t time_ms = board_millis();

//   // Calculate the angle based on the elapsed time
//   double angle = (double)time_ms * SPEED;

//   // Calculate X and Y deltas using sine and cosine for circular motion
//   double x = OFFSET_X + RADIUS * cos(angle);
//   double y = OFFSET_Y + RADIUS * sin(angle);

//   // scale
//   // least common multiple (LCM) of 1920 and 1080 is 17,280 = 0x4380
//   // LCM_1920_1080 = 17,280 (check hid_device.h)
//   // x = LCM_1920_1080 / 1920 = 9
//   // y = LCM_1920_1080 / 1080 = 16
//   x *= 9;
//   y *= 16;

//   tud_hid_abs_mouse_report(REPORT_ID_MOUSE, 0x00, (uint16_t)x, (uint16_t)y,
//                            0x80, 0x80);
// }

void hid_task(void) {
  uint32_t millis = board_millis();

  // // Poll every 10ms
  // const uint32_t interval_ms = 1;
  // static uint32_t start_ms = 0;

  // if (millis - start_ms < interval_ms) return;
  // start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  static uint32_t lastDebounceTime = 0;
  static uint32_t lastState = 0;
  static bool allowed = true;
  // printf("btn:%d\n", btn);
  // printf("lastState:%d\n", lastState);
  if (btn != lastState && !btn) {
    printf("debouncing reset\n");
    allowed = true;
    lastDebounceTime = millis;
  };
  lastState = btn;

  // printf("hid_task: current jobs: %d\n", schedule.queue.size());

  // Remote wakeup
  if (tud_suspended() && btn) {
    // printf("suspended and button");
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    // printf("else");
    // send_hid_report(REPORT_ID_MOUSE, btn);

    schedule.process();

    if (!btn) return;
    if ((millis - lastDebounceTime) < 50) return;
    if (!allowed) return;

    // simulate message reading by enconding a protobuffer message
    uint8_t *buffer = nullptr;
    size_t buffer_size = 0;
    encode_message(NetworkCommandType_LINEAR, {123, 456}, {789, 321}, 3000u,
                   &buffer, &buffer_size);

    // now decode it back
    NetworkCommand *cmd = new NetworkCommand;
    bool status;
    decode_message(buffer, buffer_size, &cmd, &status);

    printf("cmd: %d\n", cmd->cmd);
    if (cmd->cmd == NetworkCommandType_LINEAR) {
      Point2D p0 = {
          static_cast<int16_t>(cmd->args.pair.p0.x),
          static_cast<int16_t>(cmd->args.pair.p0.y),
      };
      Point2D p1 = {
          static_cast<int16_t>(cmd->args.pair.p1.x),
          static_cast<int16_t>(cmd->args.pair.p1.y),
      };
      std::unique_ptr<Job> job = std::make_unique<LinearJob>(p0, p1, 3000u);
      printf("scheduling job...\n");
      schedule.add_job(std::move(job));
    }

    // Free the allocated buffer
    free(buffer);
    free(cmd);

    allowed = false;
  }
}

void encode_message(NetworkCommandType type, NetworkPoint2D p0,
                    NetworkPoint2D p1, int32_t duration, uint8_t **buffer_out,
                    size_t *size_out) {
  NetworkCommand cmd = NetworkCommand_init_zero;
  cmd.cmd = type;
  cmd.duration = duration;
  NetworkPoint2DPair pair = {
      .has_p0 = true, .p0 = p0, .has_p1 = true, .p1 = p1};
  printf("Setting which_args to %d for pair\n", NetworkCommand_pair_tag);
  cmd.which_args = NetworkCommand_pair_tag;
  cmd.args.pair = pair;

  // Calculate the size of the encoded message
  bool status = pb_get_encoded_size(size_out, NetworkCommand_fields, &cmd);

  if (status) {
    printf("The size of the encoded message is: %zu bytes\n", *size_out);
  } else {
    printf("Failed to calculate encoded size\n");
  }

  // allocate a buffer of the right size and encode the message
  *buffer_out = (uint8_t *)malloc(*size_out);

  // Encode the message
  pb_ostream_t stream = pb_ostream_from_buffer(*buffer_out, *size_out);
  status = pb_encode(&stream, NetworkCommand_fields, &cmd);
  size_t encoded_length = stream.bytes_written;

  // Check if encoding was successful
  if (!status) {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
  } else {
    printf("Encoded message of length %zu bytes\n", encoded_length);
    printf(
        "Successfully encoded message: cmd = %d, duration = %d, which_args = "
        "%d (pair tag: %d)\n",
        cmd.cmd, cmd.duration, cmd.which_args, NetworkCommand_pair_tag);
  }
}

// Function to decode a protobuf message
void decode_message(uint8_t *buffer, size_t buffer_size, NetworkCommand **cmd,
                    bool *status) {
  // Create an instance of the message structure
  **cmd = NetworkCommand_init_zero;

  // Create a stream for reading from the buffer
  pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_size);

  // Decode the message
  *status = pb_decode(&stream, NetworkCommand_fields, *cmd);

  // Check if decoding was successful
  if (!*status) {
    printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    return;
  }

  // Use a local pointer to dereference **cmd just once
  NetworkCommand *decoded_cmd = *cmd;

  // Process the decoded message
  printf("Decoded command: %d\n", decoded_cmd->cmd);
  printf("Duration: %d\n", decoded_cmd->duration);

  // Check which argument was used
  printf("which_args: %d (expected %d for pair)\n", decoded_cmd->which_args,
         NetworkCommand_pair_tag);
  if (decoded_cmd->which_args == NetworkCommand_pair_tag) {
    printf("Pair coordinates: (%d, %d) to (%d, %d)\n",
           decoded_cmd->args.pair.p0.x, decoded_cmd->args.pair.p0.y,
           decoded_cmd->args.pair.p1.x, decoded_cmd->args.pair.p1.y);
  } else {
    printf("No valid arguments found\n");
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
// void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
//                                 uint16_t len) {
//   (void)instance;
//   (void)len;
// }

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;

  if (report_type != HID_REPORT_TYPE_OUTPUT) return;
  if (report_id != REPORT_ID_KEYBOARD) return;
  if (bufsize < 1) return;

  uint8_t const kbd_leds = buffer[0];

  if (kbd_leds & KEYBOARD_LED_NUMLOCK) {
    // Capslock On: disable blink, turn led on
    blink_interval_ms = 0;
    board_led_write(true);
  } else {
    // Caplocks Off: back to normal blink
    board_led_write(false);
    blink_interval_ms = BLINK_MOUNTED;
  }
}

void led_blinking_task(void) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms) return;  // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state;  // toggle
}
