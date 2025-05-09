package org.snore;

public class SnoreLib {

    static {
        System.loadLibrary("snore-lib");
    }

    public static native void init(String protoPath, String modelPath, int typeNumber);

    public static native void process(Result result, byte[] input);

    public static native void release();

}
