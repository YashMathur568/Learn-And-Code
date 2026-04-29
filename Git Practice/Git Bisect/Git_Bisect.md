# Git Bisect Practice Exercise

## Overview
This exercise demonstrates how to use `git bisect` to find the commit that introduced a bug in the codebase.

## Exercise Setup

### Commits Created
1. **Commit 1**: Basic add function
2. **Commit 2**: Add subtract function
3. **Commit 3**: Add multiply function ✗ **BUG INTRODUCED** (returns `a + b` instead of `a * b`)
4. **Commit 4**: Add divide function
5. **Commit 5**: Add test cases
6. **Commit 6**: Minor improvement
7. **Commit 7**: Add error handling
8. **Commit 8**: Add comprehensive tests with pass/fail

### The Bug
In **Commit 3**, the multiply function was implemented incorrectly:
```cpp
int multiply(int a, int b) {
    return a + b;
}
```

## Git Bisect Process

### Initial Setup
```bash
git bisect start
git bisect bad
git bisect good 4b2fb7d
```

Mark the current branch as bad (has the bug), and mark Commit 1 as good (working version).

### Bisect Session Output

#### Step 1: Bisecting - Commit 4 (Divide function)
```
Bisecting: 3 revisions left to test after this (roughly 2 steps)
[66ccae452edd028532832cd8f5b402eba42a2804] Commit 4: Divide function

$ g++ "Git Practice\Git Bisect\calculator.exe" -o calculator.exe && ./calculator.exe
Calculator Program

Result: NO BUG YET (No tests in output) → Mark as GOOD
$ git bisect good
```

#### Step 2: Bisecting - Commit 6 (Minor improvement)
```
Bisecting: 1 revision left to test after this (roughly 1 step)
[a4e5d68ae769900fe3a483b28936b74f1bf6eb44] Commit 6: Minor improvement

$ g++ "Git Practice\Git Bisect\calculator.exe" -o calculator.exe && ./calculator.exe
Calculator Program v1.1

Add: 8
Subtract: 2
Multiply: 8         ← BUG FOUND! (should be 15, got 8)
Divide: 2

Result: BUG PRESENT → Mark as BAD
$ git bisect bad
```

#### Step 3: Bisecting - Commit 5 (Add test cases)
```
Bisecting: 0 revisions left to test after this (roughly 0 steps)
[44d9a86c0ae0d5bf361a849db41249fd0c26f9c6] Commit 5: Add test cases

$ g++ "Git Practice\Git Bisect\calculator.exe" -o calculator.exe && ./calculator.exe
Calculator Program
Add: 8
Subtract: 2
Multiply: 8         ← BUG PRESENT
Divide: 2

Result: BUG PRESENT → Mark as BAD
$ git bisect bad
```

### Bisect Complete
```
44d9a86c0ae0d5bf361a849db41249fd0c26f9c6 is the first bad commit
commit 44d9a86c0ae0d5bf361a849db41249fd0c26f9c6 (HEAD)
Author: yashmathur865 <yashmathur865@gmail.com>
Date:   Wed Apr 29 14:22:18 2026 +0530

    Commit 5: Add test cases

 Git Practice/Git Bisect/calculator.cpp | 6 ++++++
 1 file changed, 6 insertions(+)
```

### Exit Bisect
```bash
git bisect reset
```