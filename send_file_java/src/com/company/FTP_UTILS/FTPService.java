package com.company.FTP_UTILS;

import java.util.concurrent.ConcurrentHashMap;

public final class FTPService {

    private static class SingletonWrapper {
        static FTPService INSTANCE = new FTPService();
    }


    private ConcurrentHashMap<Integer, StreamStorage> waitingList = new ConcurrentHashMap<>();

    private FTPService() {
        //do nothing
    }

    public static FTPService getInstance() {
        return SingletonWrapper.INSTANCE;
    }

    public StreamStorage getFromWaitingList(int hashCode) {
        if (this.waitingList.containsKey(hashCode)) {
            StreamStorage result = this.waitingList.get(hashCode);

            //remove from list
            this.waitingList.remove(hashCode);

            return result;
        }
        return null;
    }

    private int createRandomNumber() {
        //from 0 -> 10^9 - 1
        return (int)(Math.random() * 1000000000);
    }

    public int putIntoWaitingList(StreamStorage streamStorage) {
        int hashKey = this.createRandomNumber();

        while (this.waitingList.containsKey(hashKey)) {
            //try to create new
            hashKey = this.createRandomNumber();
        }

        this.waitingList.put(hashKey, streamStorage);

        return hashKey;
    }

}


//control: get filename; -> REQUEST filename -> 565
//trans: request tai 555