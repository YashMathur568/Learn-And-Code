#include <iostream>
#include <memory>
#include <string>

#include "StepResult.h"
#include "GenerateStep.h"
#include "SummarizeStep.h"
#include "TranslateStep.h"
#include "RefineToneStep.h"
#include "AnalyzeStep.h"
#include "EnrichContextStep.h"
#include "FallbackStep.h"
#include "RetryStep.h"
#include "ConditionalStep.h"
#include "Workflow.h"
#include "WorkflowLogger.h"

void demonstrateProductDescriptionWorkflow(std::shared_ptr<ILogger> logger)
{
    std::cout << "\n*** DEMO 1: Product Description Workflow ***" << std::endl;
    std::cout << "Pipeline: Generate -> Refine Tone -> Summarize -> Translate\n" << std::endl;

    Workflow productWorkflow("Product Description Pipeline", logger);
    productWorkflow.addStep(std::make_shared<GenerateStep>("gpt-4"));
    productWorkflow.addStep(std::make_shared<RefineToneStep>("marketing"));
    productWorkflow.addStep(std::make_shared<SummarizeStep>(150));
    productWorkflow.addStep(std::make_shared<TranslateStep>("French"));

    StepResult result = productWorkflow.run("Wireless Bluetooth Headphones with Noise Cancellation");

    std::cout << "\nFinal Output: " << result.getOutput() << std::endl;
}

void demonstrateDataAnalysisWorkflow(std::shared_ptr<ILogger> logger)
{
    std::cout << "\n*** DEMO 2: Data Analysis Workflow ***" << std::endl;
    std::cout << "Pipeline: Analyze CSV -> Generate Insights -> Summarize Report\n" << std::endl;

    Workflow analysisWorkflow("Data Analysis Pipeline", logger);
    analysisWorkflow.addStep(std::make_shared<AnalyzeStep>("csv-statistical"));
    analysisWorkflow.addStep(std::make_shared<GenerateStep>("gpt-4-turbo"));
    analysisWorkflow.addStep(std::make_shared<SummarizeStep>(300));

    StepResult result = analysisWorkflow.run("sales_data_q1_2026.csv");

    std::cout << "\nFinal Output: " << result.getOutput() << std::endl;
}

void demonstrateConditionalTranslationWorkflow(std::shared_ptr<ILogger> logger)
{
    std::cout << "\n*** DEMO 3: Conditional Translation Workflow ***" << std::endl;
    std::cout << "Pipeline: Generate -> Summarize -> Translate (only if input > 20 chars)\n" << std::endl;

    auto translateIfLong = std::make_shared<ConditionalStep>(
        std::make_shared<TranslateStep>("German"),
        [](const std::string &input) -> bool
        {
            return input.length() > 20;
        });

    Workflow conditionalWorkflow("Conditional Translation Pipeline", logger);
    conditionalWorkflow.addStep(std::make_shared<GenerateStep>("claude-3"));
    conditionalWorkflow.addStep(std::make_shared<SummarizeStep>());
    conditionalWorkflow.addStep(translateIfLong);

    StepResult result = conditionalWorkflow.run("Explain quantum computing in simple terms");

    std::cout << "\nFinal Output: " << result.getOutput() << std::endl;
}

void demonstrateRetryAndFallbackWorkflow(std::shared_ptr<ILogger> logger)
{
    std::cout << "\n*** DEMO 4: Retry & Fallback Workflow ***" << std::endl;
    std::cout << "Pipeline: EnrichContext -> Generate (with retry) -> Fallback(Summarize -> Translate)\n" << std::endl;

    auto generateWithRetry = std::make_shared<RetryStep>(
        std::make_shared<GenerateStep>("gpt-4"), 3);

    auto summarizeWithFallback = std::make_shared<FallbackStep>(
        std::make_shared<SummarizeStep>(100),
        std::make_shared<SummarizeStep>(500));

    Workflow robustWorkflow("Robust AI Pipeline", logger);
    robustWorkflow.addStep(std::make_shared<EnrichContextStep>("internal-docs"));
    robustWorkflow.addStep(generateWithRetry);
    robustWorkflow.addStep(summarizeWithFallback);

    StepResult result = robustWorkflow.run("How does our billing system handle refunds?");

    std::cout << "\nFinal Output: " << result.getOutput() << std::endl;
}

void demonstrateUserQueryWorkflow(std::shared_ptr<ILogger> logger)
{
    std::cout << "\n*** DEMO 5: User Query Processing Workflow ***" << std::endl;
    std::cout << "Pipeline: Enrich Context -> Generate AI Response -> Refine Tone -> Summarize\n" << std::endl;

    Workflow queryWorkflow("User Query Pipeline", logger);
    queryWorkflow.addStep(std::make_shared<EnrichContextStep>("knowledge-base"));
    queryWorkflow.addStep(std::make_shared<GenerateStep>("gpt-4"));
    queryWorkflow.addStep(std::make_shared<RefineToneStep>("friendly"));
    queryWorkflow.addStep(std::make_shared<SummarizeStep>(200));

    StepResult result = queryWorkflow.run("What is the return policy for electronics?");

    std::cout << "\nFinal Output: " << result.getOutput() << std::endl;
}

int main()
{
    std::cout << "============================================================" << std::endl;
    std::cout << "       AI PROMPT ORCHESTRATION ENGINE - DEMONSTRATION        " << std::endl;
    std::cout << "============================================================" << std::endl;

    auto logger = std::make_shared<WorkflowLogger>();

    demonstrateProductDescriptionWorkflow(logger);
    demonstrateDataAnalysisWorkflow(logger);
    demonstrateConditionalTranslationWorkflow(logger);
    demonstrateRetryAndFallbackWorkflow(logger);
    demonstrateUserQueryWorkflow(logger);

    std::cout << "\n============================================================" << std::endl;
    std::cout << "                   ALL DEMOS COMPLETE                        " << std::endl;
    std::cout << "============================================================" << std::endl;

    return 0;
}
