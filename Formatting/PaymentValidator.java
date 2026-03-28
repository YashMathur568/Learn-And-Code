package com.payment.processing;

import java.math.BigDecimal;

public class PaymentValidator {

    private static final BigDecimal MIN_AMOUNT = new BigDecimal("0.01");
    private static final BigDecimal MAX_ALLOWED_AMOUNT = new BigDecimal("5000");

    public void validate(PaymentRequest request) {
        validateCustomerId(request.customerId());
        validateAmount(request.amount());
    }

    private void validateCustomerId(String customerId) {
        if (customerId == null || customerId.isBlank()) {
            throw new IllegalArgumentException("Customer ID required");
        }
    }

    private void validateAmount(BigDecimal amount) {
        if (amount == null || amount.compareTo(MIN_AMOUNT) < 0) {
            throw new IllegalArgumentException("Invalid amount");
        }

        if (amount.compareTo(MAX_ALLOWED_AMOUNT) > 0) {
            throw new IllegalArgumentException("Limit exceeded");
        }
    }
}
