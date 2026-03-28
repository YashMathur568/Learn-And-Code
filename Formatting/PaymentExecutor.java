package com.payment.processing;

public class PaymentExecutor {

    private final Logger logger;

    public PaymentExecutor(Logger logger) {
        this.logger = logger;
    }

    public void execute(PaymentRequest request) {
        logger.log("Executing payment of " + request.amount());
        // Simulate payment gateway call
        // In real scenario, this would call actual payment gateway
    }
}
