#coding=utf-8

import os
import logging

#
## log config
#
FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'
logging.basicConfig(
    level=logging.INFO,
    format=FORMAT,
    datefmt='%H:%M:%S'
)

class FileLock:
    def __init__(self, lockName, path):
        self.lockName = lockName
        self.file = os.path.join(path, lockName)

    def lock(self):
        # 如果文件存在，说明已经被锁定
        try:
            if os.path.exists(self.file):
                logging.debug(f'It is locked: {self.file}')
            else:
                self.fileHandle = open(self.file, 'w')
                self.fileHandle.write('lock')
                self.fileHandle.flush()
                self.fileHandle.close()
                logging.debug(f'Create lock file: {self.file}')
            return True
        except Exception as e:
            logging.error('lock file failed: ' + str(e))
            return False

    def unlock(self):
        try:
            if os.path.exists(self.file):
                os.remove(self.file)
                logging.debug(f'Remove lock file: {self.file}')
            else:
                logging.debug(f'Lock file not exists: {self.file}')
            return True
        except Exception as e:
            logging.error('unlock file failed: ' + str(e))
            return False

    def isLocked(self):
        return os.path.exists(self.file)
