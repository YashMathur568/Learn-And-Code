package com.payment.processing;

public class TransactionIdGenerator {

    private static final String TRANSACTION_PREFIX = "TXN-";

    public String generate() {
        return TRANSACTION_PREFIX + System.currentTimeMillis();
    }
}
