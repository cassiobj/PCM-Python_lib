# PCM-Python_lib

PCM-Python Installation Instructions

Overview

PCM-Python is a Python 2.7 extension for Oracle BRM 7.5, provided as a shared object file (PCMPy.so). This extension allows Python applications to interact with the Oracle BRM system efficiently.

Prerequisites

1. Python 2.7: Ensure that Python 2.7 is installed on your system.
2. Oracle BRM 7.5: Ensure that Oracle BRM 7.5 is installed and properly configured.
3. Development Tools: Make sure you have the necessary development tools installed, such as GCC (GNU Compiler Collection) and the python-dev package.

Installation Steps

1. Download the PCM-Python Extension

Download the PCMPy.so file from the provided source or repository.

2. Place the PCMPy.so File

You have two options for placing the PCMPy.so file:

Option A: Place in Project Directory

Move the PCMPy.so file to an appropriate directory in your project where Python can access it:

    mv PCMPy.so /path/to/your/project/

Option B: Place in Python's Default Library Directory

Move the PCMPy.so file to Python's default library directory. You can find the directory by running the following Python command:

    import site
    print(site.getsitepackages())

Move the PCMPy.so file to one of the directories listed:

    mv PCMPy.so /path/to/python/lib/directory/

3. Verify the Installation

Create a simple Python script to verify that the extension is loaded correctly:

    import os
    import sys

    # Add the directory containing PCMPy.so to the system path if necessary
    sys.path.append('/path/to/your/project/')  # Only needed if using Option A

    try:
        import PCMPy
        print("PCM-Python extension loaded successfully.")
    except ImportError:
        print("Failed to load the PCM-Python extension.")

Run the script:

    python verify_pcm_py.py

If the extension is loaded successfully, you should see the message "PCM-Python extension loaded successfully."

4. Usage

You can now use the PCM-Python extension in your Python scripts. For example:

    import PCMPy

    # Attempt to connect to the Context Manager
    result = PCMPy.connect_cm()
    if result:
        print("Connection established successfully!")
    else:
        print("Failed to establish connection.")

Troubleshooting

- ImportError: If you encounter an ImportError, ensure that the directory containing PCMPy.so is included in your PYTHONPATH, or place the file in Python's default library directory.
- Compatibility Issues: Verify that you are using Python 2.7 and Oracle BRM 7.5. Compatibility with other versions is not guaranteed.

Support

For further assistance, please refer to the official documentation or contact support at cassiobj@gmail.com.

© 2024 Cássio B. Jemael. All rights reserved.

