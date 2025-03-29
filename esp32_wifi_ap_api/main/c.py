import os
import sys

def get_gitignore_exclusions(directory):
    """Reads the .gitignore file and returns a set of excluded file/directory patterns."""
    excluded_patterns = {'.mypy_cache', 'venv', 'node_modules', 'package-lock.json', '__pycache__', 'combined_code.txt'}
    gitignore_path = os.path.join(directory, '.gitignore')
    if os.path.isfile(gitignore_path):
        with open(gitignore_path, 'r') as gitignore:
            for line in gitignore:
                line = line.strip()
                if line and not line.startswith('#'):
                    excluded_patterns.add(line)
    return excluded_patterns

def should_exclude(path, excluded_patterns, script_name):
    """Checks if a file or directory matches any exclusion pattern."""
    if os.path.basename(path) == script_name:
        return True
    for pattern in excluded_patterns:
        if path.endswith(pattern) or pattern in path:
            return True
    return False

def filter_print_statements(line):
    """Filters out print statements or lines with error output."""
    return 'stdout' not in line and 'Could not read file' not in line

def combine_code_from_directories(include_dirs, output_file, exclude_patterns=None):
    """Combines code from multiple directories into a single file, excluding specified paths."""
    
    # Initialize exclusions
    excluded_patterns = set()
    if exclude_patterns:
        excluded_patterns.update(exclude_patterns)
    
    # Get the script name itself to exclude it
    script_name = os.path.basename(sys.argv[0])
    excluded_patterns.add(script_name)

    with open(output_file, 'w') as outfile:
        for directory in include_dirs:
            directory = directory.strip()
            if not os.path.isdir(directory):
                print(f"Skipping invalid directory: {directory}")
                continue

            excluded_patterns.update(get_gitignore_exclusions(directory))

            for root, dirs, files in os.walk(directory):
                # Remove excluded directories from traversal
                dirs[:] = [d for d in dirs if not should_exclude(os.path.join(root, d), excluded_patterns, script_name)]
                
                for file in files:
                    file_path = os.path.join(root, file)
                    # Skip excluded files
                    if should_exclude(file_path, excluded_patterns, script_name):
                        continue
                    try:
                        with open(file_path, 'r') as infile:
                            # Write the file name before the content
                            outfile.write(f"// File: {file_path}\n")
                            for line in infile:
                                # Filter out print statements or error outputs
                                if filter_print_statements(line):
                                    outfile.write(line)
                            outfile.write("\n\n")  # Add spacing between files
                    except Exception as e:
                        print(f"Could not read file {file_path}: {e}")

def print_directory_structure(directory, excluded_patterns, prefix=""):
    """Prints the directory structure while skipping excluded files and directories."""
    try:
        items = os.listdir(directory)
    except PermissionError:
        print(f"{prefix} [Permission Denied] {directory}")
        return

    items.sort()  # Sort for consistent output

    for index, item in enumerate(items):
        item_path = os.path.join(directory, item)
        is_last_item = (index == len(items) - 1)
        
        # Determine the prefix for the current item
        if is_last_item:
            new_prefix = prefix + "└── "
            next_prefix = prefix + "    "
        else:
            new_prefix = prefix + "├── "
            next_prefix = prefix + "│   "
        
        # Skip excluded directories and files
        if should_exclude(item_path, excluded_patterns, script_name=""):
            continue

        # Print the current item
        print(new_prefix + item)
        
        # If the current item is a directory, recursively print its contents
        if os.path.isdir(item_path):
            print_directory_structure(item_path, excluded_patterns, next_prefix)

if __name__ == "__main__":
    # Prompt the user to enter multiple directory paths
    include_dirs = input("Enter directories to combine code from (comma-separated): ").split(",")
    
    # Define the output file name
    output_file = "combined_code.txt"
    
    # Additional exclusions
    additional_exclusions = input("Enter any additional files or directories to exclude (comma-separated): ").split(",")
    additional_exclusions = [exclusion.strip() for exclusion in additional_exclusions if exclusion.strip()]

    # Combine the code from the specified directories
    combine_code_from_directories(include_dirs, output_file, additional_exclusions)

    # Print the directory structures of included directories
    print("\nDirectory Structure:\n")
    for directory in include_dirs:
        directory = directory.strip()
        if os.path.isdir(directory):
            print(f"\n{directory}/")
            print_directory_structure(directory, additional_exclusions)
    
    print(f"\nAll code has been combined into {output_file}")