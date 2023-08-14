import remote_message_pb2

a = remote_message_pb2.DESCRIPTOR.message_types_by_name.keys()
print(a)
