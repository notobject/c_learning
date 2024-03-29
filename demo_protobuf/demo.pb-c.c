/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: demo.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "demo.pb-c.h"
void   demo__demo__init
                     (Demo__DEMO         *message)
{
  static Demo__DEMO init_value = DEMO__DEMO__INIT;
  *message = init_value;
}
size_t demo__demo__get_packed_size
                     (const Demo__DEMO *message)
{
  assert(message->base.descriptor == &demo__demo__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t demo__demo__pack
                     (const Demo__DEMO *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &demo__demo__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t demo__demo__pack_to_buffer
                     (const Demo__DEMO *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &demo__demo__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Demo__DEMO *
       demo__demo__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Demo__DEMO *)
     protobuf_c_message_unpack (&demo__demo__descriptor,
                                allocator, len, data);
}
void   demo__demo__free_unpacked
                     (Demo__DEMO *message,
                      ProtobufCAllocator *allocator)
{
  assert(message->base.descriptor == &demo__demo__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor demo__demo__field_descriptors[4] =
{
  {
    "username",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Demo__DEMO, username),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "password",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Demo__DEMO, password),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "timestamp",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    offsetof(Demo__DEMO, timestamp),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "role",
    4,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_INT32,
    offsetof(Demo__DEMO, has_role),
    offsetof(Demo__DEMO, role),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned demo__demo__field_indices_by_name[] = {
  1,   /* field[1] = password */
  3,   /* field[3] = role */
  2,   /* field[2] = timestamp */
  0,   /* field[0] = username */
};
static const ProtobufCIntRange demo__demo__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 4 }
};
const ProtobufCMessageDescriptor demo__demo__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Demo.DEMO",
  "DEMO",
  "Demo__DEMO",
  "Demo",
  sizeof(Demo__DEMO),
  4,
  demo__demo__field_descriptors,
  demo__demo__field_indices_by_name,
  1,  demo__demo__number_ranges,
  (ProtobufCMessageInit) demo__demo__init,
  NULL,NULL,NULL    /* reserved[123] */
};
