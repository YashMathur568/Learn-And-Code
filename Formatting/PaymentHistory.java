package com.payment.processing;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;

public class PaymentHistory {

    private final Map<String, PaymentRecord> history;

    public PaymentHistory() {
        this.history = new HashMap<>();
    }

    public void record(String transactionId, PaymentRequest request) {
        history.put(
                transactionId,
                new PaymentRecord(
                        request.customerId(),
                        request.amount(),
                        LocalDateTime.now()
                )
        );
    }

    public PaymentRecord getRecord(String transactionId) {
        return history.get(transactionId);
    }

    public Map<String, PaymentRecord> getAllRecords() {
        return new HashMap<>(history);
    }
}
