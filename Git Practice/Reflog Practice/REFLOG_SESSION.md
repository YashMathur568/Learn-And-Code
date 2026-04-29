# Git Reflog Practice - Complete Session

## Scenario: Recovering Lost Commits

### Initial State
```bash
git log --oneline -5
44d9a86 (HEAD) Commit 5: Add test cases
66ccae4 Commit 4: Divide function
9c9d25c Commit 3: Multiply function
1cad499 Commit 2: Subtract function
4b2fb7d Commit 1: Add function
```

### Step 1: Simulate Disaster - Hard Reset
We accidentally run a dangerous command that "loses" 5 commits:
```bash
git reset --hard HEAD~5
```

**Result:**
```
HEAD is now at 42f9d07 Added Coding Guidelines
```

Now all our recent commits have disappeared from the log!

### Step 2: Use Reflog to Investigate
We run reflog to see the history of HEAD movements:
```bash
git reflog
```

**Key Output:**
```
42f9d07 (HEAD, origin/main, main) HEAD@{0}: reset: moving to HEAD~5
44d9a86 HEAD@{1}: checkout: moving from a4e5d68...
a4e5d68 HEAD@{2}: checkout: moving from 66ccae4...
66ccae4 HEAD@{3}: checkout: moving from Git_Practice to 66ccae4...
19e6bd1 (Git_Practice) HEAD@{4}: checkout: moving from 7354af5...    ← Before the reset!
7354af5 HEAD@{5}: checkout: moving from a4e5d68...
...
19e6bd1 HEAD@{8}: commit: Commit 8: Add comprehensive tests with pass/fail
7354af5 HEAD@{9}: checkout: moving from 66ccae4...
7354af5 HEAD@{12}: commit: Commit 7: Add error handling
a4e5d68 HEAD@{13}: commit: Commit 6: Minor improvement
44d9a86 HEAD@{14}: commit: Commit 5: Add test cases
66ccae4 HEAD@{15}: commit: Commit 4: Divide function
9c9d25c HEAD@{16}: commit: Commit 3: Multiply function
1cad499 HEAD@{17}: commit: Commit 2: Subtract function
4b2fb7d HEAD@{18}: commit: Commit 1: Add function
```


### Step 4: Identify Recovery Point
Looking at reflog, you can see that `HEAD@{4}` or commit `19e6bd1` is the Git_Practice branch pointer before the reset happened.

Verify current state:
```bash
git log --oneline -3
19e6bd1 (HEAD -> Git_Practice) Commit 8: Add comprehensive tests with pass/fail
7354af5 Commit 7: Add error handling
a4e5d68 Commit 6: Minor improvement
```

### Step 5: Execute Recovery
Reset to the commit hash shown in reflog:
```bash
git reset --hard 19e6bd1
```

**Result:**
```
HEAD is now at 19e6bd1 Commit 8: Add comprehensive tests with pass/fail
```

### Step 6: Verify Success
```bash
git log --oneline -5
19e6bd1 (HEAD -> Git_Practice) Commit 8: Add comprehensive tests with pass/fail
7354af5 Commit 7: Add error handling
a4e5d68 Commit 6: Minor improvement
44d9a86 Commit 5: Add test cases
66ccae4 Commit 4: Divide function
```

**All commits recovered successfully!**