package com.example.protobufdemo.test;


import com.example.protobufdemo.Test;
import com.google.protobuf.InvalidProtocolBufferException;

public class TestProToBuf {
    public static void main(String[] args){
        test1();
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

    public static void test1(){
        int[] language = { 1, 2, 3, 4, 5 ,1, 2, 3, 4, 5};
        int[] maths = { 1, 2, 3, 4, 5 ,1, 2, 3, 4, 5};
        int[] Einglish = { 1, 2, 3, 4, 5 ,1, 2, 3, 4, 5};
        int[] num = new int[10];
        for(int i = 0;i < 10; i++){
            if(language[i] > 0 ){
                num[i] += language[i];
            }
            if(maths[i] > 0 ){
                num[i] += maths[i];
            }
            if(Einglish[i] > 0 ){
                num[i] += Einglish[i];
            }
            num[i] = num[i]/3;
            System.out.println("test: num = " +num[i]);
        }

        System.out.println("test: num = " +num.toString());

    }
}
