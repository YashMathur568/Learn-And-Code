package com.payment.processing;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;

public class PaymentProcessor {

    private static final BigDecimal MIN_AMOUNT = new BigDecimal("0.01");
    private static final BigDecimal MAX_ALLOWED_AMOUNT = new BigDecimal("5000");

    private static final int MAX_RETRIES = 2;
    private static final String PAYMENT_SUCCESS = "Payment successful";
    private static final String PAYMENT_FAILED = "Payment failed";

    private final Logger logger;
    private final NotificationService notifier;
    private final Map<String, PaymentRecord> history;

    public PaymentProcessor(Logger logger, NotificationService notifier) {
        this.logger = logger;
        this.notifier = notifier;
        this.history = new HashMap<>();
    }

    public PaymentResult process(PaymentRequest request) {
        validate(request);

        int attempt = 0;

        while (attempt < MAX_RETRIES) {
            try {
                execute(request);
                record(request);
                notifySuccess(request);

                return new PaymentResult(
                        true,
                        PAYMENT_SUCCESS,
                        generateId()
                );
            } catch (PaymentException exception) {
                attempt++;
                logger.log("Retry attempt: " + attempt);
            }
        }

        return new PaymentResult(false, PAYMENT_FAILED, null);
    }

    private void validate(PaymentRequest request) {
        if (request.customerId() == null || request.customerId().isBlank()) {
            throw new IllegalArgumentException("Customer ID required");
        }

        if (request.amount() == null
                || request.amount().compareTo(MIN_AMOUNT) < 0) {
            throw new IllegalArgumentException("Invalid amount");
        }
    }

    private void execute(PaymentRequest request) {
        logger.log("Executing payment of " + request.amount());

        if (request.amount().compareTo(MAX_ALLOWED_AMOUNT) > 0) {
            throw new PaymentException("Limit exceeded");
        }
    }

    private void record(PaymentRequest request) {
        history.put(
                generateId(),
                new PaymentRecord(
                        request.customerId(),
                        request.amount(),
                        LocalDateTime.now()
                )
        );
    }

    private void notifySuccess(PaymentRequest request) {
        notifier.send(
                request.customerId(),
                "Payment of " + request.amount() + " processed"
        );
    }

    private String generateId() {
        return "TXN-" + System.currentTimeMillis();
    }
}
