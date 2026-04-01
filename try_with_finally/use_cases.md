# Try-Finally Use Cases in Java

The finally block in Java ensures that certain code always executes, whether an exception occurs or not. It is commonly used for cleanup operations.

## General Structure

```java
try {
    // risky code
} finally {
    // cleanup code (always runs)
}
```

## 1. Closing Files

When working with files, we must close the file even if an exception occurs.

```java
import java.io.*;

public class FileExample {
    public static void main(String[] args) {

        FileInputStream file = null;

        try {
            file = new FileInputStream("data.txt");
            int data = file.read();
            System.out.println(data);
        } 
        catch (Exception e) {
            System.out.println("Error reading file");
        } 
        finally {
            try {
                if (file != null) {
                    file.close();
                    System.out.println("File closed");
                }
            } 
            catch (Exception e) {
                System.out.println("Error closing file");
            }
        }
    }
}
```

**Why use finally?**
Even if reading fails, the file will still be closed.

## 2. Releasing Locks

In multithreaded programs, we must always release locks to avoid deadlocks.

```java
import java.util.concurrent.locks.*;

public class LockExample {

    static Lock lock = new ReentrantLock();

    public static void main(String[] args) {

        lock.lock();

        try {
            System.out.println("Critical section");
        } 
        finally {
            lock.unlock();
            System.out.println("Lock released");
        }
    }
}
```

**Why use finally?**
If an exception occurs inside the critical section, the lock will still be released.

## 3. Closing Database Connections

Database connections should always be closed.

```java
Connection conn = null;

try {
    conn = DriverManager.getConnection("db-url");
    System.out.println("Query executed");
}
catch (Exception e) {
    System.out.println("DB error");
}
finally {
    if (conn != null) {
        conn.close();
        System.out.println("Connection closed");
    }
}
```

**Why use finally?**
It guarantees that the connection is closed even if the query fails.

## 4. Releasing System Resources

Resources such as sockets, streams, or buffers must be cleaned up.

```java
Socket socket = null;

try {
    socket = new Socket("example.com", 80);
    System.out.println("Connected");
}
catch (Exception e) {
    System.out.println("Connection failed");
}
finally {
    if (socket != null) {
        socket.close();
        System.out.println("Socket closed");
    }
}
```

## 5. Logging or Cleanup Actions

Sometimes we want to log completion or perform cleanup tasks.

```java
try {
    System.out.println("Processing task...");
}
finally {
    System.out.println("Task finished (success or failure)");
}
```

---

## Summary

The `finally` block is essential for ensuring that critical cleanup operations always execute, preventing resource leaks and maintaining system stability even when errors occur.
