import argparse
import logging
import os
import sys

def param_sort(params):
    param_list = [(params[i], int(params[i + 1])) for i in range(0, len(params), 2)]
    param_list.sort(key=lambda x: x[1])
    return param_list

def get_file_size(file_path):
    return os.path.getsize(file_path)

def setup_logging():
    logging.basicConfig(level=logging.ERROR, format='[%(levelname)s][%(filename)s:%(lineno)d]%(message)s')

def parse_arguments():
    parser = argparse.ArgumentParser(description="Combine files at specific offsets into a single binary file.")
    parser.add_argument("outfile", type=str, help="The output file name, default is output.bin")
    parser.add_argument("size", type=int, help="The output file size limit")
    parser.add_argument("files", nargs='+', help="List of files and their offsets (e.g., file1 offset1 file2 offset2 ...)")
    return parser.parse_args()

def main():
    setup_logging()
    args = parse_arguments()

    if len(args.files) < 2 or len(args.files) % 2 != 0:
        logging.error("Files and offsets must be provided in pairs.")
        sys.exit(1)

    sorted_files = param_sort(args.files)
    total_size = 0
    occupied = 0

    for i, (file_path, ofs) in enumerate(sorted_files):
        if not os.path.exists(file_path):
            logging.error(f"File not found: {file_path}")
            sys.exit(1)

        file_size = get_file_size(file_path)
        logging.info(f"file: {file_path} ofs: {ofs} size: {file_size}")

        if ofs + file_size > args.size:
            logging.error(f"{file_path} is too big or its offset is incorrect.")
            sys.exit(2)

        if ofs < occupied:
            logging.error("Address conflicts.")
            logging.error(f"{ofs} --- {occupied}")
            sys.exit(3)

        occupied = ofs + file_size
        total_size += file_size

    if total_size > args.size:
        logging.error("Error: total file size is greater than output file size")
        sys.exit(3)

    if os.path.exists(args.outfile):
        os.remove(args.outfile)

    logging.info("--- start -----------------")
    with open(args.outfile, 'wb') as fout:
        for i, (file_path, ofs) in enumerate(sorted_files):
            with open(file_path, 'rb') as fin:
                file_data = fin.read()
                fout.seek(ofs)
                fout.write(file_data)
                logging.info(f"write {file_path}, seek {ofs}, write {len(file_data)}")
                logging.info("--------------------")
    logging.info("--- end -----------------")

if __name__ == "__main__":
    main()