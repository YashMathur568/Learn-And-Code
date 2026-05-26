#pragma once

class IDeleteExpenseInteractor {
public:
    virtual ~IDeleteExpenseInteractor() = default;
    virtual void execute(int id) = 0;
};
