#include "network.h"

#include <cstdio>
#include <cstdlib>

#include "pico/stdlib.h"

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