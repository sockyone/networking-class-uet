package com.company.FTP_CLIENT;

import com.company.FTP_UTILS.FTP;
import com.company.FTP_UTILS.FTPPacket;

import java.util.ArrayList;

public final class CommandSolver {
    public static final Command commandSolve(String string) {
        String[] list = string.split("\\s+");
        if (list.length > 2) {
            return new Command(FTP.UNKNOWN, "Too many argument");
        }
        if (list.length < 1) {
            return new Command(FTP.UNKNOWN, "Too few argument");
        }
        if (list[0].toLowerCase().equals("get")) {
            if (list.length < 2) {
                return new Command(FTP.UNKNOWN, "Too few argument");
            }
            return new Command(FTP.GET, list[1]);
        } else if (list[0].toLowerCase().equals("put")) {
            if (list.length < 2) {
                return new Command(FTP.UNKNOWN, "Too few argument");
            }
            return new Command(FTP.PUT, list[1]);
        } else if (list[0].toLowerCase().equals("list")) {
            if (list.length == 2) {
                return new Command(FTP.UNKNOWN, "Too many argument");
            }
            return new Command(FTP.LIST);
        } else if (list[0].toLowerCase().equals("remove")) {
            if (list.length < 2) {
                return new Command(FTP.UNKNOWN, "Too few argument");
            }
            return new Command(FTP.REMOVE, list[1]);
        } else if (list[0].toLowerCase().equals("ls")) {
            if (list.length == 2) {
                return new Command(FTP.UNKNOWN, "Too many argument");
            }
            return new Command(FTP.LS);
        } else if (list[0].toLowerCase().equals("rm")) {
            if (list.length < 2) {
                return new Command(FTP.UNKNOWN, "Too few argument");
            }
            return new Command(FTP.RM, list[1]);
        }
        return new Command(FTP.UNKNOWN, "Your command is invalid");
    }

    public static final FTPPacket commandToPacket(Command command) {
        if (command.getCommand() == FTP.UNKNOWN || command.getPayload() == null) {
            return new FTPPacket(command.getCommand());
        }
        return new FTPPacket(command.getCommand(), command.getPayload());
    }
}
