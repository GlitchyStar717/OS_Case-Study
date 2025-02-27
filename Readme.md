# Pintos Modifications - README

This document outlines the steps to install, build, run, and test the modifications made to the Pintos operating system.

## Step 1: Install Prerequisites

This installation process has been tested on Linux Mint 24 Cinnamon edition and Ubuntu 22.04 LTS. Compatibility with other distributions/versions/OS is not guaranteed.

1.  **Boot up your Mint/Ubuntu system.**

2.  **Install necessary dependencies:**

    ```bash
    sudo apt update && sudo apt upgrade -y
    sudo apt install build-essential gdb gcc-multilib make -y 
    sudo apt install python3 libgmp-dev libmpfr-dev libisl-dev -y 
    sudo apt install perl libmpc-dev qemu-system -y 
    ```

    These packages provide essential development tools, a QEMU emulator, and dependencies required to build Pintos.

## Step 2: Download Pintos Source Code

1.  **Clone the Pintos repository from GitHub:**

    ```bash
    git clone [https://github.com/GlitchyStar717/OS_Case-Study](https://github.com/GlitchyStar717/OS_Case-Study)
    cd pintos
    ```

    If using a different source, replace the repository URL accordingly.

## Step 3: Configure the Environment

1.  **Set necessary environment variables:**

    ```bash
    export PATH=/path/to/pintos/src/utils:$PATH
    ```

    Replace `/path/to/pintos` with the actual path to your Pintos directory.

2.  **Make this change permanent:**

    ```bash
    echo 'export PATH=/path/to/pintos/src/utils:$PATH' >> ~/.bashrc
    source ~/.bashrc
    ```

    Replace `/path/to/pintos` with the actual path to your Pintos directory.

    * Alternatively, you can add the export command to `~/.profile` instead of `~/.bashrc`.

## Step 4: Compile Pintos

1.  **Navigate to the `src/threads` directory and compile the source:**

    ```bash
    cd src/threads
    make
    ```

2.  **Compile the utils directory:**

    ```bash
    cd ../utils
    make
    ```

## Step 5: Run Pintos with QEMU

1.  **Navigate to the `src/threads/build` directory:**

    ```bash
    cd ../threads/build
    ```

2.  **Run Pintos with QEMU:**

    ```bash
    pintos run alarm-single
    ```

    This command will test the successful compilation of Pintos. You can replace `alarm-single` with other test names as needed.

## Additional Notes

* Ensure you replace `/path/to/pintos` with the correct path to your Pintos directory in the environment variable configuration.
* If you encounter any issues, double-check that all dependencies are installed correctly and that the environment variables are set properly.
* For further testing, explore other Pintos test cases by using `pintos run <test-name>`.
* Remember to consult the Pintos documentation for more detailed information and troubleshooting tips.