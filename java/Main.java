import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Main {
    public static final Logger logger = LoggerFactory.getLogger(Main.class);
    public static void main(String[] args) throws Exception {
        //Utils.recordJarFiles();
       
        // long start = System.currentTimeMillis();

        // byte[] bytes = {0x0F, 0x1A, 0x2B, 0x3C};
        // String hexString = Scheme.hexBytesToString(bytes);
        // System.out.println(hexString); // 输出: 0F1A2B3C

        // CPABE cpape = new CPABE();
        // cpape.init();

        // long end = System.currentTimeMillis();
        // System.out.println("Duration: " + Utils.getDuration(start, end) + "ms");

    
        
        LSSS lsss = new LSSS();
        int[][] accessMatrix = lsss.initMatrix(3);
        Utils.printIntMatrix(accessMatrix);
    }
    
}
