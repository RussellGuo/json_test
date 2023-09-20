package com.huaqin.posservices.remotemessage;

import android.os.RemoteCallbackList;
import android.util.Log;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.IPosCallback;
import com.huaqin.posservices.PosService;
import com.huaqin.posservices.PosServiceManagerService;
import com.huaqin.posservices.RemoteMessage;
import com.huaqin.posservices.SerialPort;


public class RemoteMessageApi {
    private static String TAG = "RemoteMessageApi";

    /**
     * Login
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.login_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setLogin(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * Logout
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.logout_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setLogout(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * GetVersionInfo
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.get_version_info_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setGetVersionInfo(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PsamSlot
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.psam_slot_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPsamSlot(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PsamReset
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.psam_reset_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPsamReset(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PsamApdu
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.psam_apdu_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPsamApdu(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PsamClose
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.psam_close_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPsamClose(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * IccInit
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.icc_init_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setIccInit(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * IccDetect
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.icc_detect_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setIccDetect(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * IccReset
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.icc_reset_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setIccReset(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * IccApdu
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.icc_apdu_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setIccApdu(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * IccClose
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.icc_close_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setIccClose(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PrinterCheck
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.printer_check_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPrinterCheck(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PrinterParameter
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.printer_parameter_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPrinterParameter(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * PrinterContent
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.printer_content_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setPrinterContent(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * MkskDataEncdec
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.mksk_data_encdec_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setMkskDataEncdec(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * MkskKeyInject
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.mksk_key_inject_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setMkskKeyInject(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * DukptInjectTik
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.dukpt_inject_tik_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setDukptInjectTik(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * GetPinblockEncdata
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.get_pinblock_encdata_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setGetPinblockEncdata(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * MkskCountMac
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.mksk_count_mac_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setMkskCountMac(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * DukptCountMac
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.dukpt_count_mac_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setDukptCountMac(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * VerifyPlainPin
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.verify_plain_pin_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setVerifyPlainPin(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * VerifyCipherPin
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.verify_cipher_pin_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setVerifyCipherPin(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * GetKeyKcv
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.get_key_kcv_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setGetKeyKcv(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * EraseAllKey
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.erase_all_key_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setEraseAllKey(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * WriteRsaKey
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.write_rsa_key_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setWriteRsaKey(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * RsaKeyCount
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.rsa_key_count_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setRsaKeyCount(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * DukptEncdecData
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.dukpt_encdec_data_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setDukptEncdecData(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * GetDukptKsn
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.get_dukpt_ksn_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setGetDukptKsn(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * DukptKsnIncrease
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.dukpt_ksn_increase_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setDukptKsnIncrease(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * GetRandomNumber
     * @param req
     * @return
     */
    public static Boolean remoteCallService(RemoteMessage.get_random_number_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setGetRandomNumber(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte, toMcuByte.length);
    }

    /**
     * 接收mcu报文
     * @param obj
     * @return
     */
    public static void remoteCallbackService(byte[] obj){
        RemoteMessage.from_mcu forMcu = forMcuUnpack(obj);
        RemoteCallbackList<IPosCallback> mCallbacks = PosServiceManagerService.init().getPosService().getmCallbacks();
        final int N = mCallbacks.beginBroadcast();
        for (int i = 0; i < N; i++) {
            if(forMcu != null){
                try {
                Log.d(TAG,"forMcu.getResCase() = " + forMcu.getResCase());
                switch (forMcu.getResCase()){
                    case LOGIN:
                        mCallbacks.getBroadcastItem(i).onLoginRes(obj);
                        break;
                    case LOGOUT:
                        mCallbacks.getBroadcastItem(i).onLogoutRes(obj);
                        break;
                    case GET_VERSION_INFO:
                        mCallbacks.getBroadcastItem(i).onGetVersionInfoRes(obj);
                        break;
                    case PSAM_SLOT:
                        mCallbacks.getBroadcastItem(i).onPsamSlotRes(obj);
                        break;
                    case PSAM_RESET:
                        mCallbacks.getBroadcastItem(i).onPsamResetRes(obj);
                        break;
                    case PSAM_APDU:
                        mCallbacks.getBroadcastItem(i).onPsamApduRes(obj);
                        break;
                    case PSAM_CLOSE:
                        mCallbacks.getBroadcastItem(i).onPsamCloseRes(obj);
                        break;
                    case ICC_INIT:
                        mCallbacks.getBroadcastItem(i).onIccInitRes(obj);
                        break;
                    case ICC_DETECT:
                        mCallbacks.getBroadcastItem(i).onIccDetectRes(obj);
                        break;
                    case ICC_RESET:
                        mCallbacks.getBroadcastItem(i).onIccResetRes(obj);
                        break;
                    case ICC_APDU:
                        mCallbacks.getBroadcastItem(i).onIccApduRes(obj);
                        break;
                    case ICC_CLOSE:
                        mCallbacks.getBroadcastItem(i).onIccCloseRes(obj);
                        break;
                    case PRINTER_CHECK:
                        mCallbacks.getBroadcastItem(i).onPrinterCheckRes(obj);
                        break;
                    case PRINTER_PARAMETER:
                        mCallbacks.getBroadcastItem(i).onPrinterParameterRes(obj);
                        break;
                    case PRINTER_CONTENT:
                        mCallbacks.getBroadcastItem(i).onPrinterContentRes(obj);
                        break;
                    case MKSK_DATA_ENCDEC:
                        mCallbacks.getBroadcastItem(i).onMkskDataEncdecRes(obj);
                        break;
                    case MKSK_KEY_INJECT:
                        mCallbacks.getBroadcastItem(i).onMkskKeyInjectRes(obj);
                        break;
                    case DUKPT_INJECT_TIK:
                        mCallbacks.getBroadcastItem(i).onDukptInjectTikRes(obj);
                        break;
                    case GET_PINBLOCK_ENCDATA:
                        mCallbacks.getBroadcastItem(i).onGetPinblockEncdataRes(obj);
                        break;
                    case MKSK_COUNT_MAC:
                        mCallbacks.getBroadcastItem(i).onMkskCountMacRes(obj);
                        break;
                    case DUKPT_COUNT_MA:
                        mCallbacks.getBroadcastItem(i).onDukptCountMaRes(obj);
                        break;
                    case VERIFY_PLAIN_PIN:
                        mCallbacks.getBroadcastItem(i).onVerifyPlainPinRes(obj);
                        break;
                    case VERIFY_CIPHER_PIN:
                        mCallbacks.getBroadcastItem(i).onVerifyCipherPinRes(obj);
                        break;
                    case GET_KEY_KCV:
                        mCallbacks.getBroadcastItem(i).onGetKeyKcvRes(obj);
                        break;
                    case ERASE_ALL_KEY:
                        mCallbacks.getBroadcastItem(i).onEraseAllKeyRes(obj);
                        break;
                    case WRITE_RSA_KEY:
                        mCallbacks.getBroadcastItem(i).onWriteRsaKeyRes(obj);
                        break;
                    case RSA_KEY_COUNT:
                        mCallbacks.getBroadcastItem(i).onRsaKeyCountRes(obj);
                        break;
                    case DUKPT_ENCDEC_DATA:
                        mCallbacks.getBroadcastItem(i).onDukptEncdecDataRes(obj);
                        break;
                    case GET_DUKPT_KSN:
                        mCallbacks.getBroadcastItem(i).onGetDukptKsnRes(obj);
                        break;
                    case DUKPT_KSN_INCREASE:
                        mCallbacks.getBroadcastItem(i).onDukptKsnIncreaseRes(obj);
                        break;
                    case GET_RANDOM_NUMBER:
                        mCallbacks.getBroadcastItem(i).onGetRandomNumberRes(obj);
                        break;
                    case KEY:
                        mCallbacks.getBroadcastItem(i).onKeyRes(obj);
                        break;
                    case LOG:
                        mCallbacks.getBroadcastItem(i).onLogRes(obj);
                        break;
                    }
                } catch (Exception e){
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * 解码mcu报文
     * @param obj
     * @return
     */
    private static RemoteMessage.from_mcu forMcuUnpack(byte[] obj) {
        RemoteMessage.from_mcu forMcu;
        try {
            forMcu = RemoteMessage.from_mcu.parseFrom(obj);
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }
        return forMcu;
    }


}
