#!/usr/bin/env python3

import argparse
import os
import shutil
import sys

def copy_file(input_path, output_path):
    """复制单个文件（保留元数据）"""
    try:
        shutil.copy2(input_path, output_path)
    except Exception as e:
        print(f"Error copying {input_path}: {e}", file=sys.stderr)
        return False
    return True

def copy_directory(input_dir, output_dir):
    """递归复制整个文件夹"""
    try:
        if os.path.exists(output_dir):
            shutil.rmtree(output_dir)  # 删除已存在的目录（确保完全覆盖）
        shutil.copytree(input_dir, output_dir)
    except Exception as e:
        print(f"Error copying directory {input_dir}: {e}", file=sys.stderr)
        return False
    return True

def main():
    parser = argparse.ArgumentParser(description="Copy files/directories to a destination.")
    parser.add_argument("--dest", required=True, help="Destination directory.")
    parser.add_argument("--input", action="append", required=True, help="Input file or directory path.")
    parser.add_argument("--output", action="append", help="Output filename/dirname (optional).")
    args = parser.parse_args()

    if args.output and len(args.input) != len(args.output):
        print("Error: --input and --output counts must match.", file=sys.stderr)
        return 1

    # 确保目标目录存在
    os.makedirs(args.dest, exist_ok=True)

    success = True
    for i, input_path in enumerate(args.input):
        # 确定输出路径
        if args.output and args.output[i]:
            output_name = args.output[i]
        else:
            output_name = os.path.basename(os.path.normpath(input_path))  # 处理文件夹路径

        output_path = os.path.join(args.dest, output_name)

        # 判断是文件还是文件夹
        if os.path.isfile(input_path):
            success &= copy_file(input_path, output_path)
        elif os.path.isdir(input_path):
            success &= copy_directory(input_path, output_path)
        else:
            print(f"Error: {input_path} is not a valid file or directory.", file=sys.stderr)
            success = False

    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())