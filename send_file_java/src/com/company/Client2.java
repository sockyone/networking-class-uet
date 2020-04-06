package com.company;

import com.company.FTP_CLIENT.FTPClient;


public class Client2 {

    public static void main(String[] args) {
        FTPClient client = null;
        try {
            client = new FTPClient("127.0.0.1", 3000, 3001);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }
        client.run();
    }
}
