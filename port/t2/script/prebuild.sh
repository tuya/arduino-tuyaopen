#!/usr/bin/env bash

VENV_DIR=${1}
SCRIPT_DIR=${2}

PYTHON_SCRIPT_ARG1=${3}
PYTHON_SCRIPT_ARG2=${4}
PYTHON_SCRIPT_ARG3=${5}
PYTHON_SCRIPT_ARG4=${6}
PYTHON_SCRIPT_ARG5=${7}
PYTHON_SCRIPT_ARG6=${8}
PYTHON_SCRIPT_ARG7=${9}
PYTHON_SCRIPT_ARG8=${10}

# 检测Python可执行文件
if command -v python3 &>/dev/null; then
    PYTHON_CMD=python3
elif command -v python &>/dev/null && python --version | grep -q '^Python 3'; then
    PYTHON_CMD=python
else
    echo "Python 3 is not installed."
    exit 1
fi

# 检查虚拟环境是否存在
if [ ! -d "${VENV_DIR}" ]; then
    echo "Virtual environment not found. Creating one..."
    $PYTHON_CMD -m venv "${VENV_DIR}" || { echo "Failed to create virtual environment."; exit 1; }
    echo "Virtual environment created at ${VENV_DIR}"
else
    echo "Virtual environment already exists."
fi

# 指定虚拟环境的Python可执行文件
VENV_PYTHON_CMD="${VENV_DIR}bin/python3"

# 激活虚拟环境
ACTIVATE_SCRIPT="${VENV_DIR}bin/activate"
if [ -f "$ACTIVATE_SCRIPT" ]; then
    echo "Activate python virtual environment."
    source "$ACTIVATE_SCRIPT"
else
    echo "Activate script not found."
    exit 1
fi

# 检查需要的 Python 库是否安装
$VENV_PYTHON_CMD -m pip install -r "${SCRIPT_DIR}/requirements.txt" || { echo "Failed to install required Python packages."; deactivate; exit 1; }

# 执行 Python 脚本
$VENV_PYTHON_CMD "${SCRIPT_DIR}/prebuild.py" "${PYTHON_SCRIPT_ARG1}" "${PYTHON_SCRIPT_ARG2}" "${PYTHON_SCRIPT_ARG3}" "${PYTHON_SCRIPT_ARG4}" "${PYTHON_SCRIPT_ARG5}" "${PYTHON_SCRIPT_ARG6}" "${PYTHON_SCRIPT_ARG7}" "${PYTHON_SCRIPT_ARG8}" || { echo "Failed to run $PYTHON_SCRIPT"; deactivate; exit 1; }

# 退出虚拟环境
echo "Deactivate python virtual environment."
deactivate
