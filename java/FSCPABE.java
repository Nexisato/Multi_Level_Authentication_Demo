import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.plaf.jpbc.pairing.a.TypeACurveGenerator;
import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.a1.TypeA1CurveGenerator;
import it.unisa.dia.gas.plaf.jpbc.util.ElementUtils;
import java.io.*;
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.security.SecureRandom;
import Jama.Matrix;
import java.util.Random;

public class FSCPABE extends Scheme {
    TypeA1CurveGenerator pg = null;
    PairingParameters typeA1Params = null;
    Pairing pairing = null;
    private BigInteger N = null;
    private Field G = null;
    private Field GT = null;
    private Field Zr = null;
    private Element generator = null;
    private Element g = null;
    private Element g3 = null;
    private Element alpha = null;
    private Element a = null;
    private Element L = null;
    private Element C1 = null;
    private Element K = null;
    private Element[] u = null;
    private Element[] U = null;
    private Element[] Cx = null;
    private Element[] Dx = null;
    private Element[] C0 = null;
    private Element[] Ki = null;
    private Element egg = null;

    public static void main(String[] args) {
        FSCPABE fscpabe = new FSCPABE();
        fscpabe.init(4, 32);

        String S[] = { "A", "B" };
        fscpabe.keygen(S);

        LSSS lsss = new LSSS();
        int tree[][] = lsss.initMatrix(3);
        String plaintext = "hello";
        fscpabe.encrypt(plaintext, tree.length, tree[0].length, tree);
        fscpabe.decrypt();

    }

    public void init(int attriNum, int params) {
        this.pg = new TypeA1CurveGenerator(3, params);
        this.typeA1Params = pg.generate();
        System.out.println(typeA1Params);
        // e
        this.pairing = PairingFactory.getPairing(typeA1Params);
        this.generator = this.pairing.getG1().newRandomElement().getImmutable();
        this.g = ElementUtils.getGenerator(this.pairing, this.generator, typeA1Params, 0, 3).getImmutable();
        // 私钥
        this.g3 = ElementUtils.getGenerator(this.pairing, this.generator, typeA1Params, 2, 3).getImmutable();

        // 生成G
        this.G = pairing.getG1();
        this.N = G.getOrder();
        this.Zr = pairing.getZr();
        // GxG->GT
        this.GT = pairing.getGT();
        this.alpha = this.Zr.newRandomElement().getImmutable();
        this.a = this.Zr.newRandomElement().getImmutable();
        // e(g,g)
        this.egg = pairing.pairing(g, g).getImmutable();
        // 为属性空间每个属性选取随机值
        this.u = new Element[attriNum];
        for (int i = 0; i < attriNum; i++) {
            u[i] = this.Zr.newRandomElement().getImmutable();
        }
        this.U = new Element[attriNum];
        for (int i = 0; i < attriNum; i++) {
            U[i] = this.g.powZn(u[i]).getImmutable();
        }

    }

    public void keygen(String[] S) {
        Element R = ElementUtils.getGenerator(this.pairing, this.generator,
                this.typeA1Params, 2, 3).getImmutable();
        Element R1 = ElementUtils.getGenerator(this.pairing, this.generator,
                this.typeA1Params, 2, 3).getImmutable();

        Element[] Ri = new Element[S.length];
        for (int i = 0; i < S.length; i++) {
            Ri[i] = ElementUtils.getGenerator(this.pairing, this.generator, this.typeA1Params, 2, 3).getImmutable();
        }

        Element t = Zr.newRandomElement().getImmutable();
        this.K = this.g.powZn(this.alpha).mul(this.g.powZn(this.a.mul(t))).mul(R).getImmutable();
        this.L = this.g.powZn(t).mul(R1).getImmutable();

        this.Ki = new Element[S.length];
        for (int i = 0; i < S.length; i++) {
            this.Ki[i] = this.U[i].powZn(t).mul(Ri[i]).getImmutable();
        }
    }

    public void encrypt(String plaintext, int line, int col, int[][] tree) {
        long seed_val = this.Zr.newRandomElement().toBigInteger().longValue();
        Random seed = new Random(seed_val);
        int[] secret = new int[col];
        for (int i = 0; i < col; ++i) {
            secret[i] = seed.nextInt();
        }
        int[] rand = new int[line];
        for (int j = 0; j < line; ++j) {
            rand[j] = seed.nextInt();
        }
        this.C1 = this.g.powZn(Zr.newElement(secret[0])).getImmutable();
        this.Cx = new Element[line];
        for (int i = 0; i < line; ++i) {
            Cx[i] = (this.g.powZn(this.a.mul(Utils.multiVectorSum(tree[i], secret))))
                    .mul(this.g.powZn(this.u[i].negate().mul(rand[i]))).getImmutable();
        }
        this.Dx = new Element[line];
        for (int i = 0; i < line; i++) {
            this.Dx[i] = this.g.duplicate().powZn(Zr.newElement(rand[i])).getImmutable();
        }

        byte[] plaintextBytes = plaintext.getBytes();
        Element[] plaintextToGt = new Element[plaintextBytes.length];
        for (int i = 0; i < plaintextBytes.length; i++) {
            plaintextToGt[i] = GT.newElement(plaintextBytes[i]).getImmutable();
        }
        this.C0 = new Element[plaintextToGt.length];
        Element C0pre = this.egg.powZn(this.alpha.mul(secret[0])).getImmutable();

        System.out.println("原始明文为 (长度 {})：" + plaintext.length() + " " + plaintext);
        System.out.println("加密密文为 (长度 {})：" + C0.length);
        for (int i = 0; i < C0.length; i++) {
            this.C0[i] = C0pre.mul(plaintextToGt[i]).getImmutable();
            System.out.println("C0: " + C0[i]);
        }
    }

    public void decrypt() {
        double[][] access = { { 0, 1 }, { -1, 1 }, { 0, 0 } };
        // double[][] access =
        // {{0,0,0,1},{0,-1,0,1},{0,1,0,0},{-1,0,0,1},{0,0,-1,0},{0,0,0,0},{0,0,0,0}};
        // double[][] access =
        // {{0,0,0,-1,0,0,0,1},{0,0,0,1,0,0,0,0},{0,-1,0,0,0,0,0,1},{0,1,0,1,0,-1,0,0},{0,0,0,0,0,1,0,1},{0,0,0,0,0,0,0,0,},{-1,0,0,0,0,0,0,0},{0,0,-1,0,0,0,0,0},{0,0,0,0,-1,0,0,0},{0,0,0,0,0,0,-1,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
        // double[][] access =
        // {{0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},{0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,1},{0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,-1,0,1,0,0,0,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,-1,0,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,1},{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

        double[] w = solveMartix(access);
        Element F = this.pairing.pairing(this.C1, this.K).getImmutable();
        Element E = this.GT.newOneElement().getImmutable();
        for (int i = 0; i < w.length; i++) {
            int j = (int) w[i];
            E = E.mul(this.pairing.pairing(this.Cx[i],
                    this.L).mul(this.pairing.pairing(this.Dx[i], this.Ki[i])))
                    .pow(BigInteger.valueOf(j)).getImmutable();
        }
        Element H = F.div(E).getImmutable();
        Element[] M = new Element[this.C0.length];
        for (int i = 0; i < this.C0.length; i++) {
            M[i] = this.C0[i].div(H).getImmutable();
        }
        BigInteger[] recovPlaintext = new BigInteger[M.length];
        int[] recovPlaintextFinal = new int[recovPlaintext.length];
        System.out.println("解密后的明文为：");
        for (int i = 0; i < M.length; i++) {
            recovPlaintext[i] = M[i].toBigInteger();
            recovPlaintextFinal[i] = recovPlaintext[i].intValue();
            System.out.print((char)recovPlaintextFinal[i]);
        }
        System.out.println();
    }

    public static double[] solveMartix(double[][] mat) {
        Matrix m = new Matrix(mat);
        double[] rho = new double[mat.length];
        rho[0] = 1;
        for (int i = 1; i < mat.length; i++) {
            rho[i] = 0;
        }
        Matrix r = new Matrix(rho, rho.length);
        Matrix w = m.solve(r);
        double[] res = w.getColumnPackedCopy();
        return res;
    }

}
