import java.util.Scanner;
import java.util.Arrays;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class LSSS {
    public static final Logger logger = LoggerFactory.getLogger(LSSS.class);

    public int[][] initMatrix(int height) {
        logger.warn("Only [FULL-BINARY-TREE] supported now.");
        int nodeNum = (int) Math.pow(2, height) - 1;
        String[] treeNodes = new String[nodeNum];

        try (Scanner scanner = new Scanner(System.in)) {
            logger.info("LSSS Tree Height [{}]", height);
            logger.info(
                    "Input the access tree nodes: "
                            + "'or' for or node, 'and' for and node, any value for leaf node:");
            int g_index = 0;
            for (int level = 0; level < height; level++) {
                int levelNum = (int) Math.pow(2, level);
                logger.info("Level[{}] (with {} nodes):", level, levelNum);
                if (level == height - 1) {
                    logger.info("Input the leaf nodes: ");
                }
                for (int i = 0; i < levelNum; i++) {
                    String cur = scanner.next();
                    if (level != (height - 1)) {
                        if (!cur.equals("or") && !cur.equals("and")) {
                            throw new IllegalArgumentException("Invalid input");
                        }
                    }
                    treeNodes[g_index] = cur;
                    g_index++;
                }
            }
        } catch (Exception e) {
            logger.error("Invalid input: {}", e.getMessage());
            System.exit(1);
        }

        logger.info("Full Binary TreeNodes: {}", Arrays.toString(treeNodes));

        String[] v = new String[nodeNum];
        Arrays.fill(v, "");
        v[0] = "1";
        int c = 1;

        int nonLeafNum = (int) Math.pow(2, height - 1) - 1;
        for (int i = 0; i < nonLeafNum; i++) {
            // or 节点，左右子树不变
            if (treeNodes[i].equals("or")) {
                v[2 * (i + 1) - 1] = v[2 * (i + 1) - 1].isEmpty() ? v[i] : "," + v[i];
                v[2 * (i + 1)] = v[2 * (i + 1)].isEmpty() ? v[i] : "," + v[i];
            }
            // and 节点， 填充 v 数到 c 位
            // 右子节点：v (length = c)|| 1
            // 左子节点：0 (length = c) || -1
            if (treeNodes[i].equals("and")) {
                StringBuilder sb = new StringBuilder(v[i]);
                for (int j = 0; j < c - v[i].length(); j++) {
                    sb.append(",0");
                }
                v[i] = sb.toString();
                v[2 * (i + 1)] = v[i] + ",1";

                String zero = ",0".repeat(c).substring(1);
                v[2 * (i + 1) - 1] = zero + ",-1";
                c = c + 1;
            }
        }

        String[] v_leaf = Arrays.copyOfRange(v, nonLeafNum, nodeNum);
        int[][] res = Utils.stringMatrixToInt(v_leaf, c);
        return res;
    }
}
