package com.example.protobufdemo.test;


import com.example.protobufdemo.Test;
import com.google.protobuf.InvalidProtocolBufferException;

public class TestProToBuf {
    public static void main(String[] args){
        test();
    }

    public static void test(){
        Test.nfc_data.Builder build =  Test.nfc_data.newBuilder();
        build.setId(1);
        build.setOPEN("20191018105706");
        build.setRand("123456789");
        build.setColse("0987654321");
        Test.nfc_data info = build.build();
        byte[] nfc = info.toByteArray();
        System.out.println("test: nfc = " +nfc);
        try {
            info =  Test.nfc_data.parseFrom(nfc);
            System.out.println("test: info = " +info);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
    }
}
