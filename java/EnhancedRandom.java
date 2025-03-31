import java.math.BigInteger;
import java.util.Random;
import java.security.SecureRandom;
import java.util.stream.DoubleStream;
import java.util.stream.IntStream;
import java.util.stream.LongStream;

public class EnhancedRandom {
	final private Random random;

	public EnhancedRandom() {
		this.random = new SecureRandom();
	}

	public EnhancedRandom(Random random) {
		this.random = random;
	}

	public BigInteger nextBigInteger(int exponent) {
		return nextBigInteger(BigInteger.valueOf(2).pow(exponent));
	}

	public BigInteger nextBigInteger(BigInteger until) {
		return nextBigInteger(BigInteger.ZERO, until);
	}

	public BigInteger nextBigIntegerBetweenPrimeBits(int lowBit, int highBit) {
		BigInteger from = this.nextPrimeBigInteger(lowBit);
		BigInteger until = this.nextPrimeBigInteger(highBit);
		return this.nextBigInteger(from, until);
	}

	public BigInteger nextBigIntegerBetweenBits(int lowBit, int highBit) {
		BigInteger from = BigInteger.valueOf(2).pow(lowBit);
		BigInteger until = BigInteger.valueOf(2).pow(highBit);
		return this.nextBigInteger(from, until);
	}

	public boolean isPrime(BigInteger number) {
		return number.isProbablePrime(20);
	}

	public BigInteger nextBigInteger(BigInteger from, BigInteger until) {
		if (from.compareTo(until) >= 0) {
			throw new IllegalArgumentException("until must be greater than from");
		}
		
		BigInteger randomNumber;
		int bitLength;
		if (from.bitLength() == until.bitLength()) {
			int fromBitLength = from.bitLength();
			bitLength = fromBitLength;
		} else {
			int fromBitLength = from.bitLength();
			int untilBitLength = until.bitLength();
			bitLength = this.nextInt(fromBitLength, untilBitLength);
		}

		do {
			randomNumber = new BigInteger(bitLength, random);
		} while (randomNumber.compareTo(from) < 0 || randomNumber.compareTo(until) >= 0);

		return randomNumber;
	}


	public BigInteger nextPrimeBigInteger(int bitLength) {
        return BigInteger.probablePrime(bitLength, random);
    }

	

	public int nextInt(int from, int until) {
		if (from >= until) {
            throw new IllegalArgumentException("until must be greater than from");
        }
		return this.random.nextInt(until - from) + from;
	}

	public void setSeed(long seed) {
		random.setSeed(seed);
	}

	public void nextBytes(byte[] bytes) {
		random.nextBytes(bytes);
	}

	public int nextInt() {
		return random.nextInt();
	}

	public int nextInt(int bound) {
		return random.nextInt(bound);
	}

	public long nextLong() {
		return random.nextLong();
	}

	public boolean nextBoolean() {
		return random.nextBoolean();
	}

	public float nextFloat() {
		return random.nextFloat();
	}

	public double nextDouble() {
		return random.nextDouble();
	}

	public double nextGaussian() {
		return random.nextGaussian();
	}

	public IntStream ints(long streamSize) {
		return random.ints(streamSize);
	}

	public IntStream ints() {
		return random.ints();
	}

	public IntStream ints(long streamSize, int randomNumberOrigin, int randomNumberBound) {
		return random.ints(streamSize, randomNumberOrigin, randomNumberBound);
	}

	public IntStream ints(int randomNumberOrigin, int randomNumberBound) {
		return random.ints(randomNumberOrigin, randomNumberBound);
	}

	public LongStream longs(long streamSize) {
		return random.longs(streamSize);
	}

	public LongStream longs() {
		return random.longs();
	}

	public LongStream longs(long streamSize, long randomNumberOrigin, long randomNumberBound) {
		return random.longs(streamSize, randomNumberOrigin, randomNumberBound);
	}

	public LongStream longs(long randomNumberOrigin, long randomNumberBound) {
		return random.longs(randomNumberOrigin, randomNumberBound);
	}

	public DoubleStream doubles(long streamSize) {
		return random.doubles(streamSize);
	}

	public DoubleStream doubles() {
		return random.doubles();
	}

	public DoubleStream doubles(long streamSize, double randomNumberOrigin, double randomNumberBound) {
		return random.doubles(streamSize, randomNumberOrigin, randomNumberBound);
	}

	public DoubleStream doubles(double randomNumberOrigin, double randomNumberBound) {
		return random.doubles(randomNumberOrigin, randomNumberBound);
	}
}
