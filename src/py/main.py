import ctypes
import os
import sys
from concurrent.futures import ThreadPoolExecutor

# Load the shared library
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./corelogic.dll")
else:  # Unix-based systems (Linux, macOS)
    lib = ctypes.CDLL("./libcorelogic.so")

# Define the native function signatures
lib.createTableCmd.argtypes = [ctypes.c_char_p]
lib.deleteTableCmd.argtypes = [ctypes.c_char_p]
lib.createIndexCmd.argtypes = [ctypes.c_char_p]
lib.addRecordCmd.argtypes = [ctypes.c_char_p]
lib.deleteRecordCmd.argtypes = [ctypes.c_char_p]
lib.updateRecordCmd.argtypes = [ctypes.c_char_p]
lib.selectRecordsCmd.argtypes = [ctypes.c_char_p]
lib.selectRecordsWhereCmd.argtypes = [ctypes.c_char_p]
lib.beginTransaction.argtypes = []
lib.commitTransaction.argtypes = []
lib.rollbackTransaction.argtypes = []
lib.saveCmd.argtypes = [ctypes.c_char_p]
lib.loadCmd.argtypes = [ctypes.c_char_p]
lib.saveToBinary.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.loadFromBinary.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

# Function to handle commands
def handle_command(command):
    if command.startswith("CREATE TABLE"):
        lib.createTableCmd(command.encode("utf-8"))
    elif command.startswith("DELETE TABLE"):
        lib.deleteTableCmd(command.encode("utf-8"))
    elif command.startswith("CREATE INDEX"):
        lib.createIndexCmd(command.encode("utf-8"))
    elif command.startswith("ADD"):
        lib.addRecordCmd(command.encode("utf-8"))
    elif command.startswith("DELETE"):
        lib.deleteRecordCmd(command.encode("utf-8"))
    elif command.startswith("UPDATE"):
        lib.updateRecordCmd(command.encode("utf-8"))
    elif command.startswith("SELECT"):
        if "WHERE" in command:
            lib.selectRecordsWhereCmd(command.encode("utf-8"))
        else:
            lib.selectRecordsCmd(command.encode("utf-8"))
    elif command == "HELP":
        help()
    elif command == "BEGIN":
        lib.beginTransaction()
    elif command == "COMMIT":
        lib.commitTransaction()
    elif command == "ROLLBACK":
        lib.rollbackTransaction()
    elif command.startswith("SAVE BINARY"):
        parts = command.split(" ")
        if len(parts) == 4:
            lib.saveToBinary(parts[2].encode("utf-8"), parts[3].encode("utf-8"))
        else:
            print("Invalid SAVE BINARY command.")
    elif command.startswith("LOAD BINARY"):
        parts = command.split(" ")
        if len(parts) == 4:
            lib.loadFromBinary(parts[2].encode("utf-8"), parts[3].encode("utf-8"))
        else:
            print("Invalid LOAD BINARY command.")
    elif command == "exit":
        sys.exit(0)
    else:
        print("Invalid command. Type 'HELP' for a list of commands.")

# Function to display help
def help():
    print("\nAvailable Commands:")
    print("-------------------")
    print("1. CREATE TABLE <table_name> <num_columns>")
    print("2. DELETE TABLE <table_name>")
    print("3. CREATE INDEX <table_name>")
    print("4. ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>")
    print("5. DELETE <table_name> <column_name> <value>")
    print("6. UPDATE <table_name> <column_name> <old_value> <new_value>")
    print("7. SELECT <table_name> <column_name> <value> [SORTED]")
    print("8. SAVE <table_name> <filename>")
    print("9. LOAD <table_name> <filename>")
    print("10. HELP")
    print("11. EXIT")

# Main function
def main():
    executor = ThreadPoolExecutor(max_workers=10)  # Thread pool for multi-threading
    while True:
        command = input("Enter command: ")
        executor.submit(handle_command, command)

if __name__ == "__main__":
    main()