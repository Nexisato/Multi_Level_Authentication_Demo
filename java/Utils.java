import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.lang.String;

import com.google.common.io.BaseEncoding;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Utils {
    private static File _f_lib = new File(".\\lib");
    public static final Logger logger = LoggerFactory.getLogger(Utils.class);


    public static void main(String[] args) {
        byte[] bytes = {0x0F, 0x1A, 0x2B, 0x3C};
        String hexString = hexBytesToString(bytes);
        System.out.println(hexString); // 输出: 0F1A2B3C

    }


    public static void recordJarFiles() throws IOException {
        File[] files = _f_lib.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.endsWith(".jar");
            }
        });
        for (File file : files) {
            logger.debug(file.getName());
        }
    }

    public static long getDuration(long start, long end) {
        return end - start;
    }

    public static void printIntMatrix(int[][] matrix) {
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[i].length; j++) {
                System.out.print(matrix[i][j] + " ");
            }
            System.out.println();
        }
    }

    public static void printStringMatrix(String[] matrix) {
        for (int i = 0; i < matrix.length; i++) {
            System.out.println(matrix[i]);
        }
    }

    public static int[][] stringMatrixToInt(String[] v, int c) {
        int[][] res = new int[v.length][c];
        for (int i = 0; i < v.length; i++) {
            String[] splitLine = v[i].split(",");
            for (int j = 0; j < splitLine.length; j++) {
                res[i][j] = Integer.parseInt(splitLine[j]);
            }
        }
        return res;
    }

    public static int[] multiVector(int[] v, int[] w) {
        int[] res = new int[v.length];
        for (int i = 0; i < v.length; i++) {
            res[i] = v[i] * w[i];
        }
        return res;
    }

    public static int multiVectorSum(int[] v, int[] w) {
        int sum = 0;
        for (int i = 0; i < v.length; i++) {
            sum += v[i] * w[i];
        }
        return sum;
    }
    /**
     * 
     * @methods: Static Trasnfer methods
     */

    public static String hexBytesToString(byte[] bytes) {
		return BaseEncoding.base16().lowerCase().encode(bytes).toUpperCase();
	}

    // 16进制的字符串转换为byte[]数组
    public static byte[] hexStringToBytes(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    // 10进制数组转换为字符串
    public static String intToString(int[] arr) {
        StringBuilder sb = new StringBuilder();
        for (int i : arr) {
            sb.append(i);
        }
        return sb.toString();
    }

}
