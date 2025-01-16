package app;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    static {
        try {
            System.out.println("Loading corelogic.dll...");
            System.loadLibrary("corelogic"); // Load the shared library (libcorelogic.so or corelogic.dll)
            System.out.println("Library loaded successfully.");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load corelogic library: " + e.getMessage());
            System.exit(1);
        }
    }

    // Declare native methods for C functions
    public native synchronized void createTableCmd(String command);
    public native synchronized void deleteTableCmd(String command);
    public native synchronized void createIndexCmd(String command);
    public native synchronized void addRecordCmd(String command);
    public native synchronized void deleteRecordCmd(String command);
    public native synchronized void updateRecordCmd(String command);
    public native synchronized void selectRecordsCmd(String command);
    public native synchronized void selectRecordsWhereCmd(String command);
    public native synchronized void beginTransaction();
    public native synchronized void commitTransaction();
    public native synchronized void rollbackTransaction();
    public native synchronized void saveCmd(String command);
    public native synchronized void loadCmd(String command);
    public native synchronized void saveToBinary(String tableName, String filename);
    public native synchronized void loadFromBinary(String tableName, String filename);

    public static void main(String[] args) {
        ExecutorService executor = Executors.newFixedThreadPool(10); // Thread pool for multi-threading
        Scanner scanner = new Scanner(System.in);
        Main main = new Main();

        try {
            while (true) {
                System.out.print("Enter command: ");
                String command = scanner.nextLine();

                if (command.equals("exit")) {
                    executor.shutdown();
                    scanner.close();
                    System.out.println("Exiting...");
                    break;
                }

                // Submit the command to the thread pool for execution
                executor.submit(() -> main.handleCommand(command));
            }
        } finally {
            if (!executor.isShutdown()) {
                executor.shutdown();
            }
            scanner.close();
        }
    }

    private void handleCommand(String command) {
        System.out.println("Handling command: " + command);
        try {
            if (command.startsWith("CREATE TABLE")) {
                // System.out.println("Invoking createTableCmd...");
                createTableCmd(command); // Call the native method
                System.out.println("createTableCmd executed successfully.");
            } else if (command.startsWith("DELETE TABLE")) {
                deleteTableCmd(command);
            } else if (command.startsWith("CREATE INDEX")) {
                createIndexCmd(command);
            } else if (command.startsWith("ADD")) {
                addRecordCmd(command);
            } else if (command.startsWith("DELETE")) {
                deleteRecordCmd(command);
            } else if (command.startsWith("UPDATE")) {
                updateRecordCmd(command);
            } else if (command.startsWith("SELECT")) {
                if (command.contains("WHERE")) {
                    selectRecordsWhereCmd(command);
                } else {
                    selectRecordsCmd(command);
                }
            } else if (command.equals("HELP")) {
                help();
            } else if (command.equals("BEGIN")) {
                beginTransaction();
            } else if (command.equals("COMMIT")) {
                commitTransaction();
            } else if (command.equals("ROLLBACK")) {
                rollbackTransaction();
            } else if (command.startsWith("SAVE BINARY")) {
                String[] parts = command.split(" ");
                if (parts.length == 4) {
                    saveToBinary(parts[2], parts[3]);
                } else {
                    System.out.println("Invalid SAVE BINARY command.");
                }
            } else if (command.startsWith("LOAD BINARY")) {
                String[] parts = command.split(" ");
                if (parts.length == 4) {
                    loadFromBinary(parts[2], parts[3]);
                } else {
                    System.out.println("Invalid LOAD BINARY command.");
                }
            } else {
                System.out.println("Invalid command. Type 'HELP' for a list of commands.");
            }
        } catch (Exception e) {
            System.out.println("Error handling command: " + e.getMessage());
        }
    }

    private void help() {
        System.out.println("\nAvailable Commands:");
        System.out.println("-------------------");
        System.out.println("1. CREATE TABLE <table_name> <num_columns>");
        System.out.println("2. DELETE TABLE <table_name>");
        System.out.println("3. CREATE INDEX <table_name>");
        System.out.println("4. ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>");
        System.out.println("5. DELETE <table_name> <column_name> <value>");
        System.out.println("6. UPDATE <table_name> <column_name> <old_value> <new_value>");
        System.out.println("7. SELECT <table_name> <column_name> <value> [SORTED]");
        System.out.println("8. SAVE <table_name> <filename>");
        System.out.println("9. LOAD <table_name> <filename>");
        System.out.println("10. HELP");
        System.out.println("11. EXIT");
    }
}
