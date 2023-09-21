
/* 别编辑，自动生成的代码
   生成命令是： gen_pb_rpc_code.py 目标C代码目录 目标Java代码目录 Java框架文件目录名

   semantic_api.generated.h，本文件包含了：
      1> host端请求/响应的语义层程序的总入口
      2> host端请求响应中，针对不同业务的各自的入口（其中.c中包含了一个 weak 版本的实现，使得整个程序在业务不全的时候也能编译和工作，但最终还是要有正规的C实现）
      3> MCU发送事件到host的各种evt的入口
    123都有一堆相似的代码，适合自动生成，可以节省人力并减少错误。宏也可以做到但是宏不容易调试

 */
#include "remote_message.pb.h"

// 来自host端的请求的总处理入口。在函数内会根据请求的名称派发到各自的服务函数
// 数据报层，收到报文，解出来to_mcu对象后，通过这个总入口调用各自的函数，之后会把from_mcu打包发回给host
// 返回值是各个子服务函数指定的，会打包在from_mcu后送回给host
remote_call_err_code remote_call_service(const to_mcu *to_mcu_obj, from_mcu *from_mcu_obj);


// 下面是请求/响应函数，针对不同的业务
remote_call_err_code remote_call_service_for_login(const login_req *login_req_obj, login_res *login_res_obj); //业务login的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_logout(const logout_req *logout_req_obj, logout_res *logout_res_obj); //业务logout的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_get_version_info(const get_version_info_req *get_version_info_req_obj, get_version_info_res *get_version_info_res_obj); //业务get_version_info的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_psam_slot(const psam_slot_req *psam_slot_req_obj, psam_slot_res *psam_slot_res_obj); //业务psam_slot的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_psam_reset(const psam_reset_req *psam_reset_req_obj, psam_reset_res *psam_reset_res_obj); //业务psam_reset的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_psam_apdu(const psam_apdu_req *psam_apdu_req_obj, psam_apdu_res *psam_apdu_res_obj); //业务psam_apdu的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_psam_close(const psam_close_req *psam_close_req_obj, psam_close_res *psam_close_res_obj); //业务psam_close的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_icc_init(const icc_init_req *icc_init_req_obj, icc_init_res *icc_init_res_obj); //业务icc_init的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_icc_detect(const icc_detect_req *icc_detect_req_obj, icc_detect_res *icc_detect_res_obj); //业务icc_detect的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_icc_reset(const icc_reset_req *icc_reset_req_obj, icc_reset_res *icc_reset_res_obj); //业务icc_reset的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_icc_apdu(const icc_apdu_req *icc_apdu_req_obj, icc_apdu_res *icc_apdu_res_obj); //业务icc_apdu的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_icc_close(const icc_close_req *icc_close_req_obj, icc_close_res *icc_close_res_obj); //业务icc_close的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_printer_check(const printer_check_req *printer_check_req_obj, printer_check_res *printer_check_res_obj); //业务printer_check的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_printer_parameter(const printer_parameter_req *printer_parameter_req_obj, printer_parameter_res *printer_parameter_res_obj); //业务printer_parameter的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_printer_content(const printer_content_req *printer_content_req_obj, printer_content_res *printer_content_res_obj); //业务printer_content的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_mksk_data_encdec(const mksk_data_encdec_req *mksk_data_encdec_req_obj, mksk_data_encdec_res *mksk_data_encdec_res_obj); //业务mksk_data_encdec的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_mksk_key_inject(const mksk_key_inject_req *mksk_key_inject_req_obj, mksk_key_inject_res *mksk_key_inject_res_obj); //业务mksk_key_inject的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_dukpt_inject_tik(const dukpt_inject_tik_req *dukpt_inject_tik_req_obj, dukpt_inject_tik_res *dukpt_inject_tik_res_obj); //业务dukpt_inject_tik的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_get_pinblock_encdata(const get_pinblock_encdata_req *get_pinblock_encdata_req_obj, get_pinblock_encdata_res *get_pinblock_encdata_res_obj); //业务get_pinblock_encdata的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_mksk_count_mac(const mksk_count_mac_req *mksk_count_mac_req_obj, mksk_count_mac_res *mksk_count_mac_res_obj); //业务mksk_count_mac的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_dukpt_count_mac(const dukpt_count_mac_req *dukpt_count_mac_req_obj, dukpt_count_mac_res *dukpt_count_mac_res_obj); //业务dukpt_count_mac的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_verify_plain_pin(const verify_plain_pin_req *verify_plain_pin_req_obj, verify_plain_pin_res *verify_plain_pin_res_obj); //业务verify_plain_pin的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_verify_cipher_pin(const verify_cipher_pin_req *verify_cipher_pin_req_obj, verify_cipher_pin_res *verify_cipher_pin_res_obj); //业务verify_cipher_pin的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_get_key_kcv(const get_key_kcv_req *get_key_kcv_req_obj, get_key_kcv_res *get_key_kcv_res_obj); //业务get_key_kcv的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_erase_all_key(const erase_all_key_req *erase_all_key_req_obj, erase_all_key_res *erase_all_key_res_obj); //业务erase_all_key的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_write_rsa_key(const write_rsa_key_req *write_rsa_key_req_obj, write_rsa_key_res *write_rsa_key_res_obj); //业务write_rsa_key的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_rsa_key_count(const rsa_key_count_req *rsa_key_count_req_obj, rsa_key_count_res *rsa_key_count_res_obj); //业务rsa_key_count的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_dukpt_encdec_data(const dukpt_encdec_data_req *dukpt_encdec_data_req_obj, dukpt_encdec_data_res *dukpt_encdec_data_res_obj); //业务dukpt_encdec_data的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_get_dukpt_ksn(const get_dukpt_ksn_req *get_dukpt_ksn_req_obj, get_dukpt_ksn_res *get_dukpt_ksn_res_obj); //业务get_dukpt_ksn的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_dukpt_ksn_increase(const dukpt_ksn_increase_req *dukpt_ksn_increase_req_obj, dukpt_ksn_increase_res *dukpt_ksn_increase_res_obj); //业务dukpt_ksn_increase的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_get_random_number(const get_random_number_req *get_random_number_req_obj, get_random_number_res *get_random_number_res_obj); //业务get_random_number的处理函数，被remote_call_service调用

bool send_remote_res(from_mcu *from_mcu_obj); // 本函数为所有事件函数提供服务

// 下面是事件函数
bool send_remote_event_for_key(const key_evt *key); // 发送事件到host的函数，供MCU代码调用。程序会打包一个from_mcu对象发送到host
bool send_remote_event_for_log(const log_evt *log); // 发送事件到host的函数，供MCU代码调用。程序会打包一个from_mcu对象发送到host
