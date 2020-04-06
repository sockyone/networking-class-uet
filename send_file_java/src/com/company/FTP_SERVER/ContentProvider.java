package com.company.FTP_SERVER;

import java.io.*;
import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;

public class ContentProvider {

    private static class SingletonWrapper {
        static ContentProvider INSTANCE = new ContentProvider();
    }

    private ContentProvider() {}

    public static ContentProvider getInstance() {
        return SingletonWrapper.INSTANCE;
    }

    private int numOfDownloaded = 0;
    private int numOfUploaded = 0;

    private String rootFolder = "SharedFolder";
    private ConcurrentHashMap<String, FileStatus> fileStatus = new ConcurrentHashMap<>();

    //get list files
    public ArrayList<String> getListFile() {
        File rootFolder = new File(this.rootFolder);

        File[] listOfFiles = rootFolder.listFiles();

        ArrayList<String> fileNames = new ArrayList<>();

        if (listOfFiles == null) return fileNames;

        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile())
                fileNames.add(listOfFiles[i].getName());
        }

        return fileNames;
    }

    public synchronized void downloadPlusOne() {
        this.numOfDownloaded++;
    }

    public synchronized void uploadPlusOne() {
        this.numOfUploaded++;
    }

    public int getDownloadedNum() {
        return this.numOfDownloaded;
    }

    public int getUploadedNum() {
        return this.numOfUploaded;
    }

    public InputStream getInputStream(String fileName) {
        if (this.fileStatus.containsKey(fileName)) {
            if (this.fileStatus.get(fileName).requestForReading()) {
                try {
                    InputStream inputStream = new FileInputStream(new File(this.rootFolder + "/" + fileName));
                    return inputStream;
                } catch (IOException e) {
                    this.fileStatus.get(fileName).stopReading();
                    return null;
                }
            } else {
                return null;
            }
        } else {
            //if this file is not exist
            if (!new File(this.rootFolder + "/" + fileName).exists()) return null;

            //continue
            FileStatus fs = new FileStatus();
            this.fileStatus.put(fileName, new FileStatus());
            if (fs.requestForReading()) {
                try {
                    InputStream inputStream = new FileInputStream(new File(this.rootFolder + "/" + fileName));
                    return inputStream;
                } catch (IOException e) {
                    this.fileStatus.get(fileName).stopReading();
                    return null;
                }
            } else {
                return null;
            }
        }
    }

    public OutputStream getOutputStream(String fileName) {
        if (this.fileStatus.containsKey(fileName)) {
            if (this.fileStatus.get(fileName).requestForWriting()) {
                try {
                    OutputStream outputStream = new FileOutputStream(new File(this.rootFolder + "/" + fileName));
                    return outputStream;
                } catch (IOException e) {
                    this.fileStatus.get(fileName).stopWriting();
                    return null;
                }
            } else {
                return null;
            }
        } else {
            FileStatus fs = new FileStatus();
            this.fileStatus.put(fileName, fs);
            if (fs.requestForWriting()) {
                try {
                    OutputStream outputStream = new FileOutputStream(new File(this.rootFolder + "/" + fileName));
                    return outputStream;
                } catch (IOException e) {
                    this.fileStatus.get(fileName).stopWriting();
                    return null;
                }
            } else {
                return null;
            }
        }
    }

    public void stopReading(String fileName) {
        if (this.fileStatus.containsKey(fileName)) {
            this.fileStatus.get(fileName).stopReading();
        }
    }

    public void stopWriting(String fileName) {
        if (this.fileStatus.containsKey(fileName)) {
            this.fileStatus.get(fileName).stopWriting();
        }
    }

    public boolean removeFile(String fileName) {
        if (this.fileStatus.containsKey(fileName)) {
            if (this.fileStatus.get(fileName).isAvailable()) {
                if (new File(this.rootFolder + "/" + fileName).delete()) {
                    this.fileStatus.remove(fileName);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return new File(this.rootFolder + "/" + fileName).delete();
    }

    public long getFileSize(String fileName) {
        File file = new File(this.rootFolder + "/" + fileName);
        if (file.exists() && file.isFile()) {
            return file.length();
        }
        return 0;
    }


    private class FileStatus {

        private int readerCount = 0;
        private int writerCount = 0;

        public FileStatus() {

        }

        public synchronized boolean requestForReading() {
            if (this.writerCount == 0) {
                this.readerCount++;
                ContentProvider.getInstance().downloadPlusOne();
                return true;
            }
            return false;
        }

        public synchronized boolean requestForWriting() {
            if (this.readerCount == 0 && this.writerCount == 0) {
                this.writerCount = 1;
                ContentProvider.getInstance().uploadPlusOne();
                return true;
            }
            return false;
        }

        public synchronized void stopReading() {
            if (this.readerCount > 0) {
                this.readerCount--;
            }
        }

        public synchronized void stopWriting() {
            if (this.writerCount > 0) {
                this.writerCount = 0;
            }
        }

        public boolean isAvailable() {
            return this.writerCount == 0 && this.readerCount == 0;
        }
    }

}
