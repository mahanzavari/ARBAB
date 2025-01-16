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
    
        // CREATE TABLE command
        System.out.println("1. CREATE TABLE <table_name> <num_columns>");
        System.out.println("   - Creates a new table with the specified name and number of columns.");
        System.out.println("   - After entering this command, you will be prompted to define the column names, types, and constraints.");
        System.out.println("   - Example: CREATE TABLE students 3\n");
    
        // DELETE TABLE command
        System.out.println("2. DELETE TABLE <table_name>");
        System.out.println("   - Deletes the specified table and all its records.");
        System.out.println("   - Example: DELETE TABLE students\n");
    
        // CREATE INDEX command
        System.out.println("3. CREATE INDEX <table_name>");
        System.out.println("   - Creates an index on the primary key (student-number) of the specified table.");
        System.out.println("   - Example: CREATE INDEX students\n");
    
        // ADD command
        System.out.println("4. ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>");
        System.out.println("   - Adds a new record to the specified table.");
        System.out.println("   - Example: ADD students student-number 123 name \"John Doe\" score 85\n");
    
        // DELETE command
        System.out.println("5. DELETE <table_name> <column_name> <value>");
        System.out.println("   - Deletes records from the specified table where the column matches the value.");
        System.out.println("   - Example: DELETE students student-number 123\n");
    
        // UPDATE command
        System.out.println("6. UPDATE <table_name> <column_name> <old_value> <new_value>");
        System.out.println("   - Updates records in the specified table where the column matches the old value.");
        System.out.println("   - Example: UPDATE students score 85 90\n");
    
        // SELECT command
        System.out.println("7. SELECT <table_name> <column_name> <value> [SORTED]");
        System.out.println("   - Selects records from the specified table where the column matches the value.");
        System.out.println("   - Use the optional SORTED keyword to sort the results by student-number.");
        System.out.println("   - Example: SELECT students score 85 SORTED\n");
    
        // SAVE command
        System.out.println("8. SAVE <table_name> <filename>");
        System.out.println("   - Saves the specified table to a CSV file.");
        System.out.println("   - Example: SAVE students data.csv\n");
    
        // LOAD command
        System.out.println("9. LOAD <table_name> <filename>");
        System.out.println("   - Loads the specified table from a CSV file.");
        System.out.println("   - Example: LOAD students data.csv\n");
    
        // HELP command
        System.out.println("10. HELP");
        System.out.println("    - Displays this help message.\n");
    
        // EXIT command
        System.out.println("11. EXIT");
        System.out.println("    - Exits the program.\n");
    
        // Additional information
        System.out.println("Note:");
        System.out.println("- Column types can be either 'INTEGER' or 'STRING'.");
        System.out.println("- Constraints can include 'UNIQUE', 'PRIMARY KEY', and 'NOT NULL'.");
        System.out.println("- The 'general-course-score' and 'core-course-score' columns must be between 0 and 20.");
    }
}