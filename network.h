#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "message.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

void encode_message(NetworkCommandType type, NetworkPoint2D p0,
                    NetworkPoint2D p1, int32_t duration, uint8_t **buffer_out,
                    size_t *size_out);
void decode_message(uint8_t *buffer, size_t buffer_size, NetworkCommand **cmd,
                    bool *status);

#endif /* _NETWORK_H_ */