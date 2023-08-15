import remote_message_pb2


def is_a_dummy_type(type_name):
    try:
        type = remote_message_pb2.DESCRIPTOR.message_types_by_name[type_name]
        is_dummy = len(type.fields) == 1 and type.fields[0].name == 'dummy'
        return is_dummy
    except e:
        return False

def get_field_list(fields_inside_pb, type_name_filter):
    from_mcu_list = fields_inside_pb
    field_list = []
    for field in from_mcu_list:
        type_name, field_name, number = field.message_type.name, field.name, field.number
        is_matched = type_name_filter(type_name)
        if not is_matched:
            continue
        is_dummy = is_a_dummy_type(type_name)
        field_list.append((type_name, field_name, number, is_dummy))
    return field_list

evt_list = get_field_list(remote_message_pb2._FROM_MCU.oneofs_by_name['res'].fields, lambda type_name: type_name.endswith('_evt'))
res_list = get_field_list(remote_message_pb2._FROM_MCU.oneofs_by_name['res'].fields, lambda type_name: type_name.endswith('_res'))
req_list = get_field_list(remote_message_pb2._TO_MCU.oneofs_by_name['req'].fields, lambda type_name: type_name.endswith('_req'))
pass
