package com.payment.processing;

public class PaymentProcessor {

    private static final int MAX_ATTEMPTS = 2;
    private static final String PAYMENT_SUCCESS = "Payment successful";
    private static final String PAYMENT_FAILED = "Payment failed";

    private final Logger logger;
    private final NotificationService notifier;
    private final PaymentValidator validator;
    private final PaymentExecutor executor;
    private final PaymentHistory history;
    private final TransactionIdGenerator idGenerator;

    public PaymentProcessor(Logger logger, NotificationService notifier) {
        this.logger = logger;
        this.notifier = notifier;
        this.validator = new PaymentValidator();
        this.executor = new PaymentExecutor(logger);
        this.history = new PaymentHistory();
        this.idGenerator = new TransactionIdGenerator();
    }

    public PaymentResult process(PaymentRequest request) {
        validator.validate(request);

        int attempt = 0;

        while (attempt < MAX_ATTEMPTS) {
            try {
                executor.execute(request);
                String transactionId = idGenerator.generate();
                history.record(transactionId, request);
                notifySuccess(request);

                return new PaymentResult(
                        true,
                        PAYMENT_SUCCESS,
                        transactionId
                );
            } catch (PaymentException exception) {
                attempt++;
                logger.log("Retry attempt: " + attempt);
            }
        }

        return new PaymentResult(false, PAYMENT_FAILED, null);
    }

    private void notifySuccess(PaymentRequest request) {
        notifier.send(
                request.customerId(),
                "Payment of " + request.amount() + " processed"
        );
    }
}
