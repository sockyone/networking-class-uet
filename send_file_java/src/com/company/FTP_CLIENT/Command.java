package com.company.FTP_CLIENT;

import com.company.FTP_UTILS.FTP;

public class Command {


    private int command;
    private String payload = null;

    public Command(int COMMAND, String payload) {
        this.command = COMMAND;
        this.payload = payload;
    }

    public Command(int COMMAND) {
        this.command = COMMAND;
    }

    public int getCommand() {
        return this.command;
    }

    public String getPayload() {
        return this.payload;
    }
}
