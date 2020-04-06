package com.company.FTP_UTILS;

public class FTPPacket {
    private int command = FTP.UNKNOWN;
    private String content = null;

    public FTPPacket(int COMMAND, String content) {
        this.command = COMMAND;
        this.content = content;
    }

    public FTPPacket(int COMMAND) {
        this.command = COMMAND;
        this.content = "";
    }

    public int getCommand() {
        return this.command;
    }

    public String getContent() {
        //prevent this package content safe from outside
        if (this.content == null) return null;
        return new String(this.content);
    }
}
